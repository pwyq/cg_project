#include <iostream>

#include "scene.hpp"
#include "triangle.hpp"
#include "hitable.hpp"
#include "light.hpp"
#include "box.hpp"

static const int MAXLEVEL = 0;

void Scene::traceRay(Eigen::Vector3f *color, Ray &ray, int level) {
  //This variable will hold the value of t on intersection in the formula r(t) = o + t * d 
  float tOnIntersection = std::numeric_limits<float>::infinity();

  Hitable* hitObject;
  for ( Triangle* t : this->trianglesInScene ) {
    if ( t->intersect(tOnIntersection,ray) != NULL ) {
      hitObject = t;
    }
  }

  //If we reach this point, it means the ray hitted a object. Now we should compute the color of this object, so we call the shade method.
  *color = shade(*hitObject, ray, tOnIntersection, level);
}

// ray tracing with accleration structure
void Scene::traceRayWithAcc(Eigen::Vector3f *color, Ray &ray, int level) {
  //This variable will hold the value of t on intersection in the formula r(t) = o + t * d 
  float tOnIntersection = std::numeric_limits<float>::infinity();

  //This variable will hold the hitable which the ray intersects first (or NULL if no intersection)
  Hitable* hitObject = this->boxOverAllTriangles->intersect(tOnIntersection, ray);
  //If the hitObject is not set to anything, we know we did not hit anything, so use background color and return
  if ( hitObject == NULL ) {
    *color = Eigen::Vector3f(0.0, 0.0, 0.0);
    return;
  }

  //If we reach this point, it means the ray hitted a object. Now we should compute the color of this object, so we call the shade method.
  *color = shade(*hitObject, ray, tOnIntersection, level);
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
}

Scene::Scene()
{}

Eigen::Vector3f Scene::shade(Hitable &hitObject, Ray &ray, float t, int level) {
  //return Eigen::Vector3f(1.0,1.0,1.0);
  
  //Compute direct light
  Eigen::Vector3f directLight = computeDirectLight(hitObject, ray.getPoint(t));

  //Compute reflected light
  //Tucano::Material::Mtl hitMaterial = materials -> at(hitObject.material_id);

  Eigen::Vector3f reflectedLight = Eigen::Vector3f(0.0,0.0,0.0);
  /*if ( hitMaterial.getSpecular()(0) == 1 ) {
    reflectedLight = level >= MAXLEVEL ? Eigen::Vector3f(0.0,0.0,0.0) : computeReflectedLight(hitObject, ray, t, level);
  }*/
  //Compute refracted light
  Eigen::Vector3f refractedLight = Eigen::Vector3f(0.0,0.0,0.0);
  // std::cout << reflectedLight << std::endl;
  // cin.get();
  return directLight + reflectedLight + refractedLight;
}



/****************************************************************
 * Compute Light (direct+reflect+refraction)                    *
 ****************************************************************/

Eigen::Vector3f Scene::computeReflectedLight(Hitable &hitObject, Ray &ray, float t, int level){
    Eigen::Vector3f color = Eigen::Vector3f(0.0,0.0,0.0);
    
    //Calculate the outgoing vector 
    Eigen::Vector3f ReflectedVec = ray.direction.normalized() - 2 *(ray.direction.normalized().dot(hitObject.normal.normalized()))*hitObject.normal.normalized();
    
    //Create ray
    Ray reflectedRay = Ray(ray.getPoint(t), ReflectedVec);
    //We set the origin a bit further away, so it does not hit itself!
    //reflectedRay.origin = reflectedRay.getPoint(0.01);
    //Call correct trace function with the level increased by one.
    if(useAcc){
      Scene::traceRayWithAcc(&color, reflectedRay, level+1);
    }else{
      Scene::traceRay(&color, reflectedRay, level+1);
    }


    return color;
}




Eigen::Vector3f Scene::computeDirectLight(Hitable& hitObject, Eigen::Vector3f hitPoint) {
  //Get the material properties of the hitable object this ray interesected with 
  Tucano::Material::Mtl faceMaterial = materials -> at(hitObject.material_id);
  Eigen::Vector3f kd = faceMaterial.getDiffuse();
  Eigen::Vector3f ks = faceMaterial.getSpecular();
  float shininess = faceMaterial.getShininess();

  Eigen::Vector3f faceNormal = hitObject.normal.normalized();

  Eigen::Vector3f color = Eigen::Vector3f(0.0, 0.0, 0.0);
  //Loop over all the lights in the scene
  for ( int i = 0; i < lights.size(); i++ ) {
      //Get the current light
      Light currentLight = lights.at(i);

      //Build ray to the light
      Ray rayToLight = Ray(hitPoint, currentLight.position - hitPoint);
      rayToLight.origin = rayToLight.getPoint(0.0001);

      //Check if there is an object between the hitpoint and the light, if so skip this light source
      float maxt = 1.0;
      if ( this->boxOverAllTriangles->isIntersecting(maxt, rayToLight) ) continue;

      //If we reach this point, it means that the current light can reach the object, so we compute the shading
      
      //Normailized vector from the hitpoint to the light
      Eigen::Vector3f directionNormalized = rayToLight.direction.normalized();
      //Normalized vector from the hitpoint to the eye (camera)
      Eigen::Vector3f eyeDirection = (cameraPosition-hitPoint).normalized();

        
      //Compute the cosinus of the angle between the face normal and the ray to the light
      float cosinus = faceNormal.dot(directionNormalized);
      //If this cosinus is smaller than 0, we set it to zero.
      cosinus = cosinus > 0 ? cosinus : 0;
      //Compute the diffuse component
      Eigen::Vector3f diffuse = currentLight.spectrum.cwiseProduct(kd) * cosinus;
      
      //Compute the normalized vector of the reflected light ray (reflected in the normal)
      Eigen::Vector3f reflectedLight = (2 * faceNormal.dot(directionNormalized) * faceNormal - directionNormalized).normalized();
      //Compute the cosinus of the angle between the reflected light and the eye direction
      cosinus = reflectedLight.dot( eyeDirection );
      //If this cosinus is smaller than 0, we set it to zero.
      cosinus = cosinus > 0 ? cosinus : 0;
      //Raise the cosinus to the power shininess
      cosinus = std::pow(cosinus, shininess);
      //Compute the specular component
      Eigen::Vector3f specular = currentLight.spectrum.cwiseProduct(ks) * cosinus;
      
      color += diffuse + specular;
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
                this->workerScene->traceRayWithAcc(cur.result, cur.origin, 0);
            else
                this->workerScene->traceRay(cur.result, cur.origin, 0);
        }
    }
}

void Worker::end()
{
    done = true;
}


/* End of File */