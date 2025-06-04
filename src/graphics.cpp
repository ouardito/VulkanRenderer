#include <graphics.h>

namespace veng
{
  Graphics::Graphics(gsl::not_null<Window*> window) : window_(window)
  {
    IntializeVulkan();
  }

  Graphics::~Graphics()
  {
    if (instance_ != nullptr)
    {
      vkDestroyInstance(instance_, nullptr);
    }
  }

  void Graphics::IntializeVulkan()
  {
    CreateInstance();
  }

  void Graphics::CreateInstance()
  {
    gsl::span<gsl::czstring> suggested_extensions = GetSuggestedInstanceExtension();
    if (!AreAllExtensionsSupported(suggested_extensions))
    {
      std::exit(EXIT_FAILURE);
    };

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
    instance_create_info.pNext = nullptr;
    // instance_create_info.flags;
    instance_create_info.pApplicationInfo = &app_info;
    instance_create_info.enabledLayerCount = 0;
    // instance_create_info.ppEnabledLayerNames;

    instance_create_info.enabledExtensionCount = suggested_extensions.size();
    instance_create_info.ppEnabledExtensionNames = suggested_extensions.data();

    VkResult result = vkCreateInstance(&instance_create_info, nullptr, &instance_);
    if (result != VK_SUCCESS)
    {
      std::exit(EXIT_FAILURE);
    }
  }

  /////////////
  // HELPERS //
  /////////////

  gsl::span<gsl::czstring> Graphics::GetSuggestedInstanceExtension()
  {
    std::uint32_t glfw_extensions_count = 0;
    gsl::czstring* glfw_extensions;
    glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extensions_count);

    return {glfw_extensions, glfw_extensions_count};
  }

  std::vector<VkExtensionProperties> Graphics::GetSupportedExtensions()
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

  bool ExtensionMatchesNames(gsl::czstring name, const VkExtensionProperties& property)
  {
    return veng::streq(property.extensionName, name);
  };

  bool IsExtensionSupported(gsl::span<VkExtensionProperties> extensions, gsl::czstring name)
  {
    return std::any_of(extensions.begin(), extensions.end(), std::bind_front(ExtensionMatchesNames, name));
  };

  bool Graphics::AreAllExtensionsSupported(gsl::span<gsl::czstring> extension)
  {
    std::vector<VkExtensionProperties> supported_extensions = GetSupportedExtensions();

    return std::all_of(extension.begin(), extension.end(), std::bind_front(IsExtensionSupported, supported_extensions));
  }

}  // namespace veng