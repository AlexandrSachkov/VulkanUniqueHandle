#ifndef VK_UNIQUE_HANDLE_H_
#define VK_UNIQUE_HANDLE_H_

#include "vulkan/vulkan.h"
#include <functional>
#include <assert.h>

namespace vkh {
    template<typename T>
    class VkUniqueHandleBase {
        public:
            VkUniqueHandleBase(T handle, std::function<void(T)> releaseCallback) 
                : _handle(handle), _release(releaseCallback) {}

            VkUniqueHandleBase(VkUniqueHandleBase<T>&& other) {
                release();

                _handle = other._handle;
                _release = other._release;

                other._handle = VK_NULL_HANDLE;
                other._release = [](T, VkDevice, const VkAllocationCallbacks*){};
            }

            VkUniqueHandleBase& operator=(VkUniqueHandleBase<T>&& other) {
                release();

                _handle = other._handle;
                _release = other._release;

                other._handle = VK_NULL_HANDLE;
                other._release = [](T, VkDevice, const VkAllocationCallbacks*){};
            }

            T& get() {
                return _handle;
            }

            ~VkUniqueHandleBase() {
                release();
            }

            void release() {
                if(_handle != VK_NULL_HANDLE){
                    _release(_handle);
                }
            }

        private:
            VkUniqueHandleBase(const VkUniqueHandleBase<T>&) = delete;
            VkUniqueHandleBase& operator=(const VkUniqueHandleBase<T>&) = delete;

            T _handle = VK_NULL_HANDLE;
            std::function<void(T)> _release;
    };

    template <typename T>
    class VkUniqueHandle : public VkUniqueHandleBase<T> {
        public:
            VkUniqueHandle() : VkUniqueHandle(VK_NULL_HANDLE) {}
            VkUniqueHandle(T handle) : VkUniqueHandleBase<T>(handle, [](T){
                    assert(false && "Unsupported handle type");
                }) {}
    };

    template <>
    class VkUniqueHandle<VkInstance> : public VkUniqueHandleBase<VkInstance> {
        public:
            VkUniqueHandle() : VkUniqueHandle(VK_NULL_HANDLE) {}

            VkUniqueHandle(VkInstance handle) 
                : VkUniqueHandle(handle, nullptr) {}

            VkUniqueHandle(VkInstance handle, const VkAllocationCallbacks* allocCallbacks) 
                : VkUniqueHandleBase<VkInstance>(handle, [allocCallbacks](VkInstance handle){
                    vkDestroyInstance(handle, allocCallbacks);
                }) {}
    };

    template <>
    class VkUniqueHandle<VkPhysicalDevice> : public VkUniqueHandleBase<VkPhysicalDevice> {
        public:
            VkUniqueHandle() : VkUniqueHandle(VK_NULL_HANDLE) {}

            VkUniqueHandle(VkPhysicalDevice handle) 
                : VkUniqueHandleBase<VkPhysicalDevice>(handle, [](VkPhysicalDevice handle){
                    //no release required
                }) {}
    };

    template <>
    class VkUniqueHandle<VkDevice> : public VkUniqueHandleBase<VkDevice> {
        public:
            VkUniqueHandle() : VkUniqueHandle(VK_NULL_HANDLE) {}

            VkUniqueHandle(VkDevice handle) 
                : VkUniqueHandle(handle, nullptr) {}

            VkUniqueHandle(VkDevice handle, const VkAllocationCallbacks* allocCallbacks) 
                : VkUniqueHandleBase<VkDevice>(handle, [allocCallbacks](VkDevice handle){
                    vkDestroyDevice(handle, allocCallbacks);
                }) {}
    };

    template <>
    class VkUniqueHandle<VkQueue> : public VkUniqueHandleBase<VkQueue> {
        public:
            VkUniqueHandle() : VkUniqueHandle(VK_NULL_HANDLE) {}

            VkUniqueHandle(VkQueue handle) 
                : VkUniqueHandleBase<VkQueue>(handle, [](VkQueue handle){
                    //no release required
                }) {}
    };

    template <>
    class VkUniqueHandle<VkSemaphore> : public VkUniqueHandleBase<VkSemaphore> {
        public:
            VkUniqueHandle() : VkUniqueHandle(VK_NULL_HANDLE, VK_NULL_HANDLE) {}

            VkUniqueHandle(VkSemaphore handle, VkDevice device) 
                : VkUniqueHandle(handle, device, nullptr) {}

            VkUniqueHandle(VkSemaphore handle, VkDevice device, const VkAllocationCallbacks* allocCallbacks) 
                : VkUniqueHandleBase<VkSemaphore>(handle, [device, allocCallbacks](VkSemaphore handle){
                    vkDestroySemaphore(device, handle, allocCallbacks);
                }) {}
    };

    template <>
    class VkUniqueHandle<VkCommandBuffer> : public VkUniqueHandleBase<VkCommandBuffer> {
        public:
            VkUniqueHandle() : VkUniqueHandle(VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE) {}

            VkUniqueHandle(VkCommandBuffer handle, VkDevice device, VkCommandPool pool) 
                : VkUniqueHandleBase<VkCommandBuffer>(handle, [device, pool](VkCommandBuffer handle){
                    vkFreeCommandBuffers(device, pool, 1, &handle);
                }) {}
    };

    template <>
    class VkUniqueHandle<VkFence> : public VkUniqueHandleBase<VkFence> {
        public:
            VkUniqueHandle() : VkUniqueHandle(VK_NULL_HANDLE, VK_NULL_HANDLE) {}

            VkUniqueHandle(VkFence handle, VkDevice device) 
                : VkUniqueHandle(handle, device, nullptr) {}

            VkUniqueHandle(VkFence handle, VkDevice device, const VkAllocationCallbacks* allocCallbacks) 
                : VkUniqueHandleBase<VkFence>(handle, [device, allocCallbacks](VkFence handle){
                    vkDestroyFence(device, handle, allocCallbacks);
                }) {}
    };

    template <>
    class VkUniqueHandle<VkDeviceMemory> : public VkUniqueHandleBase<VkDeviceMemory> {
        public:
            VkUniqueHandle() : VkUniqueHandle(VK_NULL_HANDLE, VK_NULL_HANDLE) {}

            VkUniqueHandle(VkDeviceMemory handle, VkDevice device) 
                : VkUniqueHandle(handle, device, nullptr) {}

            VkUniqueHandle(VkDeviceMemory handle, VkDevice device, const VkAllocationCallbacks* allocCallbacks) 
                : VkUniqueHandleBase<VkDeviceMemory>(handle, [device, allocCallbacks](VkDeviceMemory handle){
                    vkFreeMemory(device, handle, allocCallbacks);
                }) {}
    };

    template <>
    class VkUniqueHandle<VkBuffer> : public VkUniqueHandleBase<VkBuffer> {
        public:
            VkUniqueHandle() : VkUniqueHandle(VK_NULL_HANDLE, VK_NULL_HANDLE) {}

            VkUniqueHandle(VkBuffer handle, VkDevice device) 
                : VkUniqueHandle(handle, device, nullptr) {}

            VkUniqueHandle(VkBuffer handle, VkDevice device, const VkAllocationCallbacks* allocCallbacks) 
                : VkUniqueHandleBase<VkBuffer>(handle, [device, allocCallbacks](VkBuffer handle){
                    vkDestroyBuffer(device, handle, allocCallbacks);
                }) {}
    };

    template <>
    class VkUniqueHandle<VkImage> : public VkUniqueHandleBase<VkImage> {
        public:
            VkUniqueHandle() : VkUniqueHandle(VK_NULL_HANDLE, VK_NULL_HANDLE) {}

            VkUniqueHandle(VkImage handle, VkDevice device) 
                : VkUniqueHandle(handle, device, nullptr) {}

            VkUniqueHandle(VkImage handle, VkDevice device, const VkAllocationCallbacks* allocCallbacks) 
                : VkUniqueHandleBase<VkImage>(handle, [device, allocCallbacks](VkImage handle){
                    vkDestroyImage(device, handle, allocCallbacks);
                }) {}
    };

    template <>
    class VkUniqueHandle<VkEvent> : public VkUniqueHandleBase<VkEvent> {
        public:
            VkUniqueHandle() : VkUniqueHandle(VK_NULL_HANDLE, VK_NULL_HANDLE) {}

            VkUniqueHandle(VkEvent handle, VkDevice device) 
                : VkUniqueHandle(handle, device, nullptr) {}

            VkUniqueHandle(VkEvent handle, VkDevice device, const VkAllocationCallbacks* allocCallbacks) 
                : VkUniqueHandleBase<VkEvent>(handle, [device, allocCallbacks](VkEvent handle){
                    vkDestroyEvent(device, handle, allocCallbacks);
                }) {}
    };

    template <>
    class VkUniqueHandle<VkQueryPool> : public VkUniqueHandleBase<VkQueryPool> {
        public:
            VkUniqueHandle() : VkUniqueHandle(VK_NULL_HANDLE, VK_NULL_HANDLE) {}

            VkUniqueHandle(VkQueryPool handle, VkDevice device) 
                : VkUniqueHandle(handle, device, nullptr) {}

            VkUniqueHandle(VkQueryPool handle, VkDevice device, const VkAllocationCallbacks* allocCallbacks) 
                : VkUniqueHandleBase<VkQueryPool>(handle, [device, allocCallbacks](VkQueryPool handle){
                    vkDestroyQueryPool(device, handle, allocCallbacks);
                }) {}
    };

    template <>
    class VkUniqueHandle<VkBufferView> : public VkUniqueHandleBase<VkBufferView> {
        public:
            VkUniqueHandle() : VkUniqueHandle(VK_NULL_HANDLE, VK_NULL_HANDLE) {}

            VkUniqueHandle(VkBufferView handle, VkDevice device) 
                : VkUniqueHandle(handle, device, nullptr) {}

            VkUniqueHandle(VkBufferView handle, VkDevice device, const VkAllocationCallbacks* allocCallbacks) 
                : VkUniqueHandleBase<VkBufferView>(handle, [device, allocCallbacks](VkBufferView handle){
                    vkDestroyBufferView(device, handle, allocCallbacks);
                }) {}
    };

    template <>
    class VkUniqueHandle<VkImageView> : public VkUniqueHandleBase<VkImageView> {
        public:
            VkUniqueHandle() : VkUniqueHandle(VK_NULL_HANDLE, VK_NULL_HANDLE) {}

            VkUniqueHandle(VkImageView handle, VkDevice device) 
                : VkUniqueHandle(handle, device, nullptr) {}

            VkUniqueHandle(VkImageView handle, VkDevice device, const VkAllocationCallbacks* allocCallbacks) 
                : VkUniqueHandleBase<VkImageView>(handle, [device, allocCallbacks](VkImageView handle){
                    vkDestroyImageView(device, handle, allocCallbacks);
                }) {}
    };

    template <>
    class VkUniqueHandle<VkShaderModule> : public VkUniqueHandleBase<VkShaderModule> {
        public:
            VkUniqueHandle() : VkUniqueHandle(VK_NULL_HANDLE, VK_NULL_HANDLE) {}

            VkUniqueHandle(VkShaderModule handle, VkDevice device) 
                : VkUniqueHandle(handle, device, nullptr) {}

            VkUniqueHandle(VkShaderModule handle, VkDevice device, const VkAllocationCallbacks* allocCallbacks) 
                : VkUniqueHandleBase<VkShaderModule>(handle, [device, allocCallbacks](VkShaderModule handle){
                    vkDestroyShaderModule(device, handle, allocCallbacks);
                }) {}
    };

    template <>
    class VkUniqueHandle<VkPipelineCache> : public VkUniqueHandleBase<VkPipelineCache> {
        public:
            VkUniqueHandle() : VkUniqueHandle(VK_NULL_HANDLE, VK_NULL_HANDLE) {}

            VkUniqueHandle(VkPipelineCache handle, VkDevice device) 
                : VkUniqueHandle(handle, device, nullptr) {}

            VkUniqueHandle(VkPipelineCache handle, VkDevice device, const VkAllocationCallbacks* allocCallbacks) 
                : VkUniqueHandleBase<VkPipelineCache>(handle, [device, allocCallbacks](VkPipelineCache handle){
                    vkDestroyPipelineCache(device, handle, allocCallbacks);
                }) {}
    };

    template <>
    class VkUniqueHandle<VkPipelineLayout> : public VkUniqueHandleBase<VkPipelineLayout> {
        public:
            VkUniqueHandle() : VkUniqueHandle(VK_NULL_HANDLE, VK_NULL_HANDLE) {}

            VkUniqueHandle(VkPipelineLayout handle, VkDevice device) 
                : VkUniqueHandle(handle, device, nullptr) {}

            VkUniqueHandle(VkPipelineLayout handle, VkDevice device, const VkAllocationCallbacks* allocCallbacks) 
                : VkUniqueHandleBase<VkPipelineLayout>(handle, [device, allocCallbacks](VkPipelineLayout handle){
                    vkDestroyPipelineLayout(device, handle, allocCallbacks);
                }) {}
    };

    template <>
    class VkUniqueHandle<VkRenderPass> : public VkUniqueHandleBase<VkRenderPass> {
        public:
            VkUniqueHandle() : VkUniqueHandle(VK_NULL_HANDLE, VK_NULL_HANDLE) {}

            VkUniqueHandle(VkRenderPass handle, VkDevice device) 
                : VkUniqueHandle(handle, device, nullptr) {}

            VkUniqueHandle(VkRenderPass handle, VkDevice device, const VkAllocationCallbacks* allocCallbacks) 
                : VkUniqueHandleBase<VkRenderPass>(handle, [device, allocCallbacks](VkRenderPass handle){
                    vkDestroyRenderPass(device, handle, allocCallbacks);
                }) {}
    };

    template <>
    class VkUniqueHandle<VkPipeline> : public VkUniqueHandleBase<VkPipeline> {
        public:
            VkUniqueHandle() : VkUniqueHandle(VK_NULL_HANDLE, VK_NULL_HANDLE) {}

            VkUniqueHandle(VkPipeline handle, VkDevice device) 
                : VkUniqueHandle(handle, device, nullptr) {}

            VkUniqueHandle(VkPipeline handle, VkDevice device, const VkAllocationCallbacks* allocCallbacks) 
                : VkUniqueHandleBase<VkPipeline>(handle, [device, allocCallbacks](VkPipeline handle){
                    vkDestroyPipeline(device, handle, allocCallbacks);
                }) {}
    };

    template <>
    class VkUniqueHandle<VkDescriptorSetLayout> : public VkUniqueHandleBase<VkDescriptorSetLayout> {
        public:
            VkUniqueHandle() : VkUniqueHandle(VK_NULL_HANDLE, VK_NULL_HANDLE) {}

            VkUniqueHandle(VkDescriptorSetLayout handle, VkDevice device) 
                : VkUniqueHandle(handle, device, nullptr) {}

            VkUniqueHandle(VkDescriptorSetLayout handle, VkDevice device, const VkAllocationCallbacks* allocCallbacks) 
                : VkUniqueHandleBase<VkDescriptorSetLayout>(handle, [device, allocCallbacks](VkDescriptorSetLayout handle){
                    vkDestroyDescriptorSetLayout(device, handle, allocCallbacks);
                }) {}
    };

    template <>
    class VkUniqueHandle<VkSampler> : public VkUniqueHandleBase<VkSampler> {
        public:
            VkUniqueHandle() : VkUniqueHandle(VK_NULL_HANDLE, VK_NULL_HANDLE) {}

            VkUniqueHandle(VkSampler handle, VkDevice device) 
                : VkUniqueHandle(handle, device, nullptr) {}

            VkUniqueHandle(VkSampler handle, VkDevice device, const VkAllocationCallbacks* allocCallbacks) 
                : VkUniqueHandleBase<VkSampler>(handle, [device, allocCallbacks](VkSampler handle){
                    vkDestroySampler(device, handle, allocCallbacks);
                }) {}
    };

    template <>
    class VkUniqueHandle<VkDescriptorPool> : public VkUniqueHandleBase<VkDescriptorPool> {
        public:
            VkUniqueHandle() : VkUniqueHandle(VK_NULL_HANDLE, VK_NULL_HANDLE) {}

            VkUniqueHandle(VkDescriptorPool handle, VkDevice device) 
                : VkUniqueHandle(handle, device, nullptr) {}

            VkUniqueHandle(VkDescriptorPool handle, VkDevice device, const VkAllocationCallbacks* allocCallbacks) 
                : VkUniqueHandleBase<VkDescriptorPool>(handle, [device, allocCallbacks](VkDescriptorPool handle){
                    vkDestroyDescriptorPool(device, handle, allocCallbacks);
                }) {}
    };

    template <>
    class VkUniqueHandle<VkDescriptorSet> : public VkUniqueHandleBase<VkDescriptorSet> {
        public:
            VkUniqueHandle() : VkUniqueHandle(VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE) {}

            VkUniqueHandle(VkDescriptorSet handle, VkDevice device, VkDescriptorPool pool) 
                : VkUniqueHandleBase<VkDescriptorSet>(handle, [device, pool](VkDescriptorSet handle){
                    vkFreeDescriptorSets(device, pool, 1, &handle);
                }) {}
    };

    template <>
    class VkUniqueHandle<VkFramebuffer> : public VkUniqueHandleBase<VkFramebuffer> {
        public:
            VkUniqueHandle() : VkUniqueHandle(VK_NULL_HANDLE, VK_NULL_HANDLE) {}

            VkUniqueHandle(VkFramebuffer handle, VkDevice device) 
                : VkUniqueHandle(handle, device, nullptr) {}

            VkUniqueHandle(VkFramebuffer handle, VkDevice device, const VkAllocationCallbacks* allocCallbacks) 
                : VkUniqueHandleBase<VkFramebuffer>(handle, [device, allocCallbacks](VkFramebuffer handle){
                    vkDestroyFramebuffer(device, handle, allocCallbacks);
                }) {}
    };

    template <>
    class VkUniqueHandle<VkCommandPool> : public VkUniqueHandleBase<VkCommandPool> {
        public:
            VkUniqueHandle() : VkUniqueHandle(VK_NULL_HANDLE, VK_NULL_HANDLE) {}

            VkUniqueHandle(VkCommandPool handle, VkDevice device) 
                : VkUniqueHandle(handle, device, nullptr) {}

            VkUniqueHandle(VkCommandPool handle, VkDevice device, const VkAllocationCallbacks* allocCallbacks) 
                : VkUniqueHandleBase<VkCommandPool>(handle, [device, allocCallbacks](VkCommandPool handle){
                    vkDestroyCommandPool(device, handle, allocCallbacks);
                }) {}
    };

    template <>
    class VkUniqueHandle<VkSamplerYcbcrConversion> : public VkUniqueHandleBase<VkSamplerYcbcrConversion> {
        public:
            VkUniqueHandle() : VkUniqueHandle(VK_NULL_HANDLE, VK_NULL_HANDLE) {}

            VkUniqueHandle(VkSamplerYcbcrConversion handle, VkDevice device) 
                : VkUniqueHandle(handle, device, nullptr) {}

            VkUniqueHandle(VkSamplerYcbcrConversion handle, VkDevice device, const VkAllocationCallbacks* allocCallbacks) 
                : VkUniqueHandleBase<VkSamplerYcbcrConversion>(handle, [device, allocCallbacks](VkSamplerYcbcrConversion handle){
                    vkDestroySamplerYcbcrConversion(device, handle, allocCallbacks);
                }) {}
    };

    template <>
    class VkUniqueHandle<VkDescriptorUpdateTemplate> : public VkUniqueHandleBase<VkDescriptorUpdateTemplate> {
        public:
            VkUniqueHandle() : VkUniqueHandle(VK_NULL_HANDLE, VK_NULL_HANDLE) {}

            VkUniqueHandle(VkDescriptorUpdateTemplate handle, VkDevice device) 
                : VkUniqueHandle(handle, device, nullptr) {}

            VkUniqueHandle(VkDescriptorUpdateTemplate handle, VkDevice device, const VkAllocationCallbacks* allocCallbacks) 
                : VkUniqueHandleBase<VkDescriptorUpdateTemplate>(handle, [device, allocCallbacks](VkDescriptorUpdateTemplate handle){
                    vkDestroyDescriptorUpdateTemplate(device, handle, allocCallbacks);
                }) {}
    };



    template <>
    class VkUniqueHandle<VkSurfaceKHR> : public VkUniqueHandleBase<VkSurfaceKHR> {
        public:
            VkUniqueHandle() : VkUniqueHandle(VK_NULL_HANDLE, VK_NULL_HANDLE) {}
            
            VkUniqueHandle(VkSurfaceKHR handle, VkInstance instance) 
                : VkUniqueHandle(handle, instance, nullptr) {}

            VkUniqueHandle(VkSurfaceKHR handle, VkInstance instance, const VkAllocationCallbacks* allocCallbacks) 
                : VkUniqueHandleBase<VkSurfaceKHR>(handle, [instance, allocCallbacks](VkSurfaceKHR handle){
                    vkDestroySurfaceKHR(instance, handle, allocCallbacks);
                }) {}
    };

    template <>
    class VkUniqueHandle<VkSwapchainKHR> : public VkUniqueHandleBase<VkSwapchainKHR> {
        public:
            VkUniqueHandle() : VkUniqueHandle(VK_NULL_HANDLE, VK_NULL_HANDLE) {}

            VkUniqueHandle(VkSwapchainKHR handle, VkDevice device) 
                : VkUniqueHandle(handle, device, nullptr) {}

            VkUniqueHandle(VkSwapchainKHR handle, VkDevice device, const VkAllocationCallbacks* allocCallbacks) 
                : VkUniqueHandleBase<VkSwapchainKHR>(handle, [device, allocCallbacks](VkSwapchainKHR handle){
                    vkDestroySwapchainKHR(device, handle, allocCallbacks);
                }) {}
    };

    template <>
    class VkUniqueHandle<VkDebugUtilsMessengerEXT> : public VkUniqueHandleBase<VkDebugUtilsMessengerEXT> {
        public:
            VkUniqueHandle() : VkUniqueHandle(VK_NULL_HANDLE, VK_NULL_HANDLE) {}

            VkUniqueHandle(VkDebugUtilsMessengerEXT handle, VkInstance instance) 
                : VkUniqueHandle(handle, instance, nullptr) {}

            VkUniqueHandle(VkDebugUtilsMessengerEXT handle, VkInstance instance, const VkAllocationCallbacks* allocCallbacks) 
                : VkUniqueHandleBase<VkDebugUtilsMessengerEXT>(handle, [instance, allocCallbacks](VkDebugUtilsMessengerEXT handle){
                    vkDestroyDebugUtilsMessengerEXT(instance, handle, allocCallbacks);
                }) {}
    };

    template <>
    class VkUniqueHandle<VkDebugReportCallbackEXT> : public VkUniqueHandleBase<VkDebugReportCallbackEXT> {
        public:
            VkUniqueHandle() : VkUniqueHandle(VK_NULL_HANDLE, VK_NULL_HANDLE) {}

            VkUniqueHandle(VkDebugReportCallbackEXT handle, VkInstance instance) 
                : VkUniqueHandle(handle, instance, nullptr) {}

            VkUniqueHandle(VkDebugReportCallbackEXT handle, VkInstance instance, const VkAllocationCallbacks* allocCallbacks) 
                : VkUniqueHandleBase<VkDebugReportCallbackEXT>(handle, [instance, allocCallbacks](VkDebugReportCallbackEXT handle){
                    vkDestroyDebugReportCallbackEXT(instance, handle, allocCallbacks);
                }) {}
    };

    template <>
    class VkUniqueHandle<VkIndirectCommandsLayoutNVX> : public VkUniqueHandleBase<VkIndirectCommandsLayoutNVX> {
        public:
            VkUniqueHandle() : VkUniqueHandle(VK_NULL_HANDLE, VK_NULL_HANDLE) {}

            VkUniqueHandle(VkIndirectCommandsLayoutNVX handle, VkDevice device) 
                : VkUniqueHandle(handle, device, nullptr) {}

            VkUniqueHandle(VkIndirectCommandsLayoutNVX handle, VkDevice device, const VkAllocationCallbacks* allocCallbacks) 
                : VkUniqueHandleBase<VkIndirectCommandsLayoutNVX>(handle, [device, allocCallbacks](VkIndirectCommandsLayoutNVX handle){
                    vkDestroyIndirectCommandsLayoutNVX(device, handle, allocCallbacks);
                }) {}
    };

    template <>
    class VkUniqueHandle<VkObjectTableNVX> : public VkUniqueHandleBase<VkObjectTableNVX> {
        public:
            VkUniqueHandle() : VkUniqueHandle(VK_NULL_HANDLE, VK_NULL_HANDLE) {}

            VkUniqueHandle(VkObjectTableNVX handle, VkDevice device) 
                : VkUniqueHandle(handle, device, nullptr) {}

            VkUniqueHandle(VkObjectTableNVX handle, VkDevice device, const VkAllocationCallbacks* allocCallbacks) 
                : VkUniqueHandleBase<VkObjectTableNVX>(handle, [device, allocCallbacks](VkObjectTableNVX handle){
                    vkDestroyObjectTableNVX(device, handle, allocCallbacks);
                }) {}
    };

    template <>
    class VkUniqueHandle<VkValidationCacheEXT> : public VkUniqueHandleBase<VkValidationCacheEXT> {
        public:
            VkUniqueHandle() : VkUniqueHandle(VK_NULL_HANDLE, VK_NULL_HANDLE) {}

            VkUniqueHandle(VkValidationCacheEXT handle, VkDevice device) 
                : VkUniqueHandle(handle, device, nullptr) {}

            VkUniqueHandle(VkValidationCacheEXT handle, VkDevice device, const VkAllocationCallbacks* allocCallbacks) 
                : VkUniqueHandleBase<VkValidationCacheEXT>(handle, [device, allocCallbacks](VkValidationCacheEXT handle){
                    vkDestroyValidationCacheEXT(device, handle, allocCallbacks);
                }) {}
    };

    template <>
    class VkUniqueHandle<VkAccelerationStructureNV> : public VkUniqueHandleBase<VkAccelerationStructureNV> {
        public:
            VkUniqueHandle() : VkUniqueHandle(VK_NULL_HANDLE, VK_NULL_HANDLE) {}

            VkUniqueHandle(VkAccelerationStructureNV handle, VkDevice device) 
                : VkUniqueHandle(handle, device, nullptr) {}

            VkUniqueHandle(VkAccelerationStructureNV handle, VkDevice device, const VkAllocationCallbacks* allocCallbacks) 
                : VkUniqueHandleBase<VkAccelerationStructureNV>(handle, [device, allocCallbacks](VkAccelerationStructureNV handle){
                    vkDestroyAccelerationStructureNV(device, handle, allocCallbacks);
                }) {}
    };
}

#endif //VK_UNIQUE_HANDLE_H_