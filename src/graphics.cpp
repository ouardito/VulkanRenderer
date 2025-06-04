#include <graphics.h>

#pragma region VK_FUNC_EXT_IMPL
VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* ext, const VkAllocationCallbacks* allocator,
    VkDebugUtilsMessengerEXT* debug_messenger)
{
  PFN_vkCreateDebugUtilsMessengerEXT function = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
      vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
  if (function != nullptr)
  {
    SPDLOG_INFO("Succesfully created debug utils messenger.");
    return function(instance, ext, nullptr, debug_messenger);
  }
  else
  {
    SPDLOG_ERROR("Could not create debug utils messenger.");
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDebugUtilsMessengerEXT(
    VkInstance instance, VkDebugUtilsMessengerEXT debug_messenger, const VkAllocationCallbacks* allocator)
{
  PFN_vkDestroyDebugUtilsMessengerEXT function = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
      vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));

  if (function != nullptr)
  {
    function(instance, debug_messenger, nullptr);
    SPDLOG_INFO("Succesfully destroyed debug utils messenger.");
  }
  else
  {
    SPDLOG_ERROR("Could not destroyed debug utils messenger");
  }
}
#pragma endregion

namespace veng
{

#pragma region VALIDATION_LAYERS
  static VKAPI_ATTR VkBool32 VKAPI_CALL ValidationCallback(
      VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type,
      const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data)
  {
    if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
      SPDLOG_WARN("[VULKAN VALIDATION CALLBACK] | Validation Warning: {}", callback_data->pMessage);
    }
    else
    {
      SPDLOG_ERROR("[VULKAN VALIDATION CALLBACK] | Validation Error: {}", callback_data->pMessage);
    }

    return VK_FALSE;
  };

  static VkDebugUtilsMessengerCreateInfoEXT GetCreateMessengerInfo()
  {
    VkDebugUtilsMessengerCreateInfoEXT creation_info = {};

    // const void* pNext;
    // VkDebugUtilsMessengerCreateFlagsEXT flags;
    creation_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    creation_info.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    creation_info.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    creation_info.pfnUserCallback = ValidationCallback;
    creation_info.pUserData = nullptr;

    return creation_info;
  };

  // Validation layers
  std::vector<VkLayerProperties> Graphics::GetSupportedValidationLayers()
  {
    std::uint32_t count;
    vkEnumerateInstanceLayerProperties(&count, nullptr);

    if (count == 0)
    {
      return {};
    }
    else
    {
      std::vector<VkLayerProperties> properties(count);
      vkEnumerateInstanceLayerProperties(&count, properties.data());
      return properties;
    }
  }

  // Functional Helpers - TO BE USED ONLY FOR AreAllLayersSupported() //
  bool LayerMatchesNames(gsl::czstring name, const VkLayerProperties& property)
  {
    return veng::streq(property.layerName, name);
  };

  bool IsLayerSupported(gsl::span<VkLayerProperties> layers, gsl::czstring name)
  {
    return std::any_of(layers.begin(), layers.end(), std::bind_front(LayerMatchesNames, name));
  };
  //////////////////////////////////////////////////////////////////////////

  bool Graphics::AreAllLayersSupported(gsl::span<gsl::czstring> layers)
  {
    std::vector<VkLayerProperties> supported_layers = GetSupportedValidationLayers();

    return std::all_of(layers.begin(), layers.end(), std::bind_front(IsLayerSupported, supported_layers));
  }

  void Graphics::SetupDebugMessenger()
  {
    if (!validation_enabled_)
    {
      return;
    }

    VkDebugUtilsMessengerCreateInfoEXT info = GetCreateMessengerInfo();
    VkResult result = vkCreateDebugUtilsMessengerEXT(instance_, &info, nullptr, &debug_messenger_);
    if (result != VK_SUCCESS)
    {
      SPDLOG_ERROR("Failed To create DebugMessenger.");
      return;
    }
  }

#pragma endregion

#pragma region INSTANCE_AND_EXTENSIONS

  // Instance Extension
  gsl::span<gsl::czstring> Graphics::GetSuggestedInstanceExtension()
  {
    std::uint32_t glfw_extensions_count = 0;
    gsl::czstring* glfw_extensions;
    glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extensions_count);

    return {glfw_extensions, glfw_extensions_count};
  }

  std::vector<gsl::czstring> Graphics::GetRequiredInstanceExtension()
  {
    gsl::span<gsl::czstring> suggested_extensions = GetSuggestedInstanceExtension();
    std::vector<gsl::czstring> required_extensions(suggested_extensions.size());
    std::copy(suggested_extensions.begin(), suggested_extensions.end(), required_extensions.begin());

    if (validation_enabled_)
    {
      required_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    if (!AreAllExtensionsSupported(required_extensions))
    {
      std::exit(EXIT_FAILURE);
    }

    return required_extensions;
  }

  std::vector<VkExtensionProperties> Graphics::GetSupportedInstanceExtensions()
  {
    std::uint32_t count;
    vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);

    if (count == 0)
    {
      return {};
    }
    else
    {
      std::vector<VkExtensionProperties> properties(count);
      vkEnumerateInstanceExtensionProperties(nullptr, &count, properties.data());
      return properties;
    }
  }

  // Functional Helpers - TO BE USED ONLY FOR AreAllExtensionsSupported() //
  bool ExtensionMatchesNames(gsl::czstring name, const VkExtensionProperties& property)
  {
    return veng::streq(property.extensionName, name);
  };

  bool IsExtensionSupported(gsl::span<VkExtensionProperties> extensions, gsl::czstring name)
  {
    return std::any_of(extensions.begin(), extensions.end(), std::bind_front(ExtensionMatchesNames, name));
  };
  //////////////////////////////////////////////////////////////////////////

  bool Graphics::AreAllExtensionsSupported(gsl::span<gsl::czstring> extension)
  {
    std::vector<VkExtensionProperties> supported_extensions = GetSupportedInstanceExtensions();

    return std::all_of(extension.begin(), extension.end(), std::bind_front(IsExtensionSupported, supported_extensions));
  }

  void Graphics::CreateInstance()
  {
    std::array<gsl::czstring, 1> validation_layers = {"VK_LAYER_KHRONOS_validation"};
    if (!AreAllLayersSupported(validation_layers))
    {
      validation_enabled_ = false;
    }

    std::vector<gsl::czstring> required_extensions = GetRequiredInstanceExtension();

    VkApplicationInfo app_info = {};

    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pNext = nullptr;  // no extensions (custom)
    app_info.pApplicationName = "Application";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "VEng";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_4;

    VkInstanceCreateInfo instance_create_info = {};

    instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_create_info.pApplicationInfo = &app_info;
    instance_create_info.enabledExtensionCount = required_extensions.size();
    instance_create_info.ppEnabledExtensionNames = required_extensions.data();
    // instance_create_info.flags = TODO;
    auto messenger_creation_info = GetCreateMessengerInfo();

    if (validation_enabled_)
    {
      instance_create_info.pNext = &messenger_creation_info;
      instance_create_info.enabledLayerCount = validation_layers.size();
      instance_create_info.ppEnabledLayerNames = validation_layers.data();
    }
    else
    {
      instance_create_info.enabledLayerCount = 0;
      instance_create_info.ppEnabledLayerNames = nullptr;
    }

    VkResult result = vkCreateInstance(&instance_create_info, nullptr, &instance_);
    if (result != VK_SUCCESS)
    {
      std::exit(EXIT_FAILURE);
    }
  }

#pragma endregion

#pragma region DEVICES_AND_QUEUES

  bool Graphics::isDeviceSuitable(VkPhysicalDevice device)
  {
    VkPhysicalDeviceProperties device_properties;
    vkGetPhysicalDeviceProperties(device, &device_properties);

    VkPhysicalDeviceFeatures device_features;
    vkGetPhysicalDeviceFeatures(device, &device_features);

    return true;  // TODO: Add criterias
  }

  void Graphics::PickPhysicalDevice()
  {
    std::vector<VkPhysicalDevice> devices = GetAvailableDevices();

    std::erase_if(devices, std::not_fn(std::bind_front(&Graphics::isDeviceSuitable, this)));

    if (devices.empty())
    {
      SPDLOG_ERROR("No physical devices found that match Device Criterias.");
      std::exit(EXIT_FAILURE);
    }

    for (VkPhysicalDevice device : devices)
    {
      VkPhysicalDeviceProperties device_properties;
      vkGetPhysicalDeviceProperties(device, &device_properties);
      SPDLOG_INFO("Devices found:{}", device_properties.deviceName);
    }
  }

  std::vector<VkPhysicalDevice> Graphics::GetAvailableDevices()
  {
    std::uint32_t device_count;
    vkEnumeratePhysicalDevices(instance_, &device_count, nullptr);

    if (device_count == 0)
    {
      return {};
    }
    else
    {
      std::vector<VkPhysicalDevice> devices(device_count);
      vkEnumeratePhysicalDevices(instance_, &device_count, devices.data());

      return devices;
    }
  };
#pragma endregion

  Graphics::Graphics(gsl::not_null<Window*> window) : window_(window)
  {
#ifndef NDEBUG
    validation_enabled_ = true;
#endif  // !NDEBUG

    IntializeVulkan();
  }

  Graphics::~Graphics()
  {
    if (instance_ != nullptr)
    {
      if (debug_messenger_ != nullptr)
      {
        vkDestroyDebugUtilsMessengerEXT(instance_, debug_messenger_, nullptr);
      }
      vkDestroyInstance(instance_, nullptr);
    }
  }

  void Graphics::IntializeVulkan()
  {
    CreateInstance();
    SetupDebugMessenger();
    PickPhysicalDevice();
  }

}  // namespace veng