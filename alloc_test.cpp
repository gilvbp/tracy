#include <vulkan/vulkan.h>
#include "/usr/include/Tracy/tracy/Tracy.hpp"
#include <iostream>
#include <vector>


int main() {
  ZoneScoped; // Tracy

  // 1. Create Vulkan instance
  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "Tracy Vulkan Alloc Test";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "No Engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_2;

  VkInstanceCreateInfo instInfo{};
  instInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instInfo.pApplicationInfo = &appInfo;

  VkInstance instance;
  if (vkCreateInstance(&instInfo, nullptr, &instance) != VK_SUCCESS) {
    std::cerr << "Failed to create Vulkan instance" << std::endl;
    return -1;
  }

  // 2. Select physical device
  uint32_t gpuCount = 0;
  vkEnumeratePhysicalDevices(instance, &gpuCount, nullptr);
  if (gpuCount == 0) {
    std::cerr << "No Vulkan GPUs found" << std::endl;
    return -1;
  }
  std::vector<VkPhysicalDevice> gpus(gpuCount);
  vkEnumeratePhysicalDevices(instance, &gpuCount, gpus.data());
  VkPhysicalDevice physical = gpus[0]; // pick first

  // 3. Find graphics queue family
  uint32_t queueCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(physical, &queueCount, nullptr);
  std::vector<VkQueueFamilyProperties> queueProps(queueCount);
  vkGetPhysicalDeviceQueueFamilyProperties(physical, &queueCount, queueProps.data());

  int graphicsIndex = -1;
  for (int i = 0; i < (int)queueCount; i++) {
    if (queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      graphicsIndex = i;
      break;
    }
  }
  if (graphicsIndex < 0) {
    std::cerr << "No graphics queue family found" << std::endl;
    return -1;
  }

  float queuePriority = 1.0f;
  VkDeviceQueueCreateInfo queueInfo{};
  queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queueInfo.queueFamilyIndex = graphicsIndex;
  queueInfo.queueCount = 1;
  queueInfo.pQueuePriorities = &queuePriority;

  // 4. Create logical device
  VkDeviceCreateInfo devInfo{};
  devInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  devInfo.queueCreateInfoCount = 1;
  devInfo.pQueueCreateInfos = &queueInfo;
  devInfo.enabledExtensionCount = 0;

  VkDevice device;
  if (vkCreateDevice(physical, &devInfo, nullptr, &device) != VK_SUCCESS) {
    std::cerr << "Failed to create Vulkan device" << std::endl;
    return -1;
  }

  // 5. Retrieve the graphics queue
  VkQueue queue;
  vkGetDeviceQueue(device, graphicsIndex, 0, &queue);

  // Now 'device' and 'queue' are initialized and can be used for vkAllocateMemory etc.
  std::cout << "Vulkan device and queue initialized successfully." << std::endl;
  {
    // Pre‑query memory properties
    VkPhysicalDeviceMemoryProperties memProps;
    vkGetPhysicalDeviceMemoryProperties(physical, &memProps);

    // Find a memory type index that's device local
    uint32_t typeIndex = UINT32_MAX;
    for (uint32_t i = 0; i < memProps.memoryTypeCount; i++) {
      if ((memProps.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) &&
          (memProps.memoryHeaps[memProps.memoryTypes[i].heapIndex].size >= (1ull << 30))) {
        typeIndex = i;
        break;
      }
    }
    if (typeIndex == UINT32_MAX) {
      std::cerr << "No suitable memory type found" << std::endl;
      return -1;
    }

    // Allocate and free in a loop
    for (int i = 0; i < 50; ++i) {
      {
        ZoneScopedN("vkAllocateMemory");
        VkMemoryAllocateInfo ai{};
        ai.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        ai.allocationSize = 1ull << 30;
        ai.memoryTypeIndex = typeIndex;
        VkDeviceMemory mem;
        vkAllocateMemory(device, &ai, nullptr, &mem);
      }
      {
        ZoneScopedN("vkFreeMemory");
        // Note: VMA or direct free
        VkDeviceMemory memToFree = VK_NULL_HANDLE;
        // Actually use the same mem variable if you store it, or allocate+free inside the block
      }
    }
}
    // Cleanup
  vkDestroyDevice(device, nullptr);
  vkDestroyInstance(instance, nullptr);
  return 0;
}
