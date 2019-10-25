// Must come before GLFW
#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <iostream>
#include "flyscene.hpp"

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 1000

Flyscene *flyscene;
Eigen::Vector2f mouse_pos = Eigen::Vector2f::Zero();

#ifdef TUCANODEBUG
void GLAPIENTRY
MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	if (severity == GL_DEBUG_SEVERITY_HIGH || severity == GL_DEBUG_SEVERITY_MEDIUM) {
		fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
			(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
			type, severity, message);
	}
}
#endif

void initialize(void) {
  Tucano::Misc::initGlew();

#ifdef TUCANODEBUG
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(MessageCallback, 0);
#endif

  flyscene = new Flyscene();
  flyscene->initialize(WINDOW_WIDTH, WINDOW_HEIGHT);  

  // Available keys: b f g j k n o p u v x y
  std::cout << "\n\n"
            << "\t ************************ Usage ************************** " << std::endl;
  std::cout << "\t\tR    : Reset camera" << std::endl;
  std::cout << "\t\tWASD : Move camera in xz plane" << std::endl;
  std::cout << "\t\tQEZC : Move camera along y axis" << std::endl;
  std::cout << "\t\tSPACE: Shoot debug ray from mouse cursor position" << std::endl;
  std::cout << "\t\tL    : Add new point light source at current\n"
            << "\t\t       canera position" << std::endl;
  std::cout << "\t\tT    : Ray trace the scene" << std::endl;
  std::cout << "\t\tEsc  : Close application" << std::endl;
  std::cout << "\t ====================== Our Keys ========================= " << std::endl;
  std::cout << "\t\tH    : Toggle acceleration structure" << std::endl;
  std::cout << "\t\tI    : Output object informaiton to terminal" << std::endl;
  std::cout << "\t\tM    : Toggle sphericial light (under test)" << std::endl;
  std::cout << "\t ********************************************************* " << std::endl;
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action,
                 int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, 1);
  else if (key == GLFW_KEY_R && action == GLFW_PRESS)
    flyscene->getCamera()->reset();
  else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    flyscene->createDebugRay(mouse_pos);
  else if (key == GLFW_KEY_L && action == GLFW_PRESS)
    flyscene->addLight();
  else if (key == GLFW_KEY_T && action == GLFW_PRESS)
    flyscene->raytraceScene();
  else if (key == GLFW_KEY_H && action == GLFW_PRESS)
    flyscene->show_acceleration=!flyscene->show_acceleration;
  else if (key == GLFW_KEY_I && action == GLFW_PRESS)
    flyscene->printObjectInfo();
  else if (key == GLFW_KEY_M && action == GLFW_PRESS)
    flyscene->setSphericalLight();
}

static void mouseButtonCallback(GLFWwindow *window, int button, int action,
                                int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    flyscene->getCamera()->startRotation(Eigen::Vector2f(xpos, ypos));
  }
}

static void cursorPosCallback(GLFWwindow *window, double xpos, double ypos) {
  mouse_pos = Eigen::Vector2f(xpos, ypos);

  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
    flyscene->getCamera()->rotate(Eigen::Vector2f(xpos, ypos));
  }
}

int main(int argc, char *argv[]) {
  GLFWwindow *main_window;

  if (!glfwInit()) {
    std::cerr << "Failed to init glfw" << std::endl;
    return 1;
  }

  main_window =
      glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Ray Tracer", NULL, NULL);
  if (!main_window) {
    std::cerr << "Failed to create the GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(main_window);

  glfwSetKeyCallback(main_window, keyCallback);
  glfwSetMouseButtonCallback(main_window, mouseButtonCallback);
  glfwSetCursorPosCallback(main_window, cursorPosCallback);
  glfwSetInputMode(main_window, GLFW_STICKY_KEYS, true);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  initialize();

  while (!glfwWindowShouldClose(main_window)) {
    glfwMakeContextCurrent(main_window);
    flyscene->paintGL();
    glfwSwapBuffers(main_window);

    glfwPollEvents();
    flyscene->simulate(main_window);
  }

  glfwDestroyWindow(main_window);
  glfwTerminate();
  return 0;
}
