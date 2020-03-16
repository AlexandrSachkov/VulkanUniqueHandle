# VulkanUniqueHandle

This is a simple header-only wrapper around Vulkan API handles that turns them into unique handles with scoped lifetimes (analogous to std::unique_ptr).

## Benefits
 <ul>
  <li>Automatic release when handle goes out of scope</li>
  <li>No need to track if a handle has already been released</li>
  <li>No need to remember which methods are used to release which handles</li>
  <li>No need to remember which handles require release and which do not</li>
  <li>No need to track which device/instance/allocator was used with each handle during creation</li>
  <li>Simple and extensible</li>
  <li>Supports the following Vulkan handles:
    <ul>
      <li>VkInstance</li>
      <li>VkPhysicalDevice</li>
      <li>VkDevice</li>
      <li>VkQueue</li>
      <li>VkSemaphore</li>
      <li>VkCommandBuffer</li>
      <li>VkFence</li>
      <li>VkDeviceMemory</li>
      <li>VkBuffer</li>
      <li>VkImage</li>
      <li>VkEvent</li>
      <li>VkQueryPool</li>
      <li>VkBufferView</li>
      <li>VkImageView</li>
      <li>VkShaderModule</li>
      <li>VkPipelineCache</li>
      <li>VkPipelineLayout</li>
      <li>VkRenderPass</li>
      <li>VkPipeline</li>
      <li>VkDescriptorSetLayout</li>
      <li>VkSampler</li>
      <li>VkDescriptorPool</li>
      <li>VkDescriptorSet</li>
      <li>VkFramebuffer</li>
      <li>VkCommandPool</li>
      <li>VkSamplerYcbcrConversion</li>
      <li>VkDescriptorUpdateTemplate</li>
      <li>VkSurfaceKHR</li>
      <li>VkSwapchainKHR</li>
      <li>VkDebugUtilsMessengerEXT</li>
      <li>VkDebugReportCallbackEXT</li>
      <li>VkIndirectCommandsLayoutNVX</li>
      <li>VkObjectTableNVX</li>
      <li>VkValidationCacheEXT</li>
      <li>VkAccelerationStructureNV</li>
    </ul>
  </li>
</ul> 

## Dependencies
<ul>
 <li>Vulkan 1.2</li>
 <li>C++ 11</li>
</ul>

## Usage

Here is an example how to create and initialize a handle for a swap chain:
```cpp
#include "vkh/VkUniqueHandle.h"

void createSwapChain() {
// initialize with a null handle and the device that will be used during creation
vkh::VkUniqueHandle<VkSwapchainKHR> vkSwapChain(VK_NULL_HANDLE, vkDevice);

VkSwapchainCreateInfoKHR createInfo = {};
// pass a reference to the real handle during creation
vkCreateSwapchainKHR(m_vkDevice, &createInfo, nullptr, &vkSwapChain.get());

// vkSwapChain is automatically released when function returns using vkDestroySwapchainKHR
}
```

For member variable initialization, initialize using default constructor, then use move semantics to update:
```cpp
#include "vkh/VkUniqueHandle.h"

//Define member variable vkh::VkUniqueHandle<VkSwapchainKHR> m_vkSwapChain;

void createSwapChain() {
// initialize with a new null handle and the device that will be used during creation
m_vkSwapChain = std::move(vkh::VkUniqueHandle<VkSwapchainKHR>(VK_NULL_HANDLE, vkDevice));

VkSwapchainCreateInfoKHR createInfo = {};
// pass a reference to the real handle during creation
vkCreateSwapchainKHR(m_vkDevice, &createInfo, nullptr, &m_vkSwapChain.get());
}
```
