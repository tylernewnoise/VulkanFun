#include <iostream>
// EntryPoint
#include "vulkan/vulkan.h"

// ErrorHandling. Linux specific.
#define ASSERT_VULKAN(val)\
        if(val != VK_SUCCESS){\
            std::cout<<val<<std::endl;\
            __builtin_trap();\
}

VkInstance instance;

int main() {
    VkApplicationInfo appInfo;
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.pApplicationName = "Vulkan Tutorial";
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
    appInfo.pEngineName = "VulkanTutEngine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo instanceInfo;
    instanceInfo.sType= VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pNext = nullptr;
    instanceInfo.flags = 0;
    instanceInfo.pApplicationInfo = &appInfo;
    instanceInfo.enabledLayerCount = 0;
    instanceInfo.ppEnabledLayerNames = nullptr;
    instanceInfo.enabledExtensionCount = 0;
    instanceInfo.ppEnabledExtensionNames = nullptr;

    VkResult result = vkCreateInstance(&instanceInfo, nullptr, &instance);

    ASSERT_VULKAN(result);

    return 0;
}
