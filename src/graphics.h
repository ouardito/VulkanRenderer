#pragma once

#include <vulkan/vulkan.h>
#include <glfw_window.h>
#include <vector>
#include <array>

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
    std::vector<gsl::czstring> GetRequiredInstanceExtension();
    static std::vector<VkExtensionProperties> GetSupportedInstanceExtensions();
    static bool AreAllExtensionsSupported(gsl::span<gsl::czstring> extension);

    static std::vector<VkLayerProperties> GetSupportedValidationLayers();
    static bool AreAllLayersSupported(gsl::span<gsl::czstring> layers);

    
    

    VkInstance instance_ = nullptr;
    gsl::not_null<Window*> window_;
    bool validation_enabled_ = false;
  };

}  // namespace veng