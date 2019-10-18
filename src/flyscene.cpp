#include "flyscene.hpp"
#include <GLFW/glfw3.h>

void Flyscene::initialize(int width, int height) {
  // initiliaze the Phong Shading effect for the Opengl Previewer
  phong.initialize();

  // set the camera's projection matrix
  flycamera.setPerspectiveMatrix(60.0, width / (float)height, 0.1f, 100.0f);
  flycamera.setViewport(Eigen::Vector2f((float)width, (float)height));

  // load the OBJ file and materials
  Tucano::MeshImporter::loadObjFile(mesh, materials,
                                    "resources/models/dodgeColorTest.obj");


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

  /*
   for (int i = 0; i<mesh.getNumberOfFaces(); ++i){
     Tucano::Face face = mesh.getFace(i);
     std::cout<<"Face id: "<<i<<std::endl;
     std::cout<<"mat id "<<face.material_id<<std::endl;
     std::cout<<"number of verticies of this face: "<<face.vertex_ids.size()<<std::endl;
     std::cout<<"face   normal "<<face.normal.transpose() << std::endl << std::endl;
     for (int j =0; j<face.vertex_ids.size(); ++j){
       std::cout<<"vid "<<j<<" "<<face.vertex_ids[j]<<std::endl;
       std::cout<<"vertex "<<mesh.getVertex(face.vertex_ids[j]).transpose()<<std::endl;
       std::cout<<"normal "<<mesh.getNormal(face.vertex_ids[j]).transpose()<<std::endl;
     }
   }
   */



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
  long long total = image_size[0] * image_size[1];
  long long c = 0;
  for (int j = 0; j < image_size[1]; ++j) {
    for (int i = 0; i < image_size[0]; ++i) {
      // create a ray from the camera passing through the pixel (i,j)
      screen_coords = flycamera.screenToWorld(Eigen::Vector2f(i, j));
      // launch raytracing for the given ray and write result to pixel data
      pixel_data[i][j] = traceRay(origin, screen_coords);
      std::cout << ++c << " of " << total <<" rays traced\n";
    }
  }

  // write the ray tracing result to a PPM image
  Tucano::ImageImporter::writePPMImage("result.ppm", pixel_data);
  std::cout << "ray tracing done! " << std::endl;
}

/**
 * Finds a face from the mesh that is intersected by a ray defined
 * @param origin the origin of the ray
 * @param dir the direction of the ray
 * @return id of the intersected face on the mesh or -1 if 
 * the ray doesn't hit anything
 */
long long Flyscene::intersectRayTriangle(const Eigen::Vector3f &origin,
                                         const Eigen::Vector3f &dir) {
  for (size_t i = 0; i < mesh.getNumberOfFaces(); ++i){
    Tucano::Face face = mesh.getFace(i);

    /* Get the xyz of the 3 vertices from current face */
    Eigen::Vector3f a = mesh.getVertex(face.vertex_ids[0]).head<3>();
    Eigen::Vector3f b = mesh.getVertex(face.vertex_ids[1]).head<3>();
    Eigen::Vector3f c = mesh.getVertex(face.vertex_ids[2]).head<3>();

    /* Find the edges of the triangle */
    Eigen::Vector3f edgeA = b - a;
    Eigen::Vector3f edgeB = c - a;
    Eigen::Vector3f edgeC = a - c;
    /* Find the triangle's normal */
    Eigen::Vector3f norm = edgeA.cross(edgeB);
    norm.normalize();
    /* Find distance from origin to plane */
    double dist = norm.dot(edgeA);
    /* Find the length of ray to the point of imact on the plane */
    double t = (norm.dot(origin) + dist) / norm.dot(dir);
    Eigen::Vector3f ray = origin + t*dir;

    /* Determine if point of impact is within triangle */
    Eigen::Vector3f C0 = ray - a;
    Eigen::Vector3f C1 = ray - b;
    Eigen::Vector3f C2 = ray - c;
    if (norm.dot( C0.cross(edgeA) ) > 0
    &&  norm.dot( C1.cross(edgeB) ) > 0
    &&  norm.dot( C2.cross(edgeC) ) > 0 ) {
	    std::cout<<"Intersecting with "<<i<<std::endl;
            return i;
    }
  }

  return -1;
}

Eigen::Vector3f Flyscene::traceRay(Eigen::Vector3f &origin,
                                   Eigen::Vector3f &dest) {

  Eigen::Vector3f color(0.0f, 0.0f, 0.0f);
  Eigen::Vector3f dir = dest - origin;

  long long faceid = intersectRayTriangle(origin, dir);
  /* If we don't hit anything return default color */
  if (faceid == -1)
    return color;

  Tucano::Face face = mesh.getFace(faceid);

  Tucano::Material::Mtl mat = materials[face.material_id];
  /* TODO Fix this */
  color = mat.getAmbient();

  return color;
}
