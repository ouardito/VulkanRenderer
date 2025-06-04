#include <glfw_initialization.h>

namespace veng
{
  GlfwInitialization::GlfwInitialization()
  {
    if (glfwInit() != GLFW_TRUE)
    {
      std::exit(EXIT_FAILURE);
    }
  }

  GlfwInitialization::~GlfwInitialization()
  {
    glfwTerminate();
  }
}  // namespace veng
