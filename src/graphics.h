#pragma once

#include <vulkan/vulkan.h>
#include <glfw_window.h>
#include <vector>

namespace veng
{
  class Graphics final
  {
   public:
    Graphics(gsl::not_null<Window*> window);
    ~Graphics();

   private:
    void IntializeVulkan();
    void CreateInstance();
    static gsl::span<gsl::czstring> GetSuggestedInstanceExtension();
    static std::vector<VkExtensionProperties> GetSupportedExtensions();
    static bool AreAllExtensionsSupported(gsl::span<gsl::czstring> extension);

    VkInstance instance_ = nullptr;
    gsl::not_null<Window*> window_;
  };

}  // namespace veng