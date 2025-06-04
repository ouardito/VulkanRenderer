#pragma once

#include <vulkan/vulkan.h>
#include <glfw_window.h>

namespace veng
{
  class Graphics final
  {
   public:
    Graphics(gsl::not_null<Window*> window);
    ~Graphics();

   private:
    struct QueueFamilyIndices
    {
      std::optional<std::uint32_t> graphics_family_ = std::nullopt;
      std::optional<std::uint32_t> presentation_family_ = std::nullopt;
      bool IsValid() const { return (graphics_family_.has_value() /* && presentation_family_.has_value()*/); };
    };

    void IntializeVulkan();
    void CreateInstance();
    void SetupDebugMessenger();
    void PickPhysicalDevice();

    // Extensions
    static gsl::span<gsl::czstring> GetSuggestedInstanceExtension();
    std::vector<gsl::czstring> GetRequiredInstanceExtension();
    static std::vector<VkExtensionProperties> GetSupportedInstanceExtensions();
    static bool AreAllExtensionsSupported(gsl::span<gsl::czstring> extension);

    // Layers
    static std::vector<VkLayerProperties> GetSupportedValidationLayers();
    static bool AreAllLayersSupported(gsl::span<gsl::czstring> layers);

    // Devices
    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
    std::vector<VkPhysicalDevice> GetAvailableDevices();
    bool isDeviceSuitable(VkPhysicalDevice device);

    VkInstance instance_ = nullptr;
    VkPhysicalDevice physical_device_ = nullptr;
    VkDebugUtilsMessengerEXT debug_messenger_ = nullptr;
    gsl::not_null<Window*> window_;
    bool validation_enabled_ = false;
  };

}  // namespace veng