// EntryPoint for our Vulkan project.
#include <iostream>
#include <vector>

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
//#include "vulkan/vulkan.h"

// ErrorHandling. Linux specific.
#define ASSERT_VULKAN(val)\
        if(val != VK_SUCCESS){\
            std::cout << val << std::endl;\
            __builtin_trap();\
}

VkDevice device;
VkSurfaceKHR surface;
VkInstance instance;
GLFWwindow *window;

// Print useful information about graphic card.
void printStatsOfDevice(VkPhysicalDevice &device) {
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(device, &properties);

    std::cout << "Vulkan Device Name:           " << properties.deviceName << std::endl;
    uint32_t apiVer = properties.apiVersion;
    std::cout << "Supported Vulkan API Version: "
              << VK_VERSION_MAJOR(apiVer) << "."
              << VK_VERSION_MINOR(apiVer) << "."
              << VK_VERSION_PATCH(apiVer) << std::endl;
    std::cout << "Vendor ID:                    " << properties.vendorID << std::endl;
    std::cout << "Device ID:                    " << properties.deviceID << std::endl;
    std::cout << "Device Type:                  " << properties.deviceType << std::endl;
    std::cout << "discreteQueuePriorities:      " << properties.limits.discreteQueuePriorities
              << std::endl;
    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(device, &features);
    std::cout << "GeometryShader:               " << features.geometryShader << std::endl;

    uint32_t amountOfQueueFamilies = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &amountOfQueueFamilies, nullptr);
    auto *familyProperties = new VkQueueFamilyProperties[amountOfQueueFamilies];
    vkGetPhysicalDeviceQueueFamilyProperties(device, &amountOfQueueFamilies, familyProperties);

    std::cout << std::endl;
    std::cout << "Amount of Queue Families: " << amountOfQueueFamilies << std::endl;

    for (int i = 0; i < amountOfQueueFamilies; i++) {
        std::cout << std::endl;
        std::cout << "Queue Family #" << i << std::endl;
        std::cout << "VK_QUEUE_GRAPHICS_BIT            "
                  << ((familyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) << std::endl;
        std::cout << "VK_QUEUE_COMPUTE_BIT             "
                  << ((familyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) != 0) << std::endl;
        std::cout << "VK_QUEUE_TRANSFER_BIT            "
                  << ((familyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) != 0) << std::endl;
        std::cout << "VK_QUEUE_SPARSE_BINDING_BIT      "
                  << ((familyProperties[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) != 0)
                  << std::endl;
        std::cout << "Queue Count:                     "
                  << familyProperties[i].queueCount
                  << std::endl;
        std::cout << "Timestamp Valid Bits:            "
                  << familyProperties[i].timestampValidBits
                  << std::endl;
        uint32_t width = familyProperties[i].minImageTransferGranularity.width;
        uint32_t height = familyProperties[i].minImageTransferGranularity.height;
        uint32_t depth = familyProperties[i].minImageTransferGranularity.depth;
        std::cout << "Min Image Timestamp Granularity: " << width << ", " << height << ", " << depth
                  << std::endl;
    }

    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &surfaceCapabilities);
    std::cout << std::endl;
    std::cout << "Surface capabilities: " << std::endl;
    std::cout << "minImageCount:           " << surfaceCapabilities.minImageCount << std::endl;
    std::cout << "maxImageCount:           " << surfaceCapabilities.maxImageCount << std::endl;
    std::cout << "currentExtent:           " << surfaceCapabilities.currentExtent.width << "/"
              << surfaceCapabilities.currentExtent.height << std::endl;
    std::cout << "minImageExtent:          " << surfaceCapabilities.minImageExtent.width << "/"
              << surfaceCapabilities.minImageExtent.height << std::endl;
    std::cout << "maxImageExtent:          " << surfaceCapabilities.maxImageExtent.width << "/"
              << surfaceCapabilities.maxImageExtent.height << std::endl;
    std::cout << "maxImageArrayLayers:     " << surfaceCapabilities.maxImageArrayLayers
              << std::endl;
    std::cout << "supportedTransforms:     " << surfaceCapabilities.supportedTransforms
              << std::endl;
    std::cout << "currentTransform:        " << surfaceCapabilities.currentTransform << std::endl;
    std::cout << "supportedCompositeAlpha: " << surfaceCapabilities.supportedCompositeAlpha
              << std::endl;
    std::cout << "supportedUsageFlags:     " << surfaceCapabilities.supportedUsageFlags
              << std::endl;
    std::cout << std::endl;

    delete[] familyProperties;
}

void startGLFW() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // Window is not resizable
    window = glfwCreateWindow(640, 480, "Vulkan Tut", nullptr, nullptr);
}

void startVulkan() {
    VkApplicationInfo appInfo;
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.pApplicationName = "Vulkan Tutorial";
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
    appInfo.pEngineName = "VulkanTutorialEngine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    // Layers and Extensions
    uint32_t amountOfLayers = 0;
    vkEnumerateInstanceLayerProperties(&amountOfLayers, nullptr);
    auto *layers = new VkLayerProperties[amountOfLayers];
    vkEnumerateInstanceLayerProperties(&amountOfLayers, layers);

    std::cout << "Amount of Instance Layers: " << amountOfLayers << std::endl;
    for (int i = 0; i < amountOfLayers; i++) {
        std::cout << std::endl;
        std::cout << "Name:         " << layers[i].layerName << std::endl;
        std::cout << "SpecVersion:  " << layers[i].specVersion << std::endl;
        std::cout << "Impl Version: " << layers[i].implementationVersion << std::endl;
        std::cout << "Description:  " << layers[i].description << std::endl;
    }
    std::cout << std::endl;

    // Extensions
    uint32_t amountOfExtensions = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &amountOfExtensions, nullptr);
    auto *extensions = new VkExtensionProperties[amountOfExtensions];
    vkEnumerateInstanceExtensionProperties(nullptr, &amountOfExtensions, extensions);

    std::cout << "Amount of Extensions: " << amountOfExtensions << std::endl;
    for (int i = 0; i < amountOfExtensions; i++) {
        std::cout << std::endl;
        std::cout << "Name:         " << extensions[i].extensionName << std::endl;
        std::cout << "SpecVersion:  " << extensions[i].specVersion << std::endl;
    }
    std::cout << std::endl;

    const std::vector<const char *> validationLayers = {
            "VK_LAYER_LUNARG_standard_validation"
    };

    // Get extensions form glfw to make it platform independent
    uint32_t amountOfGlfwExtensions = 0;
    auto glfwExtensions = glfwGetRequiredInstanceExtensions(&amountOfGlfwExtensions);

    VkInstanceCreateInfo instanceInfo;
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pNext = nullptr;
    instanceInfo.flags = 0;
    instanceInfo.pApplicationInfo = &appInfo;
    instanceInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    instanceInfo.ppEnabledLayerNames = validationLayers.data();
    instanceInfo.enabledExtensionCount = amountOfGlfwExtensions;
    instanceInfo.ppEnabledExtensionNames = glfwExtensions;

    VkResult result = vkCreateInstance(&instanceInfo, nullptr, &instance);
    ASSERT_VULKAN(result);

    result = glfwCreateWindowSurface(instance, window, nullptr, &surface);
    ASSERT_VULKAN(result);

    uint32_t amountOfPhysicalDevices = 0;
    result = vkEnumeratePhysicalDevices(instance, &amountOfPhysicalDevices, nullptr);
    ASSERT_VULKAN(result);

    auto *physicalDevices = new VkPhysicalDevice[amountOfPhysicalDevices];
    // Alternative with vector
    // std::vector<VkPhysicalDevice> physicalDevices;
    // physicalDevices.reserve(amountOfPhysicalDevices);
    // result = vkEnumeratePhysicalDevices(instance, &amountOfPhysicalDevices, physicalDevices.data());
    result = vkEnumeratePhysicalDevices(instance, &amountOfPhysicalDevices, physicalDevices);
    ASSERT_VULKAN(result);

    // Loop if there are more than one graphic card.
    for (int i = 0; i < amountOfPhysicalDevices; i++) {
        printStatsOfDevice(physicalDevices[i]);
    }

    float queuePrios[] = {1.0f, 1.0f, 1.0f, 1.0f};
    VkDeviceQueueCreateInfo deviceQueueCreateInfo;
    deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    deviceQueueCreateInfo.pNext = nullptr;
    deviceQueueCreateInfo.flags = 0;
    deviceQueueCreateInfo.queueFamilyIndex = 0; //TODO Choose correct family index.
    deviceQueueCreateInfo.queueCount = 1; //TODO Validity check!
    deviceQueueCreateInfo.pQueuePriorities = queuePrios;

    VkPhysicalDeviceFeatures usedFeatueres{};

    VkDeviceCreateInfo deviceCreateInfo;
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pNext = nullptr;
    deviceCreateInfo.flags = 0;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
    deviceCreateInfo.enabledLayerCount = 0;
    deviceCreateInfo.ppEnabledLayerNames = nullptr;
    deviceCreateInfo.enabledExtensionCount = 0;
    deviceCreateInfo.ppEnabledExtensionNames = nullptr;
    deviceCreateInfo.pEnabledFeatures = &usedFeatueres;

    //TODO choose better device instead of [0]
    result = vkCreateDevice(physicalDevices[0], &deviceCreateInfo, nullptr, &device);

    ASSERT_VULKAN(result)

    VkQueue queue;
    vkGetDeviceQueue(device, 0, 0, &queue);

    delete[] layers;
    delete[] extensions;
    delete[] physicalDevices; // don't need this if vector<> is used
}

void endlessLoop() {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
}

void shutdownVulkan() {
    // Cleanup
    vkDeviceWaitIdle(device);
    vkDestroyDevice(device, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
}

void shutdownGLFW() {
    // Cleanup
    glfwDestroyWindow(window);
}

int main() {
    startGLFW();
    startVulkan();
    endlessLoop();
    shutdownVulkan();
    shutdownGLFW();
    std::cout << std::endl;
    std::cout << "GoodBye" << std::endl;
    return 0;
}
