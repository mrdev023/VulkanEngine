#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <fstream>

#define ASSERT_VULKAN(val)                        \
    if (val != VK_SUCCESS)                        \
    {                                             \
        std::cerr << "Error VULKAN" << std::endl; \
        exit(EXIT_FAILURE);                       \
    }

VkInstance instance;
VkSurfaceKHR surface;
VkDevice device;
VkSwapchainKHR swapchain;
VkImageView *imageViews;
VkFramebuffer *framebuffers;
VkShaderModule shaderModuleVert;
VkShaderModule shaderModuleFrag;
VkPipelineLayout pipelineLayout;
VkRenderPass renderPass;
VkPipeline pipeline;
VkCommandPool commandPool;
VkCommandBuffer *commandBuffers;

GLFWwindow *window;

uint32_t swapchainImageCount = 0;

const uint32_t WIDTH = 400;
const uint32_t HEIGHT = 300;
const VkFormat format = VK_FORMAT_B8G8R8A8_UNORM;

void printStats(VkPhysicalDevice &device)
{
    VkPhysicalDeviceProperties deviceProps;
    vkGetPhysicalDeviceProperties(device, &deviceProps);

    uint32_t apiVer = deviceProps.apiVersion;

    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "Name:                      " << deviceProps.deviceName << std::endl;
    std::cout << "API Version:               " << VK_VERSION_MAJOR(apiVer) << "." << VK_VERSION_MINOR(apiVer) << "." << VK_VERSION_PATCH(apiVer) << std::endl;
    std::cout << "Driver Version:            " << deviceProps.driverVersion << std::endl;
    std::cout << "Vendor ID:                 " << deviceProps.vendorID << std::endl;
    std::cout << "Device ID:                 " << deviceProps.deviceID << std::endl;
    std::cout << "Device Type:               " << deviceProps.deviceType << std::endl;
    std::cout << "Discrete Queue Priorities: " << deviceProps.limits.discreteQueuePriorities << std::endl;

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
    std::cout << "Geometry Shader:    " << deviceFeatures.geometryShader << std::endl;

    std::cout << std::endl;

    VkPhysicalDeviceMemoryProperties deviceMemProps;
    vkGetPhysicalDeviceMemoryProperties(device, &deviceMemProps);
    std::cout << "Memory Type:        " << deviceMemProps.memoryTypes << std::endl;
    std::cout << "Memory Heaps:       " << deviceMemProps.memoryHeaps << std::endl;

    std::cout << std::endl;

    uint32_t familyQueueCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &familyQueueCount, nullptr);
    VkQueueFamilyProperties *familyProperties = new VkQueueFamilyProperties[familyQueueCount];
    vkGetPhysicalDeviceQueueFamilyProperties(device, &familyQueueCount, familyProperties);

    std::cout << "Queue Family Count: " << familyQueueCount << std::endl;

    for (int i = 0; i < familyQueueCount; i++)
    {
        std::cout << std::endl;
        std::cout << "Queue Family # " << i << std::endl;
        std::cout << "VK_QUEUE_GRAPHICS_BIT       " << ((familyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) << std::endl;
        std::cout << "VK_QUEUE_COMPUTE_BIT        " << ((familyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) != 0) << std::endl;
        std::cout << "VK_QUEUE_TRANSFER_BIT       " << ((familyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) != 0) << std::endl;
        std::cout << "VK_QUEUE_SPARSE_BINDING_BIT " << ((familyProperties[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) != 0) << std::endl;
        std::cout << "Queue Count:          " << familyProperties[i].queueCount << std::endl;
        std::cout << "Timestamp Valid Bits: " << familyProperties[i].timestampValidBits << std::endl;
        uint32_t width = familyProperties[i].minImageTransferGranularity.width;
        uint32_t height = familyProperties[i].minImageTransferGranularity.height;
        uint32_t depth = familyProperties[i].minImageTransferGranularity.depth;
        std::cout << "Min Image Timestamp Granularity : " << width << ", " << height << ", " << depth << std::endl;
    }

    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &surfaceCapabilities);
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "Surface capabilities: " << std::endl;
    std::cout << "      minImageCount: " << surfaceCapabilities.minImageCount << std::endl;
    std::cout << "      maxImageCount: " << surfaceCapabilities.maxImageCount << std::endl;
    std::cout << "      currentExtent: " << surfaceCapabilities.currentExtent.width << "/" << surfaceCapabilities.currentExtent.height << std::endl;
    std::cout << "      minImageExtent: " << surfaceCapabilities.currentExtent.width << "/" << surfaceCapabilities.currentExtent.height << std::endl;
    std::cout << "      maxImageExtent: " << surfaceCapabilities.currentExtent.width << "/" << surfaceCapabilities.currentExtent.height << std::endl;
    std::cout << "      maxImageArrayLayers: " << surfaceCapabilities.maxImageArrayLayers << std::endl;
    std::cout << "      supportedTransforms: " << surfaceCapabilities.supportedTransforms << std::endl;
    std::cout << "      currentTransform: " << surfaceCapabilities.currentTransform << std::endl;
    std::cout << "      supportedCompositeA: " << surfaceCapabilities.supportedCompositeAlpha << std::endl;
    std::cout << "      supportedUsageFlags: " << surfaceCapabilities.supportedUsageFlags << std::endl;

    uint32_t formateCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formateCount, nullptr);
    VkSurfaceFormatKHR *surfaceFormats = new VkSurfaceFormatKHR[formateCount];
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formateCount, surfaceFormats);

    std::cout << std::endl;
    std::cout << "Amount of Formats: " << formateCount << std::endl;

    for (int i = 0; i < formateCount; i++)
    {
        std::cout << "Format: " << surfaceFormats[i].format << std::endl;
    }

    uint32_t presentationModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentationModeCount, nullptr);
    VkPresentModeKHR *presentModes = new VkPresentModeKHR[presentationModeCount];
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentationModeCount, presentModes);

    std::cout << std::endl;
    std::cout << "Amount of Presentation Modes: " << presentationModeCount << std::endl;

    for (int i = 0; i < presentationModeCount; i++)
    {
        std::cout << "Supported presentation mod: " << presentModes[i] << std::endl;
    }

    std::cout << std::endl;
    delete[] familyProperties;
    delete[] surfaceFormats;
    delete[] presentModes;
}

std::vector<char> readFile(const std::string &fileName)
{
    std::ifstream file(fileName, std::ios::binary | std::ios::ate);

    if (file)
    {
        size_t fileSize = (size_t)file.tellg();
        std::vector<char> fileBuffer(fileSize);
        file.seekg(0);
        file.read(fileBuffer.data(), fileSize);
        file.close();
        return fileBuffer;
    }
    else
    {
        throw std::runtime_error("Failed to open file!");
    }
}

void initWindow()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
}

void createShaderModule(const std::vector<char> &code, VkShaderModule *shaderModule)
{
    VkShaderModuleCreateInfo shaderCreateInfo;
    shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderCreateInfo.pNext = nullptr;
    shaderCreateInfo.flags = 0;
    shaderCreateInfo.codeSize = code.size();
    shaderCreateInfo.pCode = (uint32_t *)code.data();

    VkResult result = vkCreateShaderModule(device, &shaderCreateInfo, nullptr, shaderModule);
    ASSERT_VULKAN(result);
}

void initVulkan()
{
    VkApplicationInfo appInfo;
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.pApplicationName = "Vulkan";
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
    appInfo.pEngineName = "Vulkan Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    VkLayerProperties *layers = new VkLayerProperties[layerCount];
    vkEnumerateInstanceLayerProperties(&layerCount, layers);

    std::cout << std::endl;

    std::cout << "Amount of Instance layers: " << layerCount << std::endl;

    for (int i = 0; i < layerCount; i++)
    {
        std::cout << std::endl;
        std::cout << "Name:         " << layers[i].layerName << std::endl;
        std::cout << "Spec Version: " << layers[i].specVersion << std::endl;
        std::cout << "Impl Version: " << layers[i].implementationVersion << std::endl;
        std::cout << "Description:  " << layers[i].description << std::endl;
    }

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    VkExtensionProperties *extensions = new VkExtensionProperties[extensionCount];
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions);

    std::cout << std::endl;
    std::cout << "Amount of Extensions: " << extensionCount << std::endl;

    for (int i = 0; i < extensionCount; i++)
    {
        std::cout << std::endl;
        std::cout << "Name:         " << extensions[i].extensionName << std::endl;
        std::cout << "Spec Version: " << extensions[i].specVersion << std::endl;
    }

    const std::vector<const char *> validationLayers = {
        "VK_LAYER_LUNARG_standard_validation"};

    uint32_t glfwExtensionCount = 0;
    auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    VkInstanceCreateInfo instanceInfo;
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pNext = nullptr;
    instanceInfo.flags = 0;
    instanceInfo.pApplicationInfo = &appInfo;
    instanceInfo.enabledLayerCount = validationLayers.size();
    instanceInfo.ppEnabledLayerNames = validationLayers.data();
    instanceInfo.enabledExtensionCount = glfwExtensionCount;
    instanceInfo.ppEnabledExtensionNames = glfwExtensions;

    VkResult result = vkCreateInstance(&instanceInfo, nullptr, &instance);
    ASSERT_VULKAN(result);

    result = glfwCreateWindowSurface(instance, window, nullptr, &surface);
    ASSERT_VULKAN(result)

    uint32_t deviceCount = 0;
    result = vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    ASSERT_VULKAN(result);

    std::vector<VkPhysicalDevice> physicalDevices;
    physicalDevices.resize(deviceCount);

    result = vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data());
    ASSERT_VULKAN(result);

    for (int i = 0; i < deviceCount; i++)
    {
        printStats(physicalDevices[i]);
    }

    float queuPrios[] = {1.0f, 1.0f, 1.0f, 1.0f};

    VkDeviceQueueCreateInfo deviceQueueCreateInfo;
    deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    deviceQueueCreateInfo.pNext = nullptr;
    deviceQueueCreateInfo.flags = 0;
    deviceQueueCreateInfo.queueFamilyIndex = 0; //TODO: Choose correct family index
    deviceQueueCreateInfo.queueCount = 1;       //TODO:
    deviceQueueCreateInfo.pQueuePriorities = queuPrios;

    VkPhysicalDeviceFeatures usedFeatures = {};

    const std::vector<const char *> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    VkDeviceCreateInfo deviceCreateInfo;
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pNext = nullptr;
    deviceCreateInfo.flags = 0;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
    deviceCreateInfo.enabledLayerCount = 0;
    deviceCreateInfo.ppEnabledLayerNames = nullptr;
    deviceCreateInfo.enabledExtensionCount = deviceExtensions.size();
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
    deviceCreateInfo.pEnabledFeatures = &usedFeatures;

    //TODO: Pick best device, not first device.
    result = vkCreateDevice(physicalDevices[0], &deviceCreateInfo, nullptr, &device);
    ASSERT_VULKAN(result);

    VkQueue queue;
    vkGetDeviceQueue(device, 0, 0, &queue);

    VkBool32 surfaceSupport = false;
    result = vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevices[0], 0, surface, &surfaceSupport);
    ASSERT_VULKAN(result);

    if (!surfaceSupport)
    {
        std::cerr << "Surface not supported!" << std::endl;
        exit(EXIT_FAILURE);
    }

    VkSwapchainCreateInfoKHR swapChainCreateInfo;
    swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainCreateInfo.pNext = nullptr;
    swapChainCreateInfo.flags = 0;
    swapChainCreateInfo.surface = surface;
    swapChainCreateInfo.minImageCount = 3;                                   //TODO: civ
    swapChainCreateInfo.imageFormat = format;                                //TODO: civ
    swapChainCreateInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR; //TODO: civ
    swapChainCreateInfo.imageExtent = VkExtent2D{WIDTH, HEIGHT};
    swapChainCreateInfo.imageArrayLayers = 1;
    swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapChainCreateInfo.queueFamilyIndexCount = 0;
    swapChainCreateInfo.pQueueFamilyIndices = nullptr;
    swapChainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapChainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    swapChainCreateInfo.clipped = VK_TRUE;
    swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    result = vkCreateSwapchainKHR(device, &swapChainCreateInfo, nullptr, &swapchain);
    ASSERT_VULKAN(result);

    vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, nullptr);
    VkImage *swapchainImages = new VkImage[swapchainImageCount];
    result = vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, swapchainImages);
    ASSERT_VULKAN(result);

    imageViews = new VkImageView[swapchainImageCount];

    for (int i = 0; i < swapchainImageCount; i++)
    {
        VkImageViewCreateInfo imageViewCreateInfo;
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.pNext = nullptr;
        imageViewCreateInfo.flags = 0;
        imageViewCreateInfo.image = swapchainImages[i];
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = format;
        imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;

        result = vkCreateImageView(device, &imageViewCreateInfo, nullptr, &imageViews[i]);
        ASSERT_VULKAN(result);
    }

    auto shaderCodeVert = readFile("vert.spv");
    auto shaderCodeFrag = readFile("frag.spv");

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

    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;
    vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputStateCreateInfo.pNext = nullptr;
    vertexInputStateCreateInfo.flags = 0;
    vertexInputStateCreateInfo.vertexBindingDescriptionCount = 0;
    vertexInputStateCreateInfo.pVertexBindingDescriptions = nullptr;
    vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 0;
    vertexInputStateCreateInfo.pVertexAttributeDescriptions = nullptr;

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo;
    inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyCreateInfo.pNext = nullptr;
    inputAssemblyCreateInfo.flags = 0;
    inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

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

    VkPipelineRasterizationStateCreateInfo rasterizationCreateInfo;
    rasterizationCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationCreateInfo.pNext = nullptr;
    rasterizationCreateInfo.flags = 0;
    rasterizationCreateInfo.depthClampEnable = VK_FALSE;
    rasterizationCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizationCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizationCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizationCreateInfo.depthBiasEnable = VK_FALSE;
    rasterizationCreateInfo.depthBiasConstantFactor = 0.0f;
    rasterizationCreateInfo.depthBiasClamp = 0.0f;
    rasterizationCreateInfo.depthBiasSlopeFactor = 0.0f;
    rasterizationCreateInfo.lineWidth = 1.0f;

    VkPipelineMultisampleStateCreateInfo multisampleCreateInfo;
    multisampleCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleCreateInfo.pNext = nullptr;
    multisampleCreateInfo.flags = 0;
    multisampleCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampleCreateInfo.sampleShadingEnable = VK_FALSE;
    multisampleCreateInfo.minSampleShading = 1.0f;
    multisampleCreateInfo.pSampleMask = nullptr;
    multisampleCreateInfo.alphaToCoverageEnable = VK_FALSE;
    multisampleCreateInfo.alphaToOneEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState colorBlendAttachmentState;
    colorBlendAttachmentState.blendEnable = VK_TRUE;
    colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo;
    colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendStateCreateInfo.pNext = nullptr;
    colorBlendStateCreateInfo.flags = 0;
    colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
    colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_NO_OP;
    colorBlendStateCreateInfo.attachmentCount = 1;
    colorBlendStateCreateInfo.pAttachments = &colorBlendAttachmentState;
    colorBlendStateCreateInfo.blendConstants[0] = 0.0f;
    colorBlendStateCreateInfo.blendConstants[1] = 0.0f;
    colorBlendStateCreateInfo.blendConstants[2] = 0.0f;
    colorBlendStateCreateInfo.blendConstants[3] = 0.0f;

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo;
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pNext = nullptr;
    pipelineLayoutCreateInfo.flags = 0;
    pipelineLayoutCreateInfo.setLayoutCount = 0;
    pipelineLayoutCreateInfo.pSetLayouts = nullptr;
    pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

    result = vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout);
    ASSERT_VULKAN(result);

    VkAttachmentDescription attachmentDescription;
    attachmentDescription.flags = 0;
    attachmentDescription.format = format;
    attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
    attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
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

    VkRenderPassCreateInfo renderPassCreateInfo;
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.pNext = nullptr;
    renderPassCreateInfo.flags = 0;
    renderPassCreateInfo.attachmentCount = 1;
    renderPassCreateInfo.pAttachments = &attachmentDescription;
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpassDescription;
    renderPassCreateInfo.dependencyCount = 0;
    renderPassCreateInfo.pDependencies = nullptr;

    result = vkCreateRenderPass(device, &renderPassCreateInfo, nullptr, &renderPass);
    ASSERT_VULKAN(result);

    VkGraphicsPipelineCreateInfo pipelineCreateInfo;
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.pNext = nullptr;
    pipelineCreateInfo.flags = 0;
    pipelineCreateInfo.stageCount = 2;
    pipelineCreateInfo.pStages = shaderStages;
    pipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
    pipelineCreateInfo.pTessellationState = nullptr;
    pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
    pipelineCreateInfo.pRasterizationState = &rasterizationCreateInfo;
    pipelineCreateInfo.pMultisampleState = &multisampleCreateInfo;
    pipelineCreateInfo.pDepthStencilState = nullptr;
    pipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
    pipelineCreateInfo.pDynamicState = nullptr;
    pipelineCreateInfo.layout = pipelineLayout;
    pipelineCreateInfo.renderPass = renderPass;
    pipelineCreateInfo.subpass = 0;
    pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineCreateInfo.basePipelineIndex = 0;

    result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &pipeline);
    ASSERT_VULKAN(result);

    framebuffers = new VkFramebuffer[swapchainImageCount];
    for (int i = 0; i < swapchainImageCount; i++)
    {
        VkFramebufferCreateInfo frameBufferCreateInfo;
        frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        frameBufferCreateInfo.pNext = nullptr;
        frameBufferCreateInfo.flags = 0;
        frameBufferCreateInfo.renderPass = renderPass;
        frameBufferCreateInfo.attachmentCount = 1;
        frameBufferCreateInfo.pAttachments = &(imageViews[i]);
        frameBufferCreateInfo.width = WIDTH;
        frameBufferCreateInfo.height = HEIGHT;
        frameBufferCreateInfo.layers = 1;

        result = vkCreateFramebuffer(device, &frameBufferCreateInfo, nullptr, &(framebuffers[i]));
        ASSERT_VULKAN(result);
    }

    VkCommandPoolCreateInfo commandPoolCreateInfo;
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.pNext = nullptr;
    commandPoolCreateInfo.flags = 0;
    commandPoolCreateInfo.queueFamilyIndex = 0;

    result = vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool);
    ASSERT_VULKAN(result);

    VkCommandBufferAllocateInfo commandBufferAllocateInfo;
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.pNext = nullptr;
    commandBufferAllocateInfo.commandPool = commandPool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = swapchainImageCount;

    commandBuffers = new VkCommandBuffer[swapchainImageCount];
    result = vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, commandBuffers);
    ASSERT_VULKAN(result);

    delete[] swapchainImages;
    delete[] layers;
    delete[] extensions;
}

void gameLoop()
{
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }
}

void shutDownVulkan()
{
    vkDeviceWaitIdle(device);

    vkFreeCommandBuffers(device, commandPool, swapchainImageCount, commandBuffers);
    delete[] commandBuffers;

    vkDestroyCommandPool(device, commandPool, nullptr);

    for (int i = 0; i < swapchainImageCount; i++)
    {
        vkDestroyFramebuffer(device, framebuffers[i], nullptr);
    }
    delete[] framebuffers;

    vkDestroyPipeline(device, pipeline, nullptr);
    vkDestroyRenderPass(device, renderPass, nullptr);

    for (int i = 0; i < swapchainImageCount; i++)
    {
        vkDestroyImageView(device, imageViews[i], nullptr);
    }
    delete[] imageViews;

    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);

    vkDestroyShaderModule(device, shaderModuleVert, nullptr);
    vkDestroyShaderModule(device, shaderModuleFrag, nullptr);

    vkDestroySwapchainKHR(device, swapchain, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
}

void shutDownWindow()
{
    glfwDestroyWindow(window);

    glfwTerminate();
}

int main(int argc, char const *argv[])
{
    initWindow();
    initVulkan();
    gameLoop();
    shutDownVulkan();
    shutDownWindow();

    return 0;
}