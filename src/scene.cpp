#include "scene.hpp"

Eigen::Vector3f Scene::traceRay(Ray ray, int level = 0) {
  //This variable will hold the hitable which the ray intersects first.
  Hitable hitObject;

  //This variable will hold the value of t on intersection in the formula r(t) = o + t * d 
  float tOnIntersection = std::numeric_limits<float>::infinity();

  for (auto &h: this.objectsInScene) {
  	if ( h.intersect(tOnIntersection, ray) ) {
  		hitObject = h;
  	}
  }

  //If tOnIntersection is still infinity, it means this ray did not hit anything, so we return the background color
  if ( tOnIntersection == std::numeric_limits<float>::infinity() ) return Eigen::Vector3f(1.0,1.0,1.0);

  //If we reach this point, it means the ray hitted a object. Now we should compute the color of this object, so we call the shade method.
  std::cout << "Found intersection between triangle and ray" << std::endl;
  //TODO
  //Eigen::Vector3f color;
  //shade(1, hitface, origin+tOnIntersection*rayDirection, color, origin, dest);
  //return color;
  return Eigen::Vector3f(0,0,0);
}

//TODO For now this only copies triangles
Scene::Scene(const Tucano::Mesh &mesh, const Tucano::Material::Mtl &materials)
{
	this->materials = materials;
	this->objectsInScene.resize(mesh.getNumberOfFaces());
	for (size_t i = 0; i < mesh.getNumberOfFaces(); ++i)
		this->objectsInScene[i] = new Triangle(mesh.getFace(i));
}

Eigen::Vector3f Scene::shade(Hitable hitObject, Ray ray, float t) {
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
  Tucano::Material::Mtl faceMaterial = materials[hitObject.material_id];
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
      Ray rayToLight = Ray(hitPoint, currentLight.position - origin);

      //Check if ray to light intersects with an object from the scene
      bool hitted = false;
      for (auto &h: this.objectsInScene) {
        if ( h.intersect(1.0, rayToLight) && h != hitObject ) {
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
      Eigen::Vector3f eyeDirection = (this.cameraPosition-origin).normalized();

      //Compute the ambient component
      Eigen::Vector3f ambient = light.spectrum.cwiseProduct(ka);
        
      //Compute the cosinus of the angle between the face normal and the ray to the light
      float cosinus = faceNormal.dot(directionNormalized);
      //If this cosinus is smaller than 0, we set it to zero.
      cosinus = cosinus > 0 ? cosinus : 0;
      //Compute the diffuse component
      Eigen::Vector3f diffuse = light.spectrum.cwiseProduct(kd) * cosinus;
      
      //Compute the normalized vector of the reflected light ray (reflected in the normal)
      Eigen::Vector3f reflectedLight = (2 * faceNormal.dot(directionNormalized) * faceNormal - directionNormalized).normalized();
      //Compute the cosinus of the angle between the reflected light and the eye direction
      cosinus = reflectedLight.dot( eyeDirection );
      //If this cosinus is smaller than 0, we set it to zero.
      cosinus = cosinus > 0 ? cosinus : 0;
      //Raise the cosinus to the power shininess
      cosinus = pow(cosinus, shininess);
      //Compute the specular component
      Eigen::Vector3f specular = light.spectrum.cwiseProduct(ks) * cosinus;
      
      color += ambient + diffuse + specular;
  }
  return color;
}

