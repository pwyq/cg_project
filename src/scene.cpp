#include "scene.hpp"
#include "triangle.hpp"
#include "hitable.hpp"
#include "light.cpp"

#include <iostream>

Eigen::Vector3f Scene::traceRay(Ray &ray, int level) {
  //This variable will hold the hitable which the ray intersects first.
  Hitable hitObject;

  //This variable will hold the value of t on intersection in the formula r(t) = o + t * d 
  float tOnIntersection = std::numeric_limits<float>::infinity();

  for (auto &h: this->objectsInScene) {
  	if ( h->intersect(tOnIntersection, ray) ) {
  		hitObject = *h;
  	}
  }

  //If tOnIntersection is still infinity, it means this ray did not hit anything, so we return the background color
  if ( tOnIntersection == std::numeric_limits<float>::infinity() ) return Eigen::Vector3f(1.0,1.0,1.0);

  //If we reach this point, it means the ray hitted a object. Now we should compute the color of this object, so we call the shade method.
  std::cout << "Found intersection between triangle and ray" << std::endl;
  
  Eigen::Vector3f color = shade(hitObject, ray, tOnIntersection);
  return color;
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
		this->objectsInScene.push_back(new Triangle(
      mesh.getShapeMatrix() * mesh.getVertex(a).head<3>(),
		  mesh.getShapeMatrix() * mesh.getVertex(b).head<3>(),
		  mesh.getShapeMatrix() * mesh.getVertex(c).head<3>(),
		  f.normal.head<3>(),
      f.material_id));
	}
}

Scene::Scene()
{}


Eigen::Vector3f Scene::shade(Hitable &hitObject, const Ray &ray, float t) {
  //Compute direct light
  Eigen::Vector3f directLight = computeDirectLight(hitObject, ray.origin + t * ray.direction);

  //Compute reflected light
  Eigen::Vector3f reflectedLight = Eigen::Vector3f(0.0,0.0,0.0);

  //Compute refracted light
  Eigen::Vector3f refractedLight = Eigen::Vector3f(0.0,0.0,0.0);

  return directLight + reflectedLight + refractedLight;
}

Eigen::Vector3f Scene::computeDirectLight(Hitable& hitObject, Eigen::Vector3f hitPoint) {
  //Get the material properties of the hitable object this ray interesected with 
  Tucano::Material::Mtl faceMaterial = materials -> at(hitObject.material_id);
  Eigen::Vector3f ka = faceMaterial.getAmbient();
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

      //Check if ray to light intersects with an object from the scene
      bool hitted = false;
      for (Hitable* h: objectsInScene) {
        float maxt = 1.0;
        if ( h -> intersect(maxt, rayToLight) && maxt > 0.0001 ) {
          hitted = true;
          break;
        }
      }
      //If we hitted a object, it means we are in shadow so skip this light source
      if ( hitted ) continue;

      //If we reach this point, it means that the current light can reach the object, so we compute the shading
      
      //Normailized vector from the hitpoint to the light
      Eigen::Vector3f directionNormalized = rayToLight.direction.normalized();
      //Normalized vector from the hitpoint to the eye (camera)
      Eigen::Vector3f eyeDirection = (cameraPosition-hitPoint).normalized();

      //Compute the ambient component
      Eigen::Vector3f ambient = currentLight.spectrum.cwiseProduct(ka);
        
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
      cosinus = pow(cosinus, shininess);
      //Compute the specular component
      Eigen::Vector3f specular = currentLight.spectrum.cwiseProduct(ks) * cosinus;
      
      color += ambient + diffuse + specular;
  }
  return color;
}

void Scene::CreateAccelarate(){
  std::vector<Hitable*> boxesInScene;


}

void Scene::CreateBox(std::vector<Triangle> triangles){
  std::vector<Hitable*> boxesInScene;
  //Set the min max values of the new box to the min and max values
  float xmax = std::numeric_limits<float>::min();
  float xmin = std::numeric_limits<float>::max();
  float ymax = std::numeric_limits<float>::min();
  float ymin = std::numeric_limits<float>::max();
  float zmax = std::numeric_limits<float>::min();
  float zmin = std::numeric_limits<float>::max();
  

  //Loop over the trianlges and update x,y,z accordingly 
  for(Triangle t : triangles){

    if(t.getMaxX() > xmax){
      xmax = t.getMaxX();
    }
    if(t.getMinX() < xmin){
      xmin = t.getMinX();
    }
    if(t.getMaxY() > ymax){
      ymax = t.getMaxY();
    }
    if(t.getMinY() < ymin){
      ymin = t.getMinY();
    }
    if(t.getMaxZ() > xmax){
      zmax = t.getMaxZ();
    }
    if(t.getMinZ() < xmin){
      zmin = t.getMinZ();
    }
  }
  Eigen::Vector3f bMin = Eigen::Vector3f(xmin, ymin, zmin);
  Eigen::Vector3f bMax = Eigen::Vector3f(xmax, ymax, zmax);

  createdBox = Box(bMin, bMax, true);
  //createdBox.triangles = triangles;
  //return Box(bMin, bMax, true);
}


