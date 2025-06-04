#pragma once

namespace veng
{
  class Window
  {
   public:
    Window(gsl::czstring name, glm::ivec2 size);
    ~Window();

    glm::ivec2 GetWindowSize() const;
    glm::ivec2 getFrameBufferSize() const;
    bool ShouldClose() const;
    GLFWwindow* GetHandle() const;
    bool TryMoveToMonitor(std::uint16_t monitor_number) const;

   private:
    GLFWwindow* window_;
  };
}  // namespace veng
