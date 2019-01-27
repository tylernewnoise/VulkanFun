#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>

// ErrorHandling. Linux specific.
#define ASSERT_VULKAN(val)\
        if(val != VK_SUCCESS){\
            std::cout << val << std::endl;\
            __builtin_trap();\
}

// TODO namespaces
VkQueue queue;
VkDevice device;
std::vector<VkPhysicalDevice> physicalDevices;
VkInstance instance;
VkPipeline pipeline;
VkSurfaceKHR surface;
VkRenderPass renderPass;
VkImageView *imageViews;
VkSwapchainKHR swapchain = VK_NULL_HANDLE;
VkShaderModule shaderModuleVert;
VkShaderModule shaderModuleFrag;
VkPipelineLayout pipelineLayout;
VkFramebuffer *framebuffers;
VkCommandPool commandPool;
VkCommandBuffer *commandBuffers;
VkSemaphore semaphoreImageAvailable;
VkSemaphore semaphoreRenderingDone;
GLFWwindow *window;
uint32_t amountOfImagesInSwapChain = 0;

uint32_t WIDTH = 800;
uint32_t HEIGHT = 600;


void printLayers() {
    //Layers
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
    delete[] layers;
}

void printExtensions() {
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

    delete[] extensions;
}

void createInstance() {
    VkApplicationInfo appInfo;
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.pApplicationName = "Vulkan Tutorial";
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
    appInfo.pEngineName = "VulkanTutorialEngine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    const std::vector<const char *> validationLayers = {
            "VK_LAYER_LUNARG_standard_validation"
    };

    // Get extensions from glfw to make it platform independent.
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

    if (vkCreateInstance(&instanceInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create instance!");
    }
}

std::vector<VkPhysicalDevice> getAllPhysicalDevices() {
    uint32_t amountOfPhysicalDevices = 0;
    vkEnumeratePhysicalDevices(instance, &amountOfPhysicalDevices, nullptr);
    if (amountOfPhysicalDevices == 0) {
        throw std::runtime_error("Failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> physicalDevices;
    physicalDevices.resize(amountOfPhysicalDevices);
    vkEnumeratePhysicalDevices(instance, &amountOfPhysicalDevices, physicalDevices.data());

    return physicalDevices;
}

void createGlfwWindowSurface() {
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface!");
    }
}

// Set up device and print useful information about graphic card.
void setUpDevice(VkPhysicalDevice &device) {
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

    // fix for my tiled window manager to not start with 800x600
    WIDTH = surfaceCapabilities.maxImageExtent.width;
    HEIGHT = surfaceCapabilities.maxImageExtent.height;

    // SurfaceFormatOutput (for colors etc...)
    uint32_t amountOfFormats = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &amountOfFormats, nullptr);
    auto *surfaceFormats = new VkSurfaceFormatKHR[amountOfFormats];
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &amountOfFormats, surfaceFormats);

    std::cout << "Amount of Formats: " << amountOfFormats << std::endl;
    for (int i = 0; i < amountOfFormats; i++) {
        std::cout << "Format: " << surfaceFormats[i].format << std::endl;
    }

    // Presentation modes of graphics card
    uint32_t amountOfPresentationModes = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &amountOfPresentationModes, nullptr);
    auto *presentModes = new VkPresentModeKHR[amountOfPresentationModes];
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &amountOfPresentationModes,
                                              presentModes);

    std::cout << std::endl;
    std::cout << "Amount of Presentation Modes: " << amountOfPresentationModes << std::endl;
    for (int i = 0; i < amountOfPresentationModes; i++) {
        std::cout << "PresentationsModes: " << presentModes[i] << std::endl;
    }

    delete[] familyProperties;
    delete[] surfaceFormats;
    delete[] presentModes;
}

void setupAllPhysicalDevices() {
    for (auto &physicalDevice : physicalDevices) {
        setUpDevice(physicalDevice);
    }
}

void createLogicalDevice() {
    float queuePrios[] = {1.0f, 1.0f, 1.0f, 1.0f};
    VkDeviceQueueCreateInfo deviceQueueCreateInfo;
    deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    deviceQueueCreateInfo.pNext = nullptr;
    deviceQueueCreateInfo.flags = 0;
    deviceQueueCreateInfo.queueFamilyIndex = 0; //TODO Choose correct family index.
    deviceQueueCreateInfo.queueCount = 1; //TODO Validity check!
    deviceQueueCreateInfo.pQueuePriorities = queuePrios;

    VkPhysicalDeviceFeatures usedFeatures{};

    const std::vector<const char *> deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    VkDeviceCreateInfo deviceCreateInfo;
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pNext = nullptr;
    deviceCreateInfo.flags = 0;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
    deviceCreateInfo.enabledLayerCount = 0;
    deviceCreateInfo.ppEnabledLayerNames = nullptr;
    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
    deviceCreateInfo.pEnabledFeatures = &usedFeatures;

    //TODO choose better device instead of [0]
    if (vkCreateDevice(physicalDevices[0], &deviceCreateInfo, nullptr, &device) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create logical device!");
    }
}

void initQueue() {
    vkGetDeviceQueue(device, 0, 0, &queue);
}

void checkSurfaceSupport() {
    auto surfaceSupport = static_cast<VkBool32>(false);
    if (vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevices[0], 0, surface, &surfaceSupport) != VK_SUCCESS) {
        throw std::runtime_error("Failed to get surface!");
    }

    if (!surfaceSupport) {
        std::cerr << "Surface not supported!" << std::endl;
        __builtin_trap();
    }
}

void createSwapchain() {
    VkSwapchainCreateInfoKHR swapchainCreateInfo;
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.pNext = nullptr;
    swapchainCreateInfo.flags = 0;
    swapchainCreateInfo.surface = surface;
    swapchainCreateInfo.minImageCount = 3; // TODO check if valid
    swapchainCreateInfo.imageFormat = VK_FORMAT_B8G8R8A8_UNORM; // TODO check if valid
    swapchainCreateInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR; // TODO check if valid
    swapchainCreateInfo.imageExtent = VkExtent2D{WIDTH, HEIGHT};
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; // TODO check if valid
    swapchainCreateInfo.queueFamilyIndexCount = 0;
    swapchainCreateInfo.pQueueFamilyIndices = nullptr;
    swapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR; // TODO check for VK_PRESENT_MODE_MAILBOX_KHR
    swapchainCreateInfo.clipped = VK_TRUE;
    swapchainCreateInfo.oldSwapchain = swapchain; // TODO remember for image resizing

    if (vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapchain) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create swap chain!");
    }
}

void createImageviews() {
    vkGetSwapchainImagesKHR(device, swapchain, &amountOfImagesInSwapChain, nullptr);
    auto *swapchainImages = new VkImage[amountOfImagesInSwapChain];
    VkResult result = vkGetSwapchainImagesKHR(device, swapchain, &amountOfImagesInSwapChain, swapchainImages);
    ASSERT_VULKAN(result);

    imageViews = new VkImageView[amountOfImagesInSwapChain];
    for (int i = 0; i < amountOfImagesInSwapChain; i++) {
        VkImageViewCreateInfo imageViewCreateInfo;
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.pNext = nullptr;
        imageViewCreateInfo.flags = 0;
        imageViewCreateInfo.image = swapchainImages[i];
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = VK_FORMAT_B8G8R8A8_UNORM; // TODO check if valid
        imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0; // no mipmapping
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;
        if (vkCreateImageView(device, &imageViewCreateInfo, nullptr, &imageViews[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create image views!");
        }
    }
    delete[] swapchainImages;
}

void createRenderpass() {
    VkAttachmentDescription attachmentDescription;
    attachmentDescription.flags = 0;
    attachmentDescription.format = VK_FORMAT_B8G8R8A8_UNORM;
    attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT; // important for multisampling
    attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // no stencil buffers
    attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference attachmentReference;
    attachmentReference.attachment = 0;
    attachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpassDescription;
    subpassDescription.flags = 0;
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.inputAttachmentCount = 0;
    subpassDescription.pInputAttachments = nullptr;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &attachmentReference;
    subpassDescription.pResolveAttachments = nullptr;
    subpassDescription.pDepthStencilAttachment = nullptr;
    subpassDescription.preserveAttachmentCount = 0;
    subpassDescription.pPreserveAttachments = nullptr;

    VkSubpassDependency subpassDependency;
    subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependency.dstSubpass = 0;
    subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.srcAccessMask = 0;
    subpassDependency.dstAccessMask =
            VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    subpassDependency.dependencyFlags = 0;

    VkRenderPassCreateInfo renderPassCreateInfo;
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.pNext = nullptr;
    renderPassCreateInfo.flags = 0;
    renderPassCreateInfo.attachmentCount = 1;
    renderPassCreateInfo.pAttachments = &attachmentDescription;
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpassDescription;
    renderPassCreateInfo.dependencyCount = 0;
    renderPassCreateInfo.pDependencies = &subpassDependency;

    if (vkCreateRenderPass(device, &renderPassCreateInfo, nullptr, &renderPass) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create render pass!");
    }
}

void createShaderModule(const std::vector<char> &shaderCode, VkShaderModule *shaderModule) {
    VkShaderModuleCreateInfo shaderCreateInfo;
    shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderCreateInfo.pNext = nullptr;
    shaderCreateInfo.flags = 0;
    shaderCreateInfo.codeSize = shaderCode.size();
    shaderCreateInfo.pCode = reinterpret_cast<const uint32_t *>(shaderCode.data());

    if (vkCreateShaderModule(device, &shaderCreateInfo, nullptr, shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create shader module!");
    }
}

// For reading shader files.
std::vector<char> readFile(const std::string &filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (file) {
        size_t fileSize = static_cast<size_t>(file.tellg());
        std::vector<char> fileBuffer(fileSize);
        file.seekg(0);
        file.read(fileBuffer.data(), fileSize);
        file.close();
        return fileBuffer;
    } else {
        throw std::runtime_error("Failed to open file!");
    }
}

void createPipeline() {
    // Load Shaders.
    auto shaderCodeVert = readFile("../shaders/vert.spv");
    auto shaderCodeFrag = readFile("../shaders/frag.spv");
    createShaderModule(shaderCodeVert, &shaderModuleVert);
    createShaderModule(shaderCodeFrag, &shaderModuleFrag);

    VkPipelineShaderStageCreateInfo shaderStageCreateInfoVert;
    shaderStageCreateInfoVert.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfoVert.pNext = nullptr;
    shaderStageCreateInfoVert.flags = 0;
    shaderStageCreateInfoVert.stage = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStageCreateInfoVert.module = shaderModuleVert;
    shaderStageCreateInfoVert.pName = "main";
    shaderStageCreateInfoVert.pSpecializationInfo = nullptr;

    VkPipelineShaderStageCreateInfo shaderStageCreateInfoFrag;
    shaderStageCreateInfoFrag.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfoFrag.pNext = nullptr;
    shaderStageCreateInfoFrag.flags = 0;
    shaderStageCreateInfoFrag.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStageCreateInfoFrag.module = shaderModuleFrag;
    shaderStageCreateInfoFrag.pName = "main";
    shaderStageCreateInfoFrag.pSpecializationInfo = nullptr;

    VkPipelineShaderStageCreateInfo shaderStages[] = {shaderStageCreateInfoVert, shaderStageCreateInfoFrag};

    VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo;
    vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputCreateInfo.pNext = nullptr;
    vertexInputCreateInfo.flags = 0;
    vertexInputCreateInfo.vertexBindingDescriptionCount = 0;
    vertexInputCreateInfo.pVertexBindingDescriptions = nullptr;
    vertexInputCreateInfo.vertexAttributeDescriptionCount = 0;
    vertexInputCreateInfo.pVertexAttributeDescriptions = nullptr;

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo;
    inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyStateCreateInfo.pNext = nullptr;
    inputAssemblyStateCreateInfo.flags = 0;
    inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = WIDTH;
    viewport.height = HEIGHT;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor;
    scissor.offset = {0, 0};
    scissor.extent = {WIDTH, HEIGHT};

    VkPipelineViewportStateCreateInfo viewportStateCreateInfo;
    viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateCreateInfo.pNext = nullptr;
    viewportStateCreateInfo.flags = 0;
    viewportStateCreateInfo.viewportCount = 1;
    viewportStateCreateInfo.pViewports = &viewport;
    viewportStateCreateInfo.scissorCount = 1;
    viewportStateCreateInfo.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo;
    rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationStateCreateInfo.pNext = nullptr;
    rasterizationStateCreateInfo.flags = 0;
    rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
    rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
    rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
    rasterizationStateCreateInfo.depthBiasClamp = 0.0f;
    rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;
    rasterizationStateCreateInfo.lineWidth = 1.0f; //Thickness of lines

    VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo;
    multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleStateCreateInfo.pNext = nullptr;
    multisampleStateCreateInfo.flags = 0;
    multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE; //Disable Multisampling
    multisampleStateCreateInfo.minSampleShading = 1.0f;
    multisampleStateCreateInfo.pSampleMask = nullptr;
    multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
    multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT;

    VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo;
    colorBlendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendCreateInfo.pNext = nullptr;
    colorBlendCreateInfo.flags = 0;
    colorBlendCreateInfo.logicOpEnable = VK_FALSE;
    colorBlendCreateInfo.logicOp = VK_LOGIC_OP_NO_OP;
    colorBlendCreateInfo.attachmentCount = 1;
    colorBlendCreateInfo.pAttachments = &colorBlendAttachment;
    colorBlendCreateInfo.blendConstants[0] = 0.0f;
    colorBlendCreateInfo.blendConstants[1] = 0.0f;
    colorBlendCreateInfo.blendConstants[2] = 0.0f;
    colorBlendCreateInfo.blendConstants[3] = 0.0f;

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo;
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pNext = nullptr;
    pipelineLayoutCreateInfo.flags = 0;
    pipelineLayoutCreateInfo.setLayoutCount = 0;
    pipelineLayoutCreateInfo.pSetLayouts = nullptr;
    pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

    if (vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create pipeline layout!");
    }

    VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo;
    graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphicsPipelineCreateInfo.pNext = nullptr;
    graphicsPipelineCreateInfo.flags = 0;
    graphicsPipelineCreateInfo.stageCount = 2;
    graphicsPipelineCreateInfo.pStages = shaderStages;
    graphicsPipelineCreateInfo.pVertexInputState = &vertexInputCreateInfo;
    graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
    graphicsPipelineCreateInfo.pTessellationState = nullptr;
    graphicsPipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
    graphicsPipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
    graphicsPipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
    graphicsPipelineCreateInfo.pDepthStencilState = nullptr;
    graphicsPipelineCreateInfo.pColorBlendState = &colorBlendCreateInfo;
    graphicsPipelineCreateInfo.pDynamicState = nullptr;
    graphicsPipelineCreateInfo.layout = pipelineLayout; // handle
    graphicsPipelineCreateInfo.renderPass = renderPass;
    graphicsPipelineCreateInfo.subpass = 0;
    graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    graphicsPipelineCreateInfo.basePipelineIndex = -1;

    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &pipeline) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to create graphics pipeline!");
    }
}

void createFrameBuffers() {
    framebuffers = new VkFramebuffer[amountOfImagesInSwapChain];
    for (size_t i = 0; i < amountOfImagesInSwapChain; i++) {
        VkFramebufferCreateInfo framebufferCreateInfo;
        framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferCreateInfo.pNext = nullptr;
        framebufferCreateInfo.flags = 0;
        framebufferCreateInfo.renderPass = renderPass;
        framebufferCreateInfo.attachmentCount = 1;
        framebufferCreateInfo.pAttachments = &(imageViews[i]);
        framebufferCreateInfo.width = WIDTH;
        framebufferCreateInfo.height = HEIGHT;
        framebufferCreateInfo.layers = 1;

        if (vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &(framebuffers[i])) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create framebuffer!");
        }
    }
}

void createCommandPool() {
    VkCommandPoolCreateInfo commandPoolCreateInfo;
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.pNext = nullptr;
    commandPoolCreateInfo.flags = 0;
    commandPoolCreateInfo.queueFamilyIndex = 0; // TODO check if valid, get correct queue with VK_QUEUE_GRAPHICS_BIT

    if (vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create command pool!");
    }
}

void createCommandBuffers() {
    VkCommandBufferAllocateInfo commandBufferAllocateInfo;
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.pNext = nullptr;
    commandBufferAllocateInfo.commandPool = commandPool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = amountOfImagesInSwapChain;

    commandBuffers = new VkCommandBuffer[amountOfImagesInSwapChain];
    if (vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, commandBuffers) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate command buffers!");
    }
}

void recordCommandBuffers() {
    VkCommandBufferBeginInfo commandBufferBeginInfo;
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBufferBeginInfo.pNext = nullptr;
    commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    commandBufferBeginInfo.pInheritanceInfo = nullptr;

    for (size_t i = 0; i < amountOfImagesInSwapChain; i++) {
        if (vkBeginCommandBuffer(commandBuffers[i], &commandBufferBeginInfo) != VK_SUCCESS) {
            throw std::runtime_error("Failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassBeginInfo;
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.pNext = nullptr;
        renderPassBeginInfo.renderPass = renderPass;
        renderPassBeginInfo.framebuffer = framebuffers[i];
        renderPassBeginInfo.renderArea.offset = {0, 0};
        renderPassBeginInfo.renderArea.extent = {WIDTH, HEIGHT};
        VkClearValue clearValue = {0.0f, 0.0f, 0.0f, 1.0f};
        renderPassBeginInfo.clearValueCount = 1;
        renderPassBeginInfo.pClearValues = &clearValue;

        vkCmdBeginRenderPass(commandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

        vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

        vkCmdEndRenderPass(commandBuffers[i]);

        if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to record command buffer!");
        }
    }
}

void createSemaphores() {
    VkSemaphoreCreateInfo semaphoreCreateInfo;
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreCreateInfo.pNext = nullptr;
    semaphoreCreateInfo.flags = 0;

    if (vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &semaphoreImageAvailable) != VK_SUCCESS ||
        vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &semaphoreRenderingDone) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create synchronization objects for a frame!");
    }
}

void recreateSwapchain() {
    vkDeviceWaitIdle(device);
    vkFreeCommandBuffers(device, commandPool, amountOfImagesInSwapChain, commandBuffers);
    delete[] commandBuffers;
    vkDestroyCommandPool(device, commandPool, nullptr);
    for (size_t i = 0; i < amountOfImagesInSwapChain; i++) {
        vkDestroyFramebuffer(device, framebuffers[i], nullptr);
    }
    delete[] framebuffers;
    vkDestroyPipeline(device, pipeline, nullptr);
    vkDestroyRenderPass(device, renderPass, nullptr);
    for (int i = 0; i < amountOfImagesInSwapChain; i++) {
        vkDestroyImageView(device, imageViews[i], nullptr);
    }
    delete[] imageViews;
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    vkDestroyShaderModule(device, shaderModuleFrag, nullptr);
    vkDestroyShaderModule(device, shaderModuleVert, nullptr);
    // vkDestroySwapchainKHR(device, swapchain, nullptr);

    VkSwapchainKHR oldSwapchain = swapchain;

    createSwapchain();
    createImageviews();
    createRenderpass();
    createPipeline();
    createFrameBuffers();
    createCommandPool();
    createCommandBuffers();
    recordCommandBuffers();
    vkDestroySwapchainKHR(device, oldSwapchain, nullptr);
}

void onWindowResize(GLFWwindow *window, int width, int height) {
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevices[0], surface, &surfaceCapabilities);

    if (width > surfaceCapabilities.maxImageExtent.width) width = surfaceCapabilities.maxImageExtent.width;
    if (height > surfaceCapabilities.maxImageExtent.height) height = surfaceCapabilities.maxImageExtent.height;

    if (width == 0 || height == 0) return;

    WIDTH = static_cast<uint32_t>(width);
    HEIGHT = static_cast<uint32_t>(height);
    recreateSwapchain();
}

void startGLFW() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // comment this for resizable window
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Tutorial", nullptr, nullptr);
    // uncomment this for resizable window
    //glfwSetWindowSizeCallback(window, onWindowResize);
}

void startVulkan() {
    createInstance();
    physicalDevices = getAllPhysicalDevices();
    createGlfwWindowSurface();
    setupAllPhysicalDevices();
    createLogicalDevice();
    initQueue();
    checkSurfaceSupport();
    createSwapchain();
    createImageviews();
    createRenderpass();
    createPipeline();
    createFrameBuffers();
    createCommandPool();
    createCommandBuffers();
    recordCommandBuffers();
    createSemaphores();
}

void drawFrame() {
    uint32_t imageIndex;
    vkAcquireNextImageKHR(device, swapchain, std::numeric_limits<uint64_t>::max(),
                          semaphoreImageAvailable, VK_NULL_HANDLE, &imageIndex);

    VkSubmitInfo submitInfo;
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = nullptr;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &semaphoreImageAvailable;
    VkPipelineStageFlags waitStageMask[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.pWaitDstStageMask = waitStageMask;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &(commandBuffers[imageIndex]);
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &semaphoreRenderingDone;

    if (vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE)) {
        throw std::runtime_error("Failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfoKHR;
    presentInfoKHR.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfoKHR.pNext = nullptr;
    presentInfoKHR.waitSemaphoreCount = 1;
    presentInfoKHR.pWaitSemaphores = &semaphoreRenderingDone;
    presentInfoKHR.swapchainCount = 1;
    presentInfoKHR.pSwapchains = &swapchain;
    presentInfoKHR.pImageIndices = &imageIndex;
    presentInfoKHR.pResults = nullptr;

    VkResult result = vkQueuePresentKHR(queue, &presentInfoKHR);
    ASSERT_VULKAN(result);
}

void renderLoop() {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        drawFrame();
    }
}

void shutdownVulkan() {
    // Cleanup
    vkDeviceWaitIdle(device);
    vkDestroySemaphore(device, semaphoreImageAvailable, nullptr);
    vkDestroySemaphore(device, semaphoreRenderingDone, nullptr);
    vkFreeCommandBuffers(device, commandPool, amountOfImagesInSwapChain, commandBuffers);
    delete[] commandBuffers;
    vkDestroyCommandPool(device, commandPool, nullptr);
    for (size_t i = 0; i < amountOfImagesInSwapChain; i++) {
        vkDestroyFramebuffer(device, framebuffers[i], nullptr);
    }
    delete[] framebuffers;
    vkDestroyPipeline(device, pipeline, nullptr);
    vkDestroyRenderPass(device, renderPass, nullptr);
    for (int i = 0; i < amountOfImagesInSwapChain; i++) {
        vkDestroyImageView(device, imageViews[i], nullptr);
    }
    delete[] imageViews;
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    vkDestroyShaderModule(device, shaderModuleFrag, nullptr);
    vkDestroyShaderModule(device, shaderModuleVert, nullptr);
    vkDestroySwapchainKHR(device, swapchain, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
}

void shutdownGLFW() {
    // Cleanup
    glfwDestroyWindow(window);
    glfwTerminate();
}

int main(int argc, char *argv[]) {
    std::cout << std::endl;
    std::cout << "Vulkan Tutorial" << std::endl;
    std::cout << "\tExecute with -h for more options." << std::endl;
    std::cout << std::endl;

    // TODO make this better or with more options :P
    if (argc >= 2) {
        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "-h") == 0) {
                std::cout << "Usage: VulkanTut [-h] [-i] [-l]" << std::endl;
                std::cout << "\t-h (help):          print this." << std::endl;
                std::cout << "\t-l (layers):        print available validation layers and exit." << std::endl;
                std::cout << "\t-e (extensions):    print available extensions and exit." << std::endl;
                std::cout << "If no parameter is specified the program will hopefully show some fancy graphics."
                          << std::endl;
            } else if (strcmp(argv[i], "-l") == 0) {
                printLayers();
            } else if (strcmp(argv[i], "-e") == 0) {
                printExtensions();
            } else {
                std::cout << "Use -h for more info!" << std::endl;
            }
        }
        return 0;
    } else {
        startGLFW();
        startVulkan();
        renderLoop();
        shutdownVulkan();
        shutdownGLFW();
    }

    std::cout << std::endl;
    std::cout << "GoodBye" << std::endl;
    return 0;
}
