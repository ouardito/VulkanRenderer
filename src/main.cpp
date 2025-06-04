#include <glfw_initialization.h>
#include <glfw_window.h>
#include <glfw_monitor.h>
#include <graphics.h>

std::int32_t main(std::int32_t argc, gsl::zstring* argv)
{
  const veng::GlfwInitialization _glfw;
  veng::Window window("Vulkan Renderer", {800,600});
  window.TryMoveToMonitor(1);
  veng::Graphics graphics(&window);
  while (!window.ShouldClose())
  {
    glfwPollEvents();
  }

  return EXIT_SUCCESS;
}
