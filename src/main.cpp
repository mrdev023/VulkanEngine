#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

#define ASSERT_VULKAN(val)                        \
    if (val != VK_SUCCESS)                        \
    {                                             \
        std::cerr << "Error VULKAN" << std::endl; \
        exit(EXIT_FAILURE);                       \
    }

VkInstance instance;
VkSurfaceKHR surface;
VkDevice device;
GLFWwindow *window;

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

    std::cout << std::endl;

    delete[] familyProperties;
}

void initWindow()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(400, 300, "Vulkan", nullptr, nullptr);
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
    deviceCreateInfo.pEnabledFeatures = &usedFeatures;

    //TODO: Pick best device, not first device.
    result = vkCreateDevice(physicalDevices[0], &deviceCreateInfo, nullptr, &device);
    ASSERT_VULKAN(result);

    VkQueue queue;
    vkGetDeviceQueue(device, 0, 0, &queue);
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
