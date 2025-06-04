#include <glfw_initialization.h>
#include <spdlog/spdlog.h>
namespace veng
{
  void glfw_error_callback_function(std::int32_t err_code, gsl::czstring err_msg)
  {
    SPDLOG_ERROR("[GLFW CALLBACK] | ERROR:{}, {}", err_code, err_msg);
  }

  GlfwInitialization::GlfwInitialization()
  {
    glfwSetErrorCallback(glfw_error_callback_function);

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
