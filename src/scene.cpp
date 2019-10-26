#include <iostream>

#include "scene.hpp"

static const int MAX_LEVEL = 4;

void Scene::traceRay(Eigen::Vector3f *color, Ray &ray, int level, Hitable* exclude) {
  //This variable will hold the value of t on intersection in the formula r(t) = o + t * d 
  float tOnIntersection = std::numeric_limits<float>::infinity();

  Hitable* hitObject;
  for ( Triangle* t : this->trianglesInScene ) 
    if ( t->intersect(tOnIntersection,ray,exclude) != NULL ) 
      hitObject = t;

  //If we reach this point, it means the ray hitted a object. Now we should compute the color of this object, so we call the shade method.
  *color = shade(hitObject, ray, tOnIntersection, level);
}


// ray tracing with accleration structure
void Scene::traceRayWithAcc(Eigen::Vector3f *color, Ray &ray, int level, Hitable* exclude) {
  //This variable will hold the value of t on intersection in the formula r(t) = o + t * d 
  float tOnIntersection = std::numeric_limits<float>::infinity();

  //This variable will hold the hitable which the ray intersects first (or NULL if no intersection)
  Hitable* hitObject = this->boxOverAllTriangles->intersect(tOnIntersection, ray, exclude);

  //If the hitObject is not set to anything, we know we did not hit anything, so use background color and return
  if ( hitObject == NULL ) 
  {
    *color = Eigen::Vector3f(0.0, 0.0, 0.0);
    return;
  }

  //If we reach this point, it means the ray hitted a object. Now we should compute the color of this object, so we call the shade method.
  *color = shade(hitObject, ray, tOnIntersection, level);
}


//TODO For now this only copies triangles
Scene::Scene(Tucano::Mesh &mesh, std::vector<Tucano::Material::Mtl> &materials)
{
	this->materials = &materials;
	for (size_t i = 0; i < mesh.getNumberOfFaces(); ++i)
  {
		Tucano::Face f = mesh.getFace(i);
		int a = f.vertex_ids[0];
		int b = f.vertex_ids[1];
		int c = f.vertex_ids[2];
		this->trianglesInScene.push_back(new Triangle(
      mesh.getShapeMatrix() * mesh.getVertex(a).head<3>(),
		  mesh.getShapeMatrix() * mesh.getVertex(b).head<3>(),
		  mesh.getShapeMatrix() * mesh.getVertex(c).head<3>(),
		  f.normal.head<3>(),
      f.material_id));
	}
  boxOverAllTriangles = new Box(this->trianglesInScene); 
  // this->leafBoxes = firstBox -> getLeafBoxes(); 
}


Scene::Scene()
{}

Eigen::Vector3f Scene::shade(Hitable *hitObject, Ray &ray, float t, int level) {
  Eigen::Vector3f color = Eigen::Vector3f(0.0,0.0,0.0);
  // Compute direct light
  color += computeDirectLight(hitObject, ray.getPoint(t));

  // Compute reflected light
  Tucano::Material::Mtl hitMaterial = materials -> at(hitObject->material_id);
  float illuminationModel = hitMaterial.getIlluminationModel();
  if ( illuminationModel == 4 && level < MAX_LEVEL ) 
    color += computeReflectedLight(hitObject, ray, t, level);
  
  // Compute refracted light
  // if (illuminationModel == 6 && level < MAX_LEVEL) {
  if (level < MAX_LEVEL) {
    float n1 = 1.0; // air
    float n2 = hitMaterial.getOpticalDensity();
    color += computeRefractedLight(hitObject, ray, t, level, n1, n2);
  }
  return color;
}

/****************************************************************
 * Compute Light (direct+reflect+refraction)                    *
 ****************************************************************/


Eigen::Vector3f Scene::computeRefractedLight(Hitable *hitObject, Ray &ray, float t, int level, float n1, float n2) {
  Eigen::Vector3f color = Eigen::Vector3f(0, 0, 0);
  Eigen::Vector3f in = ray.direction.normalized();
  Eigen::Vector3f norm = hitObject->normal.normalized();

  float cosTheta = in.dot(norm);
  float coeff = n1 / n2;
  Eigen::Vector3f part1 = coeff * (in -  cosTheta * norm);
  Eigen::Vector3f part2 = norm * std::sqrt(1 - std::pow(coeff, 2)*(1-std::pow(cosTheta, 2)));
  Eigen::Vector3f refractedVec = part1 - part2;

  Ray refractedRay = Ray(ray.getPoint(t), refractedVec);

  if (useAcc) {
    Scene::traceRayWithAcc(&color, refractedRay, level+1, hitObject);
  }
  else {
    Scene::traceRay(&color, refractedRay, level+1, hitObject);
  }
  return color;
}


Eigen::Vector3f Scene::computeReflectedLight(Hitable *hitObject, Ray &ray, float t, int level) {
    Eigen::Vector3f color = Eigen::Vector3f(0.0,0.0,0.0);
    
    //Calculate the outgoing vector 
    // R = V - 2 * (VN)N
    Eigen::Vector3f reflectedVec = ray.direction.normalized() - 2 *(ray.direction.normalized().dot(hitObject->normal.normalized()))*hitObject->normal.normalized();
    
    //Create ray
    Ray reflectedRay = Ray(ray.getPoint(t), reflectedVec);
    //Call correct trace function with the level increased by one.
    if (useAcc) {
      Scene::traceRayWithAcc(&color, reflectedRay, level+1, hitObject);
    }
    else {
      Scene::traceRay(&color, reflectedRay, level+1, hitObject);
    }
    return color;
}


Eigen::Vector3f Scene::computeDirectLight(Hitable *hitObject, Eigen::Vector3f hitPoint) {
  //Get the material properties of the hitable object this ray interesected with 
  Tucano::Material::Mtl faceMaterial = materials -> at(hitObject->material_id);
  Eigen::Vector3f kd = faceMaterial.getDiffuse();
  Eigen::Vector3f ks = faceMaterial.getSpecular();
  float shininess = faceMaterial.getShininess();

  Eigen::Vector3f faceNormal = hitObject->normal.normalized();

  Eigen::Vector3f color = Eigen::Vector3f(0.0, 0.0, 0.0);
  
  //Loop over all the lights in the scene
  for ( int i = 0; i < lights.size(); i++ ) {
      //Get the current light
      Light* currentLight = lights.at(i);
      std::vector<Eigen::Vector3f> pointLights = currentLight->getAllPointLights();
      Eigen::Vector3f shading = Eigen::Vector3f(0.0,0.0,0.0);

      for ( int j = 0; j < pointLights.size(); j++ ) {
        //Build ray to the light
        Ray rayToLight = Ray(hitPoint, pointLights.at(j) - hitPoint);
        //Check if there is an object between the hitpoint and the light, if so skip this light source
        float maxt = 1.0;
        Hitable* hit = this->boxOverAllTriangles->intersect(maxt, rayToLight, hitObject);
        if ( hit != NULL ) continue;
        
        //If we reach this point, it means that the current light can reach the object, so we compute the shading
        Eigen::Vector3f lightSpectrum = currentLight->spectrum / pointLights.size();

        //Normailized vector from the hitpoint to the light
        Eigen::Vector3f directionNormalized = rayToLight.direction.normalized();
        //Normalized vector from the hitpoint to the eye (camera)
        Eigen::Vector3f eyeDirection = (cameraPosition-hitPoint).normalized();
        
        //Compute the cosinus of the angle between the face normal and the ray to the light
        float cosinus = faceNormal.dot(directionNormalized);
        //If this cosinus is smaller than 0, we set it to zero.
        cosinus = cosinus > 0 ? cosinus : 0;
        //Compute the diffuse component
        Eigen::Vector3f diffuse = lightSpectrum.cwiseProduct(kd) * cosinus;
      
        //Compute the normalized vector of the reflected light ray (reflected in the normal)
        Eigen::Vector3f reflectedLight = (2 * faceNormal.dot(directionNormalized) * faceNormal - directionNormalized).normalized();
        //Compute the cosinus of the angle between the reflected light and the eye direction
        cosinus = reflectedLight.dot( eyeDirection );
        //If this cosinus is smaller than 0, we set it to zero.
        cosinus = cosinus > 0 ? cosinus : 0;
        //Raise the cosinus to the power shininess
        cosinus = std::pow(cosinus, shininess);
        //Compute the specular component
        Eigen::Vector3f specular = lightSpectrum.cwiseProduct(ks) * cosinus;
      
        shading += diffuse + specular;
      }
      color += shading;
  }
  return color;
}


/****************************************************************
 * Multi-threads                                                *
 ****************************************************************/


void TaskQueue::push(const raytraceTask &task)
{
    std::lock_guard<std::mutex> lock(m);
    queue.push(task);
    totalTasks++;
}


raytraceTask TaskQueue::pop()
{
    std::lock_guard<std::mutex> lock(m);
    if (queue.empty())
    return raytraceTask();
    raytraceTask ret = queue.front();
    queue.pop();
    completedTasks++;
    return ret;
}


bool TaskQueue::isEmpty()
{
    std::lock_guard<std::mutex> lock(m);
    return queue.empty();
}


void Worker::work()
{
    while (!this->done)
    {
        if (!globalQueue->isEmpty())
        {
            raytraceTask cur = globalQueue->pop();
            if (cur.result == nullptr)
                continue; //We didn't actually get it, the queue was empty
            if ( this->workerScene->useAcc )
                this->workerScene->traceRayWithAcc(cur.result, cur.origin, 0, NULL);
            else
                this->workerScene->traceRay(cur.result, cur.origin, 0, NULL);
        }
    }
}


void Worker::end()
{
    done = true;
}


/* End of File */