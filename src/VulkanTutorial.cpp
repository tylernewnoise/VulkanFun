// EntryPoint for our Vulkan project.
#include <iostream>
#include "vulkan/vulkan.h"

// ErrorHandling. Linux specific.
#define ASSERT_VULKAN(val)\
        if(val != VK_SUCCESS){\
            std::cout << val << std::endl;\
            __builtin_trap();\
}

VkInstance instance;

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
}

int main() {
    VkApplicationInfo appInfo;
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.pApplicationName = "Vulkan Tutorial";
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
    appInfo.pEngineName = "VulkanTutorialEngine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo instanceInfo;
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pNext = nullptr;
    instanceInfo.flags = 0;
    instanceInfo.pApplicationInfo = &appInfo;
    instanceInfo.enabledLayerCount = 0;
    instanceInfo.ppEnabledLayerNames = nullptr;
    instanceInfo.enabledExtensionCount = 0;
    instanceInfo.ppEnabledExtensionNames = nullptr;

    VkResult result = vkCreateInstance(&instanceInfo, nullptr, &instance);

    ASSERT_VULKAN(result);

    uint32_t amountOfPhysicalDevices = 0;
    result = vkEnumeratePhysicalDevices(instance, &amountOfPhysicalDevices, nullptr);
    ASSERT_VULKAN(result);

    auto *physicalDevices = new VkPhysicalDevice[amountOfPhysicalDevices];

    result = vkEnumeratePhysicalDevices(instance, &amountOfPhysicalDevices, physicalDevices);
    ASSERT_VULKAN(result);

    // Loop if there are more than one graphic card.
    for (int i = 0; i < amountOfPhysicalDevices; i++) {
        printStatsOfDevice(physicalDevices[i]);
    }

    return 0;
}
