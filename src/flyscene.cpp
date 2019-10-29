#include <limits>
#include <cmath>

#include "flyscene.hpp"
#include "multithreading.hpp"

void Flyscene::initialize(int width, int height) {
  // initiliaze the Phong Shading effect for the Opengl Previewer
  phong.initialize();

  // set the camera's projection matrix
  flycamera.setPerspectiveMatrix(60.0, width / (float)height, 0.1f, 100.0f);
  flycamera.setViewport(Eigen::Vector2f((float)width, (float)height));

  // load the OBJ file and materials
  Tucano::MeshImporter::loadObjFile(mesh, materials,"resources/models/boxmirror2.obj");
  // Tucano::MeshImporter::loadObjFile(mesh, materials,"resources/models/cube.obj");
  // Tucano::MeshImporter::loadObjFile(mesh, materials,"resources/models/dodgeColorTest.obj");

  // normalize the model (scale to unit cube and center at origin)
  mesh.normalizeModelMatrix();

  // pass all the materials to the Phong Shader
  for (int i = 0; i < materials.size(); ++i)
    phong.addMaterial(materials[i]);

  // scale the camera representation (frustum) for the ray debug
  camerarep.shapeMatrix()->scale(0.2);

  glEnable(GL_DEPTH_TEST);

  /* Initialize our variables */
  scene = new Scene(mesh, materials);
  //Initialize the lights
  initializeLights();

  if (scene->useAcc)
  {
    getAllLeafBoxesInScene();
    show_acceleration = true;
  }

  printObjectInfo();

  // create a first debug ray pointing at the center of the screen
  createDebugRay(Eigen::Vector2f(width / 2.0, height / 2.0));
}


void Flyscene::printObjectInfo() {
  // moved from tucano/objimporter
  std::cout << "\t ********************* Object Info *********************** " << std::endl;
  std::cout << "\t\tnumber of vertices    : " << mesh.getNumberOfVertices() << std::endl;
  std::cout << "\t\tnumber of faces       : " << mesh.getNumberOfElements() << std::endl;
  std::cout << "\t\tnumber of materials   : " << mesh.getNumberOfMaterials() << std::endl;
  std::cout << "\t\tnumber of leaf boxes  : " << this->leafBoxesInScene.size() << std::endl;
  std::cout << "\t ********************************************************* " << std::endl;
}


void Flyscene::paintGL(void) {
  // update the camera view matrix with the last mouse interactions
  flycamera.updateViewMatrix();
  Eigen::Vector4f viewport = flycamera.getViewport();

  // clear the screen and set background color
  glClearColor(0.9, 0.9, 0.9, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //Render the lights in the scene 
  renderLights();

  // render the scene using OpenGL and one light source
  phong.render(mesh, flycamera, scene_light);

  if ( show_acceleration ) {
    for ( auto boxInScene : leafBoxesInScene ) {
      boxInScene.render(flycamera, scene_light);
    }
  }
    
  for ( auto& ray : debugRays )
    ray.render(flycamera, scene_light);
  camerarep.render(flycamera, scene_light);

  // render coordinate system at lower right corner
  flycamera.renderAtCorner();
}


void Flyscene::simulate(GLFWwindow *window) {
  // Update the camera.
  // NOTE(mickvangelderen): GLFW 3.2 has a problem on ubuntu where some key
  // events are repeated: https://github.com/glfw/glfw/issues/747. Sucks.
  float dx = (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS ? 1.0 : 0.0) -
             (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS ? 1.0 : 0.0);
  float dy = (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS ? 1.0 : 0.0) -
             (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS ? 1.0 : 0.0);
  float dz = (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ? 1.0 : 0.0) - (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS ? 1.0 : 0.0);
  flycamera.translate(dx, dy, dz);
}


void Flyscene::createDebugRay(const Eigen::Vector2f &mouse_pos) {
  // Flush all previus debug rays
  debugRays.clear();
  // from pixel position to world coordinates
  Eigen::Vector3f screen_pos = flycamera.screenToWorld(mouse_pos);

  // direction from camera center to click position
  Eigen::Vector3f dir = (screen_pos - flycamera.getCenter()).normalized();

  Ray r(flycamera.getCenter(), screen_pos - flycamera.getCenter());

  // Holds the debug ray traced in the scene
  std::vector<Ray> rayList;
  // Holds the length of the debug ray
  std::vector<float> tList;

  Eigen::Vector3f buffer;
  scene->traceRay(&buffer, r, 0, NULL, &rayList, &tList);
  for (size_t i = 0; i < rayList.size(); ++i)
  {
    Tucano::Shapes::Cylinder newDebugRay(DEBUG_RAY_RADIUS, tList[i], 16, 64);
    newDebugRay.resetModelMatrix();
    // position and orient the cylinder representing the ray
    newDebugRay.setOriginOrientation(rayList[i].origin, rayList[i].direction.normalized());
    debugRays.push_back(newDebugRay);
  }

  // place the camera representation (frustum) on current camera location, 
  camerarep.resetModelMatrix();
  camerarep.setModelMatrix(flycamera.getViewMatrix().inverse());
}


/****************************************************************
 * Light                                                        *
 ****************************************************************/
void Flyscene::initializeLights() {
  // set the color and size of the sphere to represent the light sources
  // same sphere is used for all sources
  lightrep.setColor(Eigen::Vector4f(1.0, 1.0, 0.0, 1.0));
  lightrep.setSize(0.1);

  // create a first ray-tracing light source at some random position (point light)
  // scene->lights.push_back(new Light(Eigen::Vector3f(1.0,1.0,1.0),Eigen::Vector3f(2.0, 1.5, 1.0)));
  // std::cout << "Created first light" << std::endl;
}

void Flyscene::renderLights() {
  for ( int i = 0; i < scene->lights.size(); i++ ) {
    Light* currentLight = scene->lights.at(i);
    scene_light.resetViewMatrix();
    scene_light.viewMatrix()->translate(-currentLight->position);

    //Two cases for drawing a light, either it is a normal point light, or it is a sphereLight
    SphereLight* sphereLight = dynamic_cast<SphereLight*>(currentLight);
    if ( sphereLight ) {
      for ( int j = 0; j < sphereLight->sphericalLightsPos.size(); j++ ) {
        lightrep.resetModelMatrix();
        lightrep.modelMatrix()->translate(sphereLight->sphericalLightsPos.at(j));
        lightrep.render(flycamera, scene_light);
      }
    } else {
        lightrep.resetModelMatrix();
        lightrep.modelMatrix()->translate(currentLight->position);
        lightrep.render(flycamera, scene_light);
    }

  }
}

void Flyscene::addPointLight() {
  Eigen::Vector3f position = flycamera.getCenter();
  scene->lights.push_back(new Light(Eigen::Vector3f(1.0,1.0,1.0), position));
}

void Flyscene::addSphericalLight() {
  Eigen::Vector3f position = flycamera.getCenter();
  scene->lights.push_back(new SphereLight(Eigen::Vector3f(1.0,1.0,1.0), position)); 
}


/****************************************************************
 * Ray Tracing                                                  *
 ****************************************************************/


void Flyscene::raytraceScene(int width, int height) {
  std::cout << "[INFO] Start ray tracing ..." << std::endl;

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

  //Set the camera in the scene
  scene -> cameraPosition = flycamera.getCenter();
  
  Multithreading multithreading(scene);
 
  // origin of the ray is always the camera center
  Eigen::Vector3f origin = flycamera.getCenter();
  Eigen::Vector3f screen_coords;
  // for every pixel shoot a ray from the origin through the pixel coords
  for (int j = 0; j < image_size[1]; ++j) {
    for (int i = 0; i < image_size[0]; ++i) {
      // create a ray from the camera passing through the pixel (i,j)
      screen_coords = flycamera.screenToWorld(Eigen::Vector2f(i, j));
      // launch raytracing for the given ray and write result to pixel data
      Ray r(origin, screen_coords - origin);
      if ( scene -> useThreads)
        multithreading.globalQueue.push(raytraceTask(&pixel_data[i][j], r));
      else
        scene->traceRay(&pixel_data[i][j], r, 0, NULL, nullptr, nullptr);
    }
  }

  // Wait for threads to finish
  auto t1 = std::chrono::high_resolution_clock::now();
  while (!multithreading.globalQueue.isEmpty())
  {
    std::cout<<multithreading.globalQueue.completedTasks<<" / "<<multithreading.globalQueue.totalTasks<<" rays traced\n";
    std::this_thread::sleep_for(1ms);
  }
  auto t2 = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
  
  // write the ray tracing result to a PPM image
  Tucano::ImageImporter::writePPMImage("result.ppm", pixel_data);
  std::cout << "[SUCCESS] ray tracing done with time = " << duration << " microseconds." << std::endl;
}


void Flyscene::getAllLeafBoxesInScene() {
  this->leafBoxesInScene.clear();
  Box* firstBox = scene -> boxOverAllTriangles;
  std::vector<Box*> boxes = firstBox -> getLeafBoxes();
  for ( Box* box : boxes ) {
    this->leafBoxesInScene.push_back(convertToTucanoBox(box));
  }
  //this->leafBoxesInScene.at(1).setColor(Eigen::Vector4f(0.0, 0.0, 1.0, 0.5));
}


Tucano::Shapes::Box Flyscene::convertToTucanoBox( Box *box ) {
  float width  = std::abs(box->bMin(0) - box->bMax(0));
  float height = std::abs(box->bMin(1) - box->bMax(1));
  float depth  = std::abs(box->bMin(2) - box->bMax(2));
  Eigen::Vector3f boxCenter = Eigen::Vector3f( (box->bMin(0)+box->bMax(0))/2, (box->bMin(1)+box->bMax(1))/2, (box->bMin(2)+box->bMax(2))/2 );
  Tucano::Shapes::Box tucanoBox = Tucano::Shapes::Box(width,height,depth);
  tucanoBox.resetModelMatrix();
  tucanoBox.modelMatrix()->translate(boxCenter);
  tucanoBox.setColor(Eigen::Vector4f(1.0, 1.0, 0.0, 0.5));
  return tucanoBox;
}

/* End of file */
