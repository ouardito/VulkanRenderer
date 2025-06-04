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
      bool IsValid() const { return (graphics_family_.has_value() && presentation_family_.has_value()); };
    };

    struct SwapChainProperties
    {
      VkSurfaceCapabilitiesKHR capabilities_;
      std::vector<VkSurfaceFormatKHR> formats_;
      std::vector<VkPresentModeKHR> presentaion_modes_;

      bool IsValid() const { return (!formats_.empty() && !presentaion_modes_.empty()); };
    };

    void IntializeVulkan();
    void CreateInstance();
    void SetupDebugMessenger();
    void PickPhysicalDevice();
    void CreateLogicalDeviceAndQueues();
    void CreateSurface();

    // Instance Extensions
    static gsl::span<gsl::czstring> GetSuggestedInstanceExtension();
    std::vector<gsl::czstring> GetRequiredInstanceExtension();
    static std::vector<VkExtensionProperties> GetSupportedInstanceExtensions();
    static bool AreAllExtensionsSupported(gsl::span<gsl::czstring> extension);

    // Layers
    static std::vector<VkLayerProperties> GetSupportedValidationLayers();
    static bool AreAllLayersSupported(gsl::span<gsl::czstring> layers);

    // Physical Devices 
    std::vector<VkPhysicalDevice> GetAvailableDevices();
    bool isDeviceSuitable(VkPhysicalDevice device);

    // Physical Devices - Extensions
    std::array<gsl::czstring, 1> required_device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    std::vector<VkExtensionProperties> GetDeviceAvailableExtensions(VkPhysicalDevice physicaldevice);
    bool AreAllDeviceExtensionsSupported(VkPhysicalDevice device);

    // Physical Devices - Queue
    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

    // Physical Devices - SwapChain
    SwapChainProperties GetSwapChainProperties(VkPhysicalDevice device);

    VkInstance instance_ = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debug_messenger_ = VK_NULL_HANDLE;

    VkPhysicalDevice physical_device_ = VK_NULL_HANDLE;
    VkDevice logical_device_ = VK_NULL_HANDLE;
    VkQueue graphics_queue_ = VK_NULL_HANDLE;
    VkQueue presentation_queue_ = VK_NULL_HANDLE;

    VkSurfaceKHR surface_ = VK_NULL_HANDLE;

    gsl::not_null<Window*> window_;
    bool validation_enabled_ = false;
  };

}  // namespace veng
