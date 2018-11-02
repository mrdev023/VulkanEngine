#include <vulkan/vulkan.h>
#include <iostream>

#define ASSERT_VULKAN(val)                                                  \
    if (val != VK_SUCCESS)                                                  \
    {                                                                       \
        std::cerr << "Error VULKAN::INSTANCE_CREATION_FAILED" << std::endl; \
        exit(EXIT_FAILURE);                                                 \
    }

VkInstance instance;

void printStats(VkPhysicalDevice &device)
{
    VkPhysicalDeviceProperties deviceProps;
    vkGetPhysicalDeviceProperties(device, &deviceProps);

    uint32_t apiVer = deviceProps.apiVersion;

    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "Name:               " << deviceProps.deviceName << std::endl;
    std::cout << "API Version:        " << VK_VERSION_MAJOR(apiVer) << "." << VK_VERSION_MINOR(apiVer) << "." << VK_VERSION_PATCH(apiVer) << std::endl;
    std::cout << "Driver Version:     " << deviceProps.driverVersion << std::endl;
    std::cout << "Vendor ID:          " << deviceProps.vendorID << std::endl;
    std::cout << "Device ID:          " << deviceProps.deviceID << std::endl;
    std::cout << "Device Type:        " << deviceProps.deviceType << std::endl;

    std::cout << std::endl;

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
    vkGetPhysicalDeviceQueueFamilyProperties(device, &familyQueueCount, NULL);
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

    std::cout << std::endl;

    delete[] familyProperties;
}

int main(int argc, char const *argv[])
{
    VkApplicationInfo appInfo;
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan";
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
    appInfo.pEngineName = "Vulkan Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_1;

    VkInstanceCreateInfo instanceInfo;
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pNext = NULL;
    instanceInfo.flags = 0;
    instanceInfo.pApplicationInfo = &appInfo;
    instanceInfo.enabledLayerCount = 0;
    instanceInfo.ppEnabledLayerNames = NULL;
    instanceInfo.enabledExtensionCount = 0;
    instanceInfo.ppEnabledExtensionNames = NULL;

    VkResult result = vkCreateInstance(&instanceInfo, NULL, &instance);

    ASSERT_VULKAN(result);

    uint32_t deviceCount = 0;
    result = vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);
    ASSERT_VULKAN(result);

    VkPhysicalDevice *physicalDevices = new VkPhysicalDevice[deviceCount];

    result = vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices);
    ASSERT_VULKAN(result);

    for (int i = 0; i < deviceCount; i++)
    {
        printStats(physicalDevices[i]);
    }

    return 0;
}
