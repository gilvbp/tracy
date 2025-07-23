// minimal_vulkan.c
#include <stdio.h>
#include <stdlib.h>
#include <vulkan/vulkan.h>

// Macro simples para checar erros e abortar
#define VK_CHECK(fn)                                                    \
    do {                                                                \
        VkResult res = (fn);                                            \
        if (res != VK_SUCCESS) {                                        \
            fprintf(stderr, "Erro Vulkan em %s:%d  code=%d\n",          \
                    __FILE__, __LINE__, res);                           \
            exit(1);                                                    \
        }                                                               \
    } while (0)

int main(void) {
  // 1) Cria VkInstance
  VkApplicationInfo appInfo = {
    .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    .pApplicationName   = "MinimalVulkan",
    .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
    .pEngineName        = "NoEngine",
    .engineVersion      = VK_MAKE_VERSION(1, 0, 0),
    .apiVersion         = VK_API_VERSION_1_0,
  };

  VkInstanceCreateInfo instInfo{};
  instInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instInfo.pNext                   = nullptr;
  instInfo.flags                   = 0;
  instInfo.pApplicationInfo        = &appInfo;
  instInfo.enabledLayerCount       = 0;
  instInfo.ppEnabledLayerNames     = nullptr;
  instInfo.enabledExtensionCount   = 0;
  instInfo.ppEnabledExtensionNames = nullptr;


  VkInstance instance;
  VK_CHECK(vkCreateInstance(&instInfo, NULL, &instance));
  printf("Instância Vulkan criada com sucesso.\n");

  // 2) Enumera dispositivos físicos (GPUs)
  uint32_t gpuCount = 0;
  VK_CHECK(vkEnumeratePhysicalDevices(instance, &gpuCount, NULL));
  if (gpuCount == 0) {
    fprintf(stderr, "Nenhuma GPU com suporte a Vulkan encontrada.\n");
    vkDestroyInstance(instance, NULL);
    return 1;
  }

  auto gpus = reinterpret_cast<VkPhysicalDevice*>(
    std::malloc(sizeof(VkPhysicalDevice) * gpuCount)
  );
  VK_CHECK(vkEnumeratePhysicalDevices(instance, &gpuCount, gpus));
  printf("Encontradas %u GPU(s) com Vulkan:\n", gpuCount);

  for (uint32_t i = 0; i < gpuCount; i++) {
    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(gpus[i], &props);
    printf("  [%u] %s (Vulkan %u.%u)\n",
           i,
           props.deviceName,
           VK_VERSION_MAJOR(props.apiVersion),
           VK_VERSION_MINOR(props.apiVersion));
  }

  free(gpus);

  // 3) Limpa e sai
  vkDestroyInstance(instance, NULL);
  printf("Instância Vulkan destruída. Fim.\n");
  return 0;
}