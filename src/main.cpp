#include <glfw_initialization.h>
#include <glfw_window.h>
#include <glfw_monitor.h>
#include <graphics.h>

std::int32_t main(std::int32_t argc, gsl::zstring* argv)
{
  std::shared_ptr<spdlog::logger> logger = spdlog::stdout_color_mt("veng");
  logger->set_pattern("[%H:%M:%S] %^[%l]%$ [%s:%#] %v");
  spdlog::set_default_logger(logger);
  spdlog::set_level(spdlog::level::trace);  // Show all log levels

  SPDLOG_INFO("Current working directory: {}", std::filesystem::current_path().string());

  const veng::GlfwInitialization _glfw;
  veng::Window window("Vulkan Renderer", {800, 600});
  window.TryMoveToMonitor(1);

  veng::Graphics graphics(&window);
  while (!window.ShouldClose())
  {
    glfwPollEvents();
    graphics.BeginFrame();
    graphics.RenderTriangle();
    graphics.EndFrame();
  }

  return EXIT_SUCCESS;
}
