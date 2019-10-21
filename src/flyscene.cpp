#include "flyscene.hpp"
#include <GLFW/glfw3.h>
#include <limits>

#define DEPTH = 2;

void Flyscene::initialize(int width, int height) {
  // initiliaze the Phong Shading effect for the Opengl Previewer
  phong.initialize();

  // set the camera's projection matrix
  flycamera.setPerspectiveMatrix(60.0, width / (float)height, 0.1f, 100.0f);
  flycamera.setViewport(Eigen::Vector2f((float)width, (float)height));

  // load the OBJ file and materials
  Tucano::MeshImporter::loadObjFile(mesh, materials,
                                    "resources/models/cube.obj");

  // normalize the model (scale to unit cube and center at origin)
  mesh.normalizeModelMatrix();

  // pass all the materials to the Phong Shader
  for (int i = 0; i < materials.size(); ++i)
    phong.addMaterial(materials[i]);



  // set the color and size of the sphere to represent the light sources
  // same sphere is used for all sources
  lightrep.setColor(Eigen::Vector4f(1.0, 1.0, 0.0, 1.0));
  lightrep.setSize(0.15);

  // create a first ray-tracing light source at some random position
  lights.push_back(Eigen::Vector3f(-1.0, 1.0, 1.0));

  // scale the camera representation (frustum) for the ray debug
  camerarep.shapeMatrix()->scale(0.2);

  // the debug ray is a cylinder, set the radius and length of the cylinder
  ray.setSize(0.005, 10.0);

  // craete a first debug ray pointing at the center of the screen
  createDebugRay(Eigen::Vector2f(width / 2.0, height / 2.0));

  glEnable(GL_DEPTH_TEST);

  // for (int i = 0; i<mesh.getNumberOfFaces(); ++i){
  //   Tucano::Face face = mesh.getFace(i);    
  //   for (int j =0; j<face.vertex_ids.size(); ++j){
  //     std::cout<<"vid "<<j<<" "<<face.vertex_ids[j]<<std::endl;
  //     std::cout<<"vertex "<<mesh.getVertex(face.vertex_ids[j]).transpose()<<std::endl;
  //     std::cout<<"normal "<<mesh.getNormal(face.vertex_ids[j]).transpose()<<std::endl;
  //   }
  //   std::cout<<"mat id "<<face.material_id<<std::endl<<std::endl;
  //   std::cout<<"face   normal "<<face.normal.transpose() << std::endl << std::endl;
  // }

}

void Flyscene::paintGL(void) {

  // update the camera view matrix with the last mouse interactions
  flycamera.updateViewMatrix();
  Eigen::Vector4f viewport = flycamera.getViewport();

  // clear the screen and set background color
  glClearColor(0.9, 0.9, 0.9, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // position the scene light at the last ray-tracing light source
  scene_light.resetViewMatrix();
  scene_light.viewMatrix()->translate(-lights.back());

  // render the scene using OpenGL and one light source
  phong.render(mesh, flycamera, scene_light);

  // render the ray and camera representation for ray debug
  ray.render(flycamera, scene_light);
  camerarep.render(flycamera, scene_light);

  // render ray tracing light sources as yellow spheres
  for (int i = 0; i < lights.size(); ++i) {
    lightrep.resetModelMatrix();
    lightrep.modelMatrix()->translate(lights[i]);
    lightrep.render(flycamera, scene_light);
  }

  // render coordinate system at lower right corner
  flycamera.renderAtCorner();
}

void Flyscene::simulate(GLFWwindow *window) {
  // Update the camera.
  // NOTE(mickvangelderen): GLFW 3.2 has a problem on ubuntu where some key
  // events are repeated: https://github.com/glfw/glfw/issues/747. Sucks.
  float dx = (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS ? 1.0 : 0.0) -
             (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS ? 1.0 : 0.0);
  float dy = (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS ||
                      glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS
                  ? 1.0
                  : 0.0) -
             (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS ||
                      glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS
                  ? 1.0
                  : 0.0);
  float dz = (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ? 1.0 : 0.0) -
             (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS ? 1.0 : 0.0);
  flycamera.translate(dx, dy, dz);
}

void Flyscene::createDebugRay(const Eigen::Vector2f &mouse_pos) {
  ray.resetModelMatrix();
  // from pixel position to world coordinates
  Eigen::Vector3f screen_pos = flycamera.screenToWorld(mouse_pos);

  // direction from camera center to click position
  Eigen::Vector3f dir = (screen_pos - flycamera.getCenter()).normalized();
  
  // position and orient the cylinder representing the ray
  ray.setOriginOrientation(flycamera.getCenter(), dir);

  // place the camera representation (frustum) on current camera location, 
  camerarep.resetModelMatrix();
  camerarep.setModelMatrix(flycamera.getViewMatrix().inverse());
}

void Flyscene::raytraceScene(int width, int height) {
  std::cout << "ray tracing ..." << std::endl;

  // if no width or height passed, use dimensions of current viewport
  Eigen::Vector2i image_size(width, height);
  if (width == 0 || height == 0) {
    image_size = flycamera.getViewportSize();
  }

  // create 2d vector to hold pixel colors and resize to match image size
  vector<vector<Eigen::Vector3f>> pixel_data;
  pixel_data.resize(image_size[1]);
  for (int i = 0; i < image_size[1]; ++i)
    pixel_data[i].resize(image_size[0]);

  // origin of the ray is always the camera center
  Eigen::Vector3f origin = flycamera.getCenter();
  Eigen::Vector3f screen_coords;

  
  // for every pixel shoot a ray from the origin through the pixel coords
  for (int j = 0; j < image_size[1]; ++j) {
    for (int i = 0; i < image_size[0]; ++i) {
      std::cout << "Shooting ray from: (" << i << "," << j << ")" << std::endl;
      // create a ray from the camera passing through the pixel (i,j)
      screen_coords = flycamera.screenToWorld(Eigen::Vector2f(i, j));
      // launch raytracing for the given ray and write result to pixel data
      pixel_data[i][j] = scene.traceRay(Ray(origin, screen_coords - origin));
    }
  }

  // write the ray tracing result to a PPM image
  Tucano::ImageImporter::writePPMImage("result.ppm", pixel_data);
  std::cout << "ray tracing done! " << std::endl;
}




















/*
Eigen::Vector3f Flyscene::traceRay(int level, Ray ray) {
  //This variable will hold the face which the ray intersects first.
  Hitable hitObject;

  //This variable will hold the value of t on intersection in the formula r(t) = o + t * d 
  float tOnIntersection = std::numeric_limits<float>::infinity();

  //Call intersect. After this call "hitface" and "tOnIntersection" will hold the right values
  intersect( hitface, tOnIntersection, origin, rayDirection );

  //If tOnIntersection is still infinity, it means this ray did not hit anything, so we return the background color
  if ( tOnIntersection == std::numeric_limits<float>::infinity() ) return Eigen::Vector3f(1.0,1.0,1.0);

  //If we reach this point, it means the ray hitted a face. Now we should compute the color of this face, so we call the shade method.
  std::cout << "Found intersection between triangle and ray" << std::endl;
  Eigen::Vector3f color;
  shade(1, hitface, origin+tOnIntersection*rayDirection, color, origin, dest);
  return color;
}

void Flyscene::intersect(Tucano::Face &face, float &max, Eigen::Vector3f &origin, Eigen::Vector3f &direction ) {
  //Loop over all the faces of the mesh
  for ( int i = 0; i < mesh.getNumberOfFaces(); i++ ) {
      //Get the face at position i, this is the face we are going to test for intersection
      Tucano::Face currentFace = mesh.getFace(i);

      //Get all the vertices of this face
      Eigen::Vector4f vertex0Projective = mesh.getVertex(currentFace.vertex_ids[0]);
      Eigen::Vector4f vertex1Projective = mesh.getVertex(currentFace.vertex_ids[1]);
      Eigen::Vector4f vertex2Projective = mesh.getVertex(currentFace.vertex_ids[2]);
      Eigen::Vector3f vertex0 = mesh.getShapeMatrix() * Eigen::Vector3f(vertex0Projective(0),vertex0Projective(1),vertex0Projective(2));
      Eigen::Vector3f vertex1 = mesh.getShapeMatrix() * Eigen::Vector3f(vertex1Projective(0),vertex1Projective(1),vertex1Projective(2));
      Eigen::Vector3f vertex2 = mesh.getShapeMatrix() * Eigen::Vector3f(vertex2Projective(0),vertex2Projective(1),vertex2Projective(2));

      //Find triangle's plane, plane equation: p * faceNormal - D = 0
      Eigen::Vector3f faceNormal = currentFace.normal;                   
      float D = faceNormal.dot(vertex0);

      //Calculate intersection point of ray with triangle's plane
      float t = (D - origin.dot(faceNormal)) / direction.dot(faceNormal);
      
      //If t is negative this means the triange's plane is behind the ray
      if ( t < 0 ) continue;
      //If t is greater then max, it means that this face is behind some face we already intersected with, so skip this face.
      if ( t > max ) continue;
      //Calculate the actual intersection point of the ray with the triangle
      Eigen::Vector3f intersectionPoint = origin + t * direction;

      //Check if intersection is inside the triangle. We do this by calculating the barycentric coordinates
      float areaABC = faceNormal.dot( (vertex1 - vertex0).cross(vertex2 - vertex0) );
      float areaPBC = faceNormal.dot( (vertex1 - intersectionPoint).cross(vertex2 - intersectionPoint) );
      float areaPCA = faceNormal.dot( (vertex2 - intersectionPoint).cross(vertex0 - intersectionPoint) );

      float a = areaPBC / areaABC;
      float b = areaPCA / areaABC;
      
      //If the intersection point is outside the triangle we can skip this face 
      if ( a < 0 || a > 1 ) continue;
      if ( b < 0 || b > 1 ) continue;
      if ( a + b > 1 ) continue;
      
      //When we reach this point, it means that the current face intersects with the ray and the intersection
      //point is closer to the intersection point we already found. Therefore we update the values "face" and "max".
      face = currentFace;
      max = t;
  }
}

void Flyscene::shade(int level, Tucano::Face face, Eigen::Vector3f hitpoint, Eigen::Vector3f &color, Eigen::Vector3f &origin, Eigen::Vector3f &dest) {

  Eigen::Vector3f directLightColor = computeDirectLight(face, hitpoint, origin);



  color = directLightColor;
  //color = Eigen::Vector3f(0.4,0.1,0.0);
}


Eigen::Vector3f Flyscene::computeDirectLight(Tucano::Face &face, Eigen::Vector3f hitpoint, Eigen::Vector3f &eye) {
  //Get the material properties of the face this ray interesected with 
  Tucano::Material::Mtl faceMaterial = materials[face.material_id];
  Eigen::Vector3f ka = faceMaterial.getAmbient();
  Eigen::Vector3f kd = faceMaterial.getDiffuse();
  Eigen::Vector3f ks = faceMaterial.getSpecular();
  float shininess = faceMaterial.getShininess();
  Eigen::Vector3f faceNormal = face.normal.normalized();

  //We assume that all lights have this spectrum 
  Eigen::Vector3f lightSpectrum = Eigen::Vector3f(1.0, 1.0, 1.0);

  Eigen::Vector3f color = Eigen::Vector3f(0.0, 0.0, 0.0);
  
  //Loop over all the lights in the scene
  for ( int i = 0; i < lights.size(); i++ ) {
      //Get the current light
      Eigen::Vector3f currentLight = lights.at(i);

      //Build ray to the light
      Eigen::Vector3f origin = hitpoint;
      Eigen::Vector3f direction = currentLight-origin;

      //Check if ray to light intersects with an object from the scene
      Tucano::Face intersectionFace;
      float maxt = 1.0;
      intersect(intersectionFace, maxt, origin, direction);
      //If the ray intersected with a triangle between the light source and the face, maxt will be < 1
      //This means the face does not get lighted by this light source, so we skip the contrubution of this light
      if ( maxt < 1.0 && maxt > 0.00001 ) continue;

      //If we reach this point, it means that the current light can reach the face, so we compute the shading
      
      //Normailized vector from the hitpoint to the light
      Eigen::Vector3f directionNormalized = direction.normalized();
      //Normalized vector from the hitpoint to the eye (camera)
      Eigen::Vector3f eyeDirection = (eye-origin).normalized();

      //Compute the ambient component
      Eigen::Vector3f ambient = lightSpectrum.cwiseProduct(ka);
        
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
      cosinus = pow(cosinus, shininess);
      //Compute the specular component
      Eigen::Vector3f specular = lightSpectrum.cwiseProduct(ks) * cosinus;
      
      color += ambient + diffuse + specular;
  }
  return color;
}*/