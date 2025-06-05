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
    SPDLOG_INFO("Successfully created debug utils messenger.");
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
    VkDebugUtilsMessengerCreateInfoEXT creation_info = NULL_STRUCT;

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
    std::uint32_t count = 0;
    vkEnumerateInstanceLayerProperties(&count, nullptr);

    if (count == 0)
    {
      return NULL_STRUCT;
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
      SPDLOG_ERROR("Required extensions (or one of them) is not supported.");
      std::exit(EXIT_FAILURE);
    }

    return required_extensions;
  }

  std::vector<VkExtensionProperties> Graphics::GetSupportedInstanceExtensions()
  {
    std::uint32_t count = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);

    if (count == 0)
    {
      return NULL_STRUCT;
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

    VkApplicationInfo app_info = NULL_STRUCT;

    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pNext = nullptr;  // no extensions (custom)
    app_info.pApplicationName = "Application";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "VEng";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_4;

    VkInstanceCreateInfo instance_create_info = NULL_STRUCT;

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
      SPDLOG_ERROR("Failed Creating instance");
      std::exit(EXIT_FAILURE);
    }
  }

#pragma endregion

#pragma region DEVICES_AND_QUEUES

  Graphics::QueueFamilyIndices Graphics::FindQueueFamilies(VkPhysicalDevice device)
  {
    std::uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, VK_NULL_HANDLE);
    std::vector<VkQueueFamilyProperties> families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, families.data());

    auto graphics_family_it = std::find_if(
        families.begin(), families.end(),
        [](const VkQueueFamilyProperties& props)
        {
          return props.queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT);
        });

    QueueFamilyIndices QFI_result;
    QFI_result.graphics_family_ = graphics_family_it - families.begin();

    // Loop through the queue family properties
    for (std::uint32_t i = 0; i < families.size(); i++)
    {
      VkBool32 has_representation_support = false;
      vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface_, &has_representation_support);
      if (has_representation_support)
      {
        QFI_result.presentation_family_ = i;
        break;
      }
    }

    return QFI_result;
  }
  Graphics::SwapChainProperties Graphics::GetSwapChainProperties(VkPhysicalDevice device)
  {
    SwapChainProperties properties;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface_, &properties.capabilities_);

    std::uint32_t format_count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &format_count, VK_NULL_HANDLE);
    properties.formats_.resize(format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &format_count, properties.formats_.data());

    std::uint32_t presentation_modes_count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &presentation_modes_count, VK_NULL_HANDLE);
    properties.presentaion_modes_.resize(presentation_modes_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        device, surface_, &presentation_modes_count, properties.presentaion_modes_.data());

    return properties;
  }

  std::vector<VkExtensionProperties> Graphics::GetDeviceAvailableExtensions(VkPhysicalDevice physicaldevice)
  {
    std::uint32_t available_extensions_count = 0;
    vkEnumerateDeviceExtensionProperties(physicaldevice, VK_NULL_HANDLE, &available_extensions_count, VK_NULL_HANDLE);
    std::vector<VkExtensionProperties> available_extensions(available_extensions_count);
    vkEnumerateDeviceExtensionProperties(
        physicaldevice, VK_NULL_HANDLE, &available_extensions_count, available_extensions.data());

    return available_extensions;
  }
  bool IsDeviceExtensionsWithinList(const std::vector<VkExtensionProperties>& extensions, gsl::czstring name)
  {
    return std::any_of(
        extensions.begin(), extensions.end(),
        [name](const VkExtensionProperties& props)
        {
          return veng::streq(props.extensionName, name);
        });
  }
  bool Graphics::AreAllDeviceExtensionsSupported(VkPhysicalDevice device)
  {
    std::vector<VkExtensionProperties> available_device_extensions = GetDeviceAvailableExtensions(device);
    return std::all_of(
        required_device_extensions.begin(), required_device_extensions.end(),
        std::bind_front(IsDeviceExtensionsWithinList, available_device_extensions));
  }

  bool Graphics::isDeviceSuitable(VkPhysicalDevice device)
  {
    QueueFamilyIndices families = FindQueueFamilies(device);

    return families.IsValid() && AreAllDeviceExtensionsSupported(device) && GetSwapChainProperties(device).IsValid();
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

    physical_device_ = devices[0];
  }

  std::vector<VkPhysicalDevice> Graphics::GetAvailableDevices()
  {
    std::uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(instance_, &device_count, nullptr);

    if (device_count == 0)
    {
      return NULL_STRUCT;
    }
    else
    {
      std::vector<VkPhysicalDevice> devices(device_count);
      vkEnumeratePhysicalDevices(instance_, &device_count, devices.data());

      return devices;
    }
  };

  void Graphics::CreateLogicalDeviceAndQueues()
  {
    QueueFamilyIndices picked_device_families = FindQueueFamilies(physical_device_);
    if (!picked_device_families.IsValid())
    {
      SPDLOG_ERROR("Picked Device's Queue Families for the current physical device are invalid");
      std::exit(EXIT_FAILURE);
    }

    std::set<std::uint32_t> unique_queue_families = {
        picked_device_families.graphics_family_.value(), picked_device_families.presentation_family_.value()};

    std::float_t queue_priority = 1.0f;

    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    for (std::uint32_t uqf : unique_queue_families)
    {
      VkDeviceQueueCreateInfo queue_info = NULL_STRUCT;
      queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_info.queueFamilyIndex = uqf;
      queue_info.queueCount = 1;
      queue_info.pQueuePriorities = &queue_priority;
      queue_create_infos.push_back(queue_info);
    }

    VkPhysicalDeviceFeatures required_features = NULL_STRUCT;
    VkDeviceCreateInfo device_info = NULL_STRUCT;
    device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_info.queueCreateInfoCount = queue_create_infos.size();
    device_info.pQueueCreateInfos = queue_create_infos.data();
    device_info.pEnabledFeatures = &required_features;
    device_info.enabledExtensionCount = required_device_extensions.size();
    device_info.ppEnabledExtensionNames = required_device_extensions.data();
    device_info.enabledLayerCount = 0;  // DEPRECATED

    VkResult result = vkCreateDevice(physical_device_, &device_info, nullptr, &logical_device_);

    if (result != VK_SUCCESS)
    {
      SPDLOG_ERROR("Failure while creating logical device.");
      std::exit(EXIT_FAILURE);
    }

    vkGetDeviceQueue(logical_device_, picked_device_families.graphics_family_.value(), 0, &graphics_queue_);
    vkGetDeviceQueue(logical_device_, picked_device_families.presentation_family_.value(), 0, &presentation_queue_);
  }

#pragma endregion

#pragma region PRESENTATION
  void Graphics::CreateSurface()
  {
    VkResult result = glfwCreateWindowSurface(instance_, window_->GetHandle(), VK_NULL_HANDLE, &surface_);
    if (result != VK_SUCCESS)
    {
      SPDLOG_ERROR("Failed creating Window Surface");
      std::exit(EXIT_FAILURE);
    }
  }

  // Swap Chain Format Helpers
  bool IsRGBATypeFormat(VkSurfaceFormatKHR& format_properties)
  {
    return format_properties.format == VK_FORMAT_R8G8B8A8_SRGB ||
           // format_properties.format == VK_FORMAT_A8B8G8R8_SRGB_PACK32 ||
           format_properties.format == VK_FORMAT_B8G8R8A8_SRGB;
  };
  bool IsSRGBColorSpace(VkSurfaceFormatKHR& format_properties)
  {
    return format_properties.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR;
  };
  bool IsCorrectFormat(VkSurfaceFormatKHR& format_properties)
  {
    return IsRGBATypeFormat(format_properties) && IsSRGBColorSpace(format_properties);
  }
  VkSurfaceFormatKHR Graphics::ChooseSwapSurfaceFormat(gsl::span<VkSurfaceFormatKHR> formats)
  {
    if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
    {
      return {VK_FORMAT_R8G8B8A8_SRGB, VK_COLORSPACE_SRGB_NONLINEAR_KHR};
    }

    auto it = std::find_if(formats.begin(), formats.end(), IsCorrectFormat);

    if (it != formats.end())
    {
      return *it;
    }

    return formats[0];
  };

  // Swap Chain Present Mode Helpers
  bool IsMailBoxPresentMode(const VkPresentModeKHR& mode)
  {
    return mode == VK_PRESENT_MODE_MAILBOX_KHR;
  }
  VkPresentModeKHR Graphics::ChooseSwapPresentationMode(gsl::span<VkPresentModeKHR> presentation_modes)
  {
    if (std::any_of(presentation_modes.begin(), presentation_modes.end(), IsMailBoxPresentMode))
    {
      return VkPresentModeKHR::VK_PRESENT_MODE_MAILBOX_KHR;
    }
    else
    {
      return VkPresentModeKHR::VK_PRESENT_MODE_FIFO_KHR;
    }
  };

  // Swap Chain Extent helper
  VkExtent2D Graphics::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
  {
    constexpr std::uint32_t kInvalidSize = std::numeric_limits<std::uint32_t>::max();
    if (capabilities.currentExtent.width != kInvalidSize)
    {
      return capabilities.currentExtent;
    }

    glm::ivec2 size = window_->getFrameBufferSize();
    VkExtent2D actual_extent = {static_cast<std::uint32_t>(size.x), static_cast<std::uint32_t>(size.y)};
    actual_extent.width =
        std::clamp(actual_extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    actual_extent.height =
        std::clamp(actual_extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

    return actual_extent;
  };

  // Swap Chain Image Count Helper
  std::uint32_t Graphics::ChooseSwapImageCount(const VkSurfaceCapabilitiesKHR& capabilities)
  {
    std::uint32_t image_count = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && capabilities.maxImageCount < image_count)
    {
      image_count = capabilities.maxImageCount;
    }

    return image_count;
  }
  void Graphics::CreateSwapChain()
  {
    SwapChainProperties properties = GetSwapChainProperties(physical_device_);

    surface_format_ = ChooseSwapSurfaceFormat(properties.formats_);
    presentation_mode_ = ChooseSwapPresentationMode(properties.presentaion_modes_);
    extent_ = ChooseSwapExtent(properties.capabilities_);
    std::uint32_t image_count = ChooseSwapImageCount(properties.capabilities_);

    VkSwapchainCreateInfoKHR info = NULL_STRUCT;
    info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    info.surface = surface_;
    info.minImageCount = image_count;
    info.imageFormat = surface_format_.format;
    info.imageColorSpace = surface_format_.colorSpace;
    info.imageExtent = extent_;
    info.imageArrayLayers = 1;
    info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    info.presentMode = presentation_mode_;
    info.preTransform = properties.capabilities_.currentTransform;
    info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    info.clipped = VK_TRUE;
    info.oldSwapchain = VK_NULL_HANDLE;

    QueueFamilyIndices indices = FindQueueFamilies(physical_device_);

    if (indices.graphics_family_ != indices.presentation_family_)
    {
      std::array<std::uint32_t, 2> family_indices = {
          indices.graphics_family_.value(), indices.presentation_family_.value()};
      info.imageSharingMode = VkSharingMode::VK_SHARING_MODE_CONCURRENT;
      info.queueFamilyIndexCount = family_indices.size();
      info.pQueueFamilyIndices = family_indices.data();
    }
    else
    {
      info.imageSharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
    }

    VkResult result = vkCreateSwapchainKHR(logical_device_, &info, VK_NULL_HANDLE, &swap_chain_);
    if (result != VK_SUCCESS)
    {
      SPDLOG_ERROR("Failed while creating the swapchain");
      std::exit(EXIT_FAILURE);
    }

    swap_chain_images_.resize(image_count);
    vkGetSwapchainImagesKHR(logical_device_, swap_chain_, &image_count, swap_chain_images_.data());
  }

  // Image views (aka Frames)
  void Graphics::CreateImageViews()
  {
    swap_chain_image_views_.resize(swap_chain_images_.size());
    auto it = swap_chain_image_views_.begin();
    for (VkImage image : swap_chain_images_)
    {
      VkImageViewCreateInfo info = NULL_STRUCT;
      info.sType = VkStructureType::VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
      info.image = image;
      info.viewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D;
      info.format = surface_format_.format;

      info.components.r = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY;
      info.components.g = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY;
      info.components.b = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY;
      info.components.a = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY;

      info.subresourceRange.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;

      info.subresourceRange.baseMipLevel = 0;
      info.subresourceRange.levelCount = 1;
      info.subresourceRange.baseArrayLayer = 0;
      info.subresourceRange.layerCount = 1;

      VkResult result = vkCreateImageView(logical_device_, &info, VK_NULL_HANDLE, &*it);
      if (result != VK_SUCCESS)
      {
        SPDLOG_ERROR("Failed Creating an image view");
        std::exit(EXIT_FAILURE);
      }
      it = std::next(it);
    }
  }

#pragma endregion

#pragma region GRAPHICS_PIPELINE
  VkShaderModule Graphics::CreateShaderModule(gsl::span<std::uint8_t> buffer)
  {
    if (buffer.empty())
    {
      return VK_NULL_HANDLE;
    }

    VkShaderModuleCreateInfo info = NULL_STRUCT;
    info.sType = VkStructureType::VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    info.codeSize = buffer.size();
    info.pCode = reinterpret_cast<std::uint32_t*>(buffer.data());

    VkShaderModule shader_module;

    VkResult result = vkCreateShaderModule(logical_device_, &info, VK_NULL_HANDLE, &shader_module);
    if (result != VK_SUCCESS)
    {
      return VK_NULL_HANDLE;
    }

    return shader_module;
  };

  void Graphics::CreateGraphicsPipeline()
  {
    // Shader Module for the Vertex Shader
    std::vector<std::uint8_t> basic_vertex_data = ReadFile("./basic.vert.spv");
    VkShaderModule vertex_shader = CreateShaderModule(basic_vertex_data);
    gsl::final_action _destroy_vertex_shader(
        [this, vertex_shader]()
        {
          vkDestroyShaderModule(logical_device_, vertex_shader, VK_NULL_HANDLE);
        });

    // Shader Module for the Fragment Shader
    std::vector<std::uint8_t> basic_fragment_data = ReadFile("./basic.frag.spv");
    VkShaderModule fragment_shader = CreateShaderModule(basic_fragment_data);
    gsl::final_action _destroy_fragment_shader(
        [this, fragment_shader]()
        {
          vkDestroyShaderModule(logical_device_, fragment_shader, VK_NULL_HANDLE);
        });

    // Shader Module Null Checking
    if (vertex_shader == VK_NULL_HANDLE || fragment_shader == VK_NULL_HANDLE)
    {
      SPDLOG_ERROR("Vertex Shader or Fragment shader is null");
      std::exit(EXIT_FAILURE);
    }

    // Shader Staging
    VkPipelineShaderStageCreateInfo vertex_stage_info = NULL_STRUCT;
    vertex_stage_info.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertex_stage_info.stage = VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
    vertex_stage_info.module = vertex_shader;
    vertex_stage_info.pName = "main";

    VkPipelineShaderStageCreateInfo fragment_stage_info = NULL_STRUCT;
    fragment_stage_info.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragment_stage_info.stage = VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
    fragment_stage_info.module = fragment_shader;
    fragment_stage_info.pName = "main";

    std::array<VkPipelineShaderStageCreateInfo, 2> stage_infos = {vertex_stage_info, fragment_stage_info};

    // Dynamic State Create info
    std::array<VkDynamicState, 2> dynamic_states = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    VkPipelineDynamicStateCreateInfo dynamic_state_info = NULL_STRUCT;
    dynamic_state_info.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state_info.dynamicStateCount = dynamic_states.size();
    dynamic_state_info.pDynamicStates = dynamic_states.data();

    // Viewport State Create info
    VkViewport viewport = GetViewport();  // Create the Viewport

    VkRect2D scissor = GetScissor();  // Create the Scissor

    VkPipelineViewportStateCreateInfo viewport_state_info = NULL_STRUCT;
    viewport_state_info.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state_info.viewportCount = 1;
    viewport_state_info.pViewports = &viewport;
    viewport_state_info.scissorCount = 1;
    viewport_state_info.pScissors = &scissor;

    // Vertex Input State Create info
    VkPipelineVertexInputStateCreateInfo vertex_input_state_info = NULL_STRUCT;
    vertex_input_state_info.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_state_info.vertexBindingDescriptionCount = 0;
    vertex_input_state_info.vertexAttributeDescriptionCount = 0;

    // Input Assembly State Create info
    VkPipelineInputAssemblyStateCreateInfo input_assembly_state_info = NULL_STRUCT;
    input_assembly_state_info.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly_state_info.topology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly_state_info.primitiveRestartEnable = VK_FALSE;

    // Rasterization State Create info
    VkPipelineRasterizationStateCreateInfo rasterization_state_info = NULL_STRUCT;
    rasterization_state_info.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterization_state_info.depthBiasClamp = VK_FALSE;
    rasterization_state_info.rasterizerDiscardEnable = VK_FALSE;
    rasterization_state_info.polygonMode = VkPolygonMode::VK_POLYGON_MODE_FILL;
    rasterization_state_info.lineWidth = 1.0f;
    rasterization_state_info.cullMode = VkCullModeFlagBits::VK_CULL_MODE_NONE;
    rasterization_state_info.frontFace = VkFrontFace::VK_FRONT_FACE_CLOCKWISE;
    rasterization_state_info.depthBiasEnable = VK_FALSE;

    // Multisample State Create info
    VkPipelineMultisampleStateCreateInfo multisampling_state_info = NULL_STRUCT;
    multisampling_state_info.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling_state_info.sampleShadingEnable = VK_FALSE;
    multisampling_state_info.rasterizationSamples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;

    // Color Blend State Create info
    VkPipelineColorBlendAttachmentState color_blend_attachment_state =
        NULL_STRUCT;  // Responsible for Blending with the background
    color_blend_attachment_state.blendEnable = VK_FALSE;
    color_blend_attachment_state.colorWriteMask =
        VkColorComponentFlagBits::VK_COLOR_COMPONENT_R_BIT | VkColorComponentFlagBits::VK_COLOR_COMPONENT_G_BIT |
        VkColorComponentFlagBits::VK_COLOR_COMPONENT_B_BIT | VkColorComponentFlagBits::VK_COLOR_COMPONENT_A_BIT;
    VkPipelineColorBlendStateCreateInfo color_blend_state_info = NULL_STRUCT;
    color_blend_state_info.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blend_state_info.logicOpEnable = VK_FALSE;
    color_blend_state_info.attachmentCount = 1;
    color_blend_state_info.pAttachments = &color_blend_attachment_state;

    // Pipeline Layout Create Info
    VkPipelineLayoutCreateInfo pipeline_layout_info = NULL_STRUCT;
    pipeline_layout_info.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    VkResult layout_result =
        vkCreatePipelineLayout(logical_device_, &pipeline_layout_info, VK_NULL_HANDLE, &pipeline_layout_);

    if (layout_result != VK_SUCCESS)
    {
      SPDLOG_ERROR("Failed creating the pipeline layout");
      std::exit(EXIT_FAILURE);
    }

    // Pipeline creation
    VkGraphicsPipelineCreateInfo graphics_pipeline_info = NULL_STRUCT;
    graphics_pipeline_info.sType = VkStructureType::VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphics_pipeline_info.stageCount = stage_infos.size();
    graphics_pipeline_info.pStages = stage_infos.data();
    graphics_pipeline_info.layout = pipeline_layout_;
    graphics_pipeline_info.subpass = 0;
    graphics_pipeline_info.renderPass = render_pass_;

    graphics_pipeline_info.pDynamicState = &dynamic_state_info;
    graphics_pipeline_info.pViewportState = &viewport_state_info;
    graphics_pipeline_info.pVertexInputState = &vertex_input_state_info;
    graphics_pipeline_info.pInputAssemblyState = &input_assembly_state_info;
    graphics_pipeline_info.pRasterizationState = &rasterization_state_info;
    graphics_pipeline_info.pMultisampleState = &multisampling_state_info;
    graphics_pipeline_info.pColorBlendState = &color_blend_state_info;
    graphics_pipeline_info.pDepthStencilState = nullptr;  // TODO: IMPLEMENT THE DEPTH STATE

    VkResult pipeline_result = vkCreateGraphicsPipelines(
        logical_device_, VK_NULL_HANDLE, 1, &graphics_pipeline_info, VK_NULL_HANDLE, &pipeline_);
    if (pipeline_result != VK_SUCCESS)
    {
      SPDLOG_ERROR("Failed creating the graphics pipeline");
      std::exit(EXIT_FAILURE);
    }
  }

  VkViewport Graphics::GetViewport()
  {
    VkViewport viewport = NULL_STRUCT;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<std::float_t>(extent_.width);
    viewport.height = static_cast<std::float_t>(extent_.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    return viewport;
  }

  VkRect2D Graphics::GetScissor()
  {
    VkRect2D scissor = NULL_STRUCT;
    scissor.extent = {extent_};
    scissor.offset = {0, 0};

    return scissor;
  }

  void Graphics::CreateRenderPass()
  {
    VkAttachmentDescription color_attachment = NULL_STRUCT;
    color_attachment.format = surface_format_.format;
    color_attachment.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VkImageLayout::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_ref = NULL_STRUCT;
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription main_subpass = NULL_STRUCT;
    main_subpass.pipelineBindPoint = VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS;
    main_subpass.colorAttachmentCount = 1;
    main_subpass.pColorAttachments = &color_attachment_ref;

    VkRenderPassCreateInfo render_pass_info = NULL_STRUCT;
    render_pass_info.sType = VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = 1;
    render_pass_info.pAttachments = &color_attachment;
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &main_subpass;

    VkResult result = vkCreateRenderPass(logical_device_, &render_pass_info, VK_NULL_HANDLE, &render_pass_);
    if (result != VK_SUCCESS)
    {
      SPDLOG_ERROR("Failed creating the render pass");

      std::exit(EXIT_FAILURE);
    }
  }

#pragma endregion

#pragma region DRAWING

  void Graphics::CreateFramebuffers()
  {
    swap_chain_framebuffers_.resize(swap_chain_image_views_.size());

    for (std::uint32_t imageView = 0; imageView < swap_chain_image_views_.size(); imageView++)
    {
      VkFramebufferCreateInfo info = NULL_STRUCT;
      info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
      info.renderPass = render_pass_;
      info.attachmentCount = 1;
      info.pAttachments = &swap_chain_image_views_[imageView];
      info.width = extent_.width;
      info.height = extent_.height;
      info.layers = 1;

      VkResult result = vkCreateFramebuffer(logical_device_, &info, nullptr, &swap_chain_framebuffers_[imageView]);

      if (result != VK_SUCCESS)
      {
        SPDLOG_ERROR("Failed to create a frame buffer");
        std::exit(EXIT_FAILURE);
      }
    }
  }

  void Graphics::CreateCommandPool()
  {
    QueueFamilyIndices indices = FindQueueFamilies(physical_device_);
    VkCommandPoolCreateInfo pool_info = NULL_STRUCT;
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool_info.queueFamilyIndex = indices.graphics_family_.value();

    VkResult pool_result = vkCreateCommandPool(logical_device_, &pool_info, VK_NULL_HANDLE, &command_pool_);

    if (pool_result != VK_SUCCESS)
    {
      SPDLOG_ERROR("Failed to create the command pool, exiting...");
      std::exit(EXIT_FAILURE);
    }
  }

  void Graphics::CreateCommandBuffer()
  {
    VkCommandBufferAllocateInfo info = NULL_STRUCT;
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.commandPool = command_pool_;
    info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    info.commandBufferCount = 1;

    VkResult result = vkAllocateCommandBuffers(logical_device_, &info, &command_buffer_);
    if (result != VK_SUCCESS)
    {
      SPDLOG_ERROR("Failed to Allocate command buffers, exiting...");
      std::exit(EXIT_FAILURE);
    }
  }

  void Graphics::BeginCommands()
  {
    VkCommandBufferBeginInfo begin_info = NULL_STRUCT;
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    VkResult begin_result = vkBeginCommandBuffer(command_buffer_, &begin_info);
    if (begin_result != VK_SUCCESS)
    {
      SPDLOG_ERROR("Failed to Begin commands buffer, exiting...");
      throw std::runtime_error("Failed to begin commands buffer!");
    }

    VkClearValue clear_color = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    VkRenderPassBeginInfo render_pass_begin_info = NULL_STRUCT;
    render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_begin_info.renderPass = render_pass_;
    render_pass_begin_info.framebuffer = swap_chain_framebuffers_[current_image_index_];
    render_pass_begin_info.renderArea.offset = {0, 0};
    render_pass_begin_info.renderArea.extent = extent_;
    render_pass_begin_info.clearValueCount = 1;
    render_pass_begin_info.pClearValues = &clear_color;

    vkCmdBeginRenderPass(command_buffer_, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(command_buffer_, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_);
    VkViewport viewport = GetViewport();
    VkRect2D scissor = GetScissor();

    vkCmdSetViewport(command_buffer_, 0, 1, &viewport);
    vkCmdSetScissor(command_buffer_, 0, 1, &scissor);
  }

  void Graphics::RenderTriangle()
  {
    vkCmdDraw(command_buffer_, 3, 1, 0, 0);
  }

  void Graphics::EndCommands()
  {
    vkCmdEndRenderPass(command_buffer_);

    VkResult end_buffer_result = vkEndCommandBuffer(command_buffer_);
    if (end_buffer_result != VK_SUCCESS)
    {
      SPDLOG_ERROR("Failed to End commands buffer, exiting...");
      throw std::runtime_error("Failed to end commands buffer!");
    }
  }

  void Graphics::CreateSignals()
  {
    VkSemaphoreCreateInfo semaphore_info = NULL_STRUCT;
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if (vkCreateSemaphore(logical_device_, &semaphore_info, VK_NULL_HANDLE, &image_available_signal_) != VK_SUCCESS)
    {
      SPDLOG_ERROR("Failed to create the semaphore for image available, exiting...");
      std::exit(EXIT_FAILURE);
    }
    if (vkCreateSemaphore(logical_device_, &semaphore_info, VK_NULL_HANDLE, &render_finished_singal_) != VK_SUCCESS)
    {
      SPDLOG_ERROR("Failed to create the semaphore for render finished, exiting...");
      std::exit(EXIT_FAILURE);
    }

    VkFenceCreateInfo fence_info = NULL_STRUCT;
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;  // Start the fence in signaled state

    if (vkCreateFence(logical_device_, &fence_info, VK_NULL_HANDLE, &still_rendering_fence_) != VK_SUCCESS)
    {
      SPDLOG_ERROR("Failed to create the still rendering fence, exiting...");
      std::exit(EXIT_FAILURE);
    }
  }

  void Graphics::BeginFrame()
  {
    // Wait for the fence to be signaled before starting a new frame
    vkWaitForFences(logical_device_, 1, &still_rendering_fence_, VK_TRUE, UINT64_MAX);
    // Reset the fence for the next frame
    vkResetFences(logical_device_, 1, &still_rendering_fence_);

    // Reset the command buffer for the new frame
    vkAcquireNextImageKHR(
        logical_device_, swap_chain_, UINT64_MAX, image_available_signal_, VK_NULL_HANDLE, &current_image_index_);

    // Begin the command buffer for the current frame
    BeginCommands();
  }

  void Graphics::EndFrame()
  {
    // End the command buffer for the current frame
    EndCommands();

    // Submit the command buffer for execution
    VkSubmitInfo submit_info = NULL_STRUCT;
    VkPipelineStageFlags wait_stages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &image_available_signal_;
    submit_info.pWaitDstStageMask = &wait_stages;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer_;
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &render_finished_singal_;

    // Submit the command buffer to the graphics queue
    // The command buffer will be executed when the image is available
    // and the render finished semaphore will be signaled when the command buffer execution is complete.
    // The still_rendering_fence will be signaled when the command buffer execution is complete.
    VkResult submit_result = vkQueueSubmit(graphics_queue_, 1, &submit_info, still_rendering_fence_);
    if (submit_result != VK_SUCCESS)
    {
      SPDLOG_ERROR("Failed to submit the draw command buffer, exiting...");
      throw std::runtime_error("Failed to submit draw command buffer!");
    }

    // Present the rendered image to the swap chain
    VkPresentInfoKHR present_info = NULL_STRUCT;
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &render_finished_singal_;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &swap_chain_;
    present_info.pImageIndices = &current_image_index_;

    vkQueuePresentKHR(presentation_queue_, &present_info);
  }

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
    // IMPORTANT : The destroy should be in reverse order of the create. Refer to IntializeVulkan() for ref.

    auto start = std::chrono::high_resolution_clock::now();

    // If logical device is initiated
    if (logical_device_ != VK_NULL_HANDLE)
    {
      SPDLOG_TRACE("Waiting for the device to finish all operations before destroying resources.");
      vkDeviceWaitIdle(logical_device_);  // Wait for the device to finish all operations before destroying resources
      SPDLOG_TRACE("Finished waiting for the device to finish all operations.");

      // Destroy the semaphores
      if (image_available_signal_ != VK_NULL_HANDLE)
      {
        SPDLOG_TRACE("Invoking Available image signal semaphore Destruction");
        vkDestroySemaphore(logical_device_, image_available_signal_, VK_NULL_HANDLE);
        SPDLOG_TRACE("Finished");
      }
      if (render_finished_singal_ != VK_NULL_HANDLE)
      {
        SPDLOG_TRACE("Invoking render finished signal semaphore Destruction");
        vkDestroySemaphore(logical_device_, render_finished_singal_, VK_NULL_HANDLE);
        SPDLOG_TRACE("Finished");
      }

      // Destroy the fence
      if (still_rendering_fence_ != VK_NULL_HANDLE)
      {
        SPDLOG_TRACE("Invoking still rendering fence Destruction");
        vkDestroyFence(logical_device_, still_rendering_fence_, VK_NULL_HANDLE);
        SPDLOG_TRACE("Finished");
      }

      // Destroy the command pool
      if (command_pool_ != VK_NULL_HANDLE)
      {
        SPDLOG_TRACE("Invoking command pool Destruction");
        vkDestroyCommandPool(logical_device_, command_pool_, VK_NULL_HANDLE);
        SPDLOG_TRACE("Finished");
      }

      // Destroy Framebuffers
      for (VkFramebuffer framebuffer : swap_chain_framebuffers_)
      {
        SPDLOG_TRACE("Invoking a Frame buffer Destruction");
        vkDestroyFramebuffer(logical_device_, framebuffer, VK_NULL_HANDLE);
        SPDLOG_TRACE("Finished");
      }

      // Destroy the graphics pipeline
      if (pipeline_ != VK_NULL_HANDLE)
      {
        SPDLOG_TRACE("Invoking PipeLine Destruction");
        vkDestroyPipeline(logical_device_, pipeline_, VK_NULL_HANDLE);
        SPDLOG_TRACE("Finished");
      }
      // Destroy Pipeline Layout
      if (pipeline_layout_ != VK_NULL_HANDLE)
      {
        SPDLOG_TRACE("Invoking PipleLine Layout Destruction");
        vkDestroyPipelineLayout(logical_device_, pipeline_layout_, VK_NULL_HANDLE);
        SPDLOG_TRACE("Finished");
      }
      // Destroy Render Pass
      if (render_pass_ != VK_NULL_HANDLE)
      {
        SPDLOG_TRACE("Invoking Render Pass Destruction");
        vkDestroyRenderPass(logical_device_, render_pass_, VK_NULL_HANDLE);
        SPDLOG_TRACE("Finished");
      }
      // Destroy Swap Chain Images
      for (VkImageView image : swap_chain_image_views_)
      {
        SPDLOG_TRACE("Invoking a swapchain image view Destruction");
        vkDestroyImageView(logical_device_, image, VK_NULL_HANDLE);
        SPDLOG_TRACE("Finished");
      }

      // Destroy Swap Chain
      if (swap_chain_ != VK_NULL_HANDLE)
      {
        SPDLOG_TRACE("Invoking swapchain Destruction");
        vkDestroySwapchainKHR(logical_device_, swap_chain_, VK_NULL_HANDLE);
        SPDLOG_TRACE("Finished");
      }

      // Then destroy the logical device
      SPDLOG_TRACE("Invoking logical device Destruction");
      vkDestroyDevice(logical_device_, VK_NULL_HANDLE);
      SPDLOG_TRACE("Finished");
    }

    // If instance is initiated
    if (instance_ != VK_NULL_HANDLE)
    {
      if (surface_ != VK_NULL_HANDLE)
      {
        // Destroy the Surface
        SPDLOG_TRACE("Invoking Surface Destruction");
        vkDestroySurfaceKHR(instance_, surface_, VK_NULL_HANDLE);
        SPDLOG_TRACE("Finished");
      }
      if (debug_messenger_ != VK_NULL_HANDLE)
      {
        // Destroy the debug messenger
        SPDLOG_TRACE("Invoking Debug messenger Destruction");
        vkDestroyDebugUtilsMessengerEXT(instance_, debug_messenger_, VK_NULL_HANDLE);
        SPDLOG_TRACE("Finished");
      }

      // Finally Destroy the instance
      SPDLOG_TRACE("Invoking Instance Destruction");
      vkDestroyInstance(instance_, VK_NULL_HANDLE);
      SPDLOG_TRACE("Finished");
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    SPDLOG_INFO("Graphics::~Graphics() took {}ms", duration.count());

    SPDLOG_INFO("Finished Destructing all non-null vk objects.");
  }

  void Graphics::IntializeVulkan()
  {
    auto start = std::chrono::high_resolution_clock::now();
    CreateInstance();
    SetupDebugMessenger();
    CreateSurface();
    PickPhysicalDevice();
    CreateLogicalDeviceAndQueues();

    CreateSwapChain();
    CreateImageViews();
    CreateRenderPass();
    CreateGraphicsPipeline();
    CreateFramebuffers();

    CreateCommandPool();
    CreateCommandBuffer();
    CreateSignals();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    SPDLOG_DEBUG("Graphics::InitializeVulkan() took {}ms", duration.count());
  }

}  // namespace veng
