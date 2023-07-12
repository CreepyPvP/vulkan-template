#include <cstdint>
#include <cstring>
#include <optional>
#include <vector>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <iostream>
#include <stdexcept>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;
const std::vector<const char *> validation_layers = {
    "VK_LAYER_KHRONOS_validation",
};

#ifdef NDEBUG
const bool enable_validation_layers = false;
#else
const bool enable_validation_layers = true;
#endif

struct QueueFamilyIndices {
  std::optional<uint32_t> graphics_family;

  bool is_complete() { return graphics_family.has_value(); }
};

class HelloTriangleApplication {
public:
  void run() {
    init_window();
    init_vulkan();
    main_loop();
    cleanup();
  }

private:
  void init_window() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
  }

  void init_vulkan() {
    create_instance();
    pick_physical_device();
  }

  void create_instance() {
    if (!check_validation_layer_support()) {
      throw std::runtime_error(
          "validation layers requested, but not available!");
    }
    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "Hello Triangle";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "No Engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_0;
    VkInstanceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;
    uint32_t glfw_extension_count = 0;
    const char **glfw_extensions;
    glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
    create_info.enabledExtensionCount = glfw_extension_count;
    create_info.ppEnabledExtensionNames = glfw_extensions;
    create_info.enabledLayerCount = 0;
    if (enable_validation_layers) {
      std::cout << "validation layers enabled" << std::endl;
      create_info.enabledLayerCount =
          static_cast<uint32_t>(validation_layers.size());
      create_info.ppEnabledLayerNames = validation_layers.data();
    } else {
      create_info.enabledLayerCount = 0;
    }
    if (vkCreateInstance(&create_info, nullptr, &instance)) {
      throw std::runtime_error("failed to create instance");
    }
  }

  void pick_physical_device() {
    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(instance, &device_count, nullptr);
    if (device_count == 0) {
      throw std::runtime_error("failed to find GPU with Vulkan support!");
    }
    std::vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(instance, &device_count, devices.data());
    for (const auto &device : devices) {
      if (is_device_suitable(device)) {
        physical_device = device;
        break;
      }
    }
    if (physical_device == VK_NULL_HANDLE) {
      throw new std::runtime_error("failed to find a suitable GPU");
    }
  }

  bool is_device_suitable(VkPhysicalDevice device) {
    // VkPhysicalDeviceProperties device_properties;
    // VkPhysicalDeviceFeatures device_features;
    // vkGetPhysicalDeviceProperties(device, &device_properties);
    // vkGetPhysicalDeviceFeatures(device, &device_features);
    QueueFamilyIndices indices = find_queue_families(device);
    return indices.is_complete();
  }

  QueueFamilyIndices find_queue_families(VkPhysicalDevice device) {
    QueueFamilyIndices indices;
    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);
    std::vector<VkQueueFamilyProperties> families(count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &count, families.data());
    int i = 0;
    for (const auto &queue_family : families) {
      if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
        indices.graphics_family = i;
      }
      i++;
    }
    return indices;
  }

  bool check_validation_layer_support() {
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
    std::vector<VkLayerProperties> available_layers(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());
    for (const char *layer_name : validation_layers) {
      bool layer_found = false;
      for (const auto &layer_properties : available_layers) {
        if (strcmp(layer_name, layer_properties.layerName)) {
          layer_found = true;
          break;
        }
      }
      if (!layer_found) {
        return false;
      }
    }
    return true;
  }

  void main_loop() {
    while (!glfwWindowShouldClose(window)) {
      glfwPollEvents();
    }
  }

  void cleanup() {
    vkDestroyInstance(instance, nullptr);
    glfwDestroyWindow(window);
    glfwTerminate();
  }

  GLFWwindow *window;
  VkInstance instance;
};

int main() {
  HelloTriangleApplication app;
  try {
    app.run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
