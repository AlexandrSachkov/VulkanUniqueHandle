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
                other._release = [](T){};
            }

            VkUniqueHandleBase& operator=(VkUniqueHandleBase<T>&& other) {
                release();

                _handle = other._handle;
                _release = other._release;

                other._handle = VK_NULL_HANDLE;
                other._release = [](T){};
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
                    _handle = VK_NULL_HANDLE;
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

            VkUniqueHandle(T&& other) : VkUniqueHandleBase<T>(std::move(other)) {}
            VkUniqueHandle& operator=(T&& other) {
                VkUniqueHandleBase<T>::operator=(std::move(other));
                return *this;
            }
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

            VkUniqueHandle(VkUniqueHandle&& other) : VkUniqueHandleBase<VkInstance>(std::move(other)) {}

            VkUniqueHandle& operator=(VkUniqueHandle&& other){
                VkUniqueHandleBase<VkInstance>::operator=(std::move(other));
                return *this;
            }
    };

    template <>
    class VkUniqueHandle<VkPhysicalDevice> : public VkUniqueHandleBase<VkPhysicalDevice> {
        public:
            VkUniqueHandle() : VkUniqueHandle(VK_NULL_HANDLE) {}

            VkUniqueHandle(VkPhysicalDevice handle) 
                : VkUniqueHandleBase<VkPhysicalDevice>(handle, [](VkPhysicalDevice handle){
                    //no release required
                }) {}

            VkUniqueHandle(VkUniqueHandle&& other) : VkUniqueHandleBase<VkPhysicalDevice>(std::move(other)) {}

            VkUniqueHandle& operator=(VkUniqueHandle&& other){
                VkUniqueHandleBase<VkPhysicalDevice>::operator=(std::move(other));
                return *this;
            }
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

            VkUniqueHandle(VkUniqueHandle&& other) : VkUniqueHandleBase<VkDevice>(std::move(other)) {}

            VkUniqueHandle& operator=(VkUniqueHandle&& other){
                VkUniqueHandleBase<VkDevice>::operator=(std::move(other));
                return *this;
            }
    };

    template <>
    class VkUniqueHandle<VkQueue> : public VkUniqueHandleBase<VkQueue> {
        public:
            VkUniqueHandle() : VkUniqueHandle(VK_NULL_HANDLE) {}

            VkUniqueHandle(VkQueue handle) 
                : VkUniqueHandleBase<VkQueue>(handle, [](VkQueue handle){
                    //no release required
                }) {}

            VkUniqueHandle(VkUniqueHandle&& other) : VkUniqueHandleBase<VkQueue>(std::move(other)) {}

            VkUniqueHandle& operator=(VkUniqueHandle&& other){
                VkUniqueHandleBase<VkQueue>::operator=(std::move(other));
                return *this;
            }
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

            VkUniqueHandle(VkUniqueHandle&& other) : VkUniqueHandleBase<VkSemaphore>(std::move(other)) {}

            VkUniqueHandle& operator=(VkUniqueHandle&& other){
                VkUniqueHandleBase<VkSemaphore>::operator=(std::move(other));
                return *this;
            }
    };

    template <>
    class VkUniqueHandle<VkCommandBuffer> : public VkUniqueHandleBase<VkCommandBuffer> {
        public:
            VkUniqueHandle() : VkUniqueHandle(VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE) {}

            VkUniqueHandle(VkCommandBuffer handle, VkDevice device, VkCommandPool pool) 
                : VkUniqueHandleBase<VkCommandBuffer>(handle, [device, pool](VkCommandBuffer handle){
                    vkFreeCommandBuffers(device, pool, 1, &handle);
                }) {}

            VkUniqueHandle(VkUniqueHandle&& other) : VkUniqueHandleBase<VkCommandBuffer>(std::move(other)) {}

            VkUniqueHandle& operator=(VkUniqueHandle&& other){
                VkUniqueHandleBase<VkCommandBuffer>::operator=(std::move(other));
                return *this;
            }
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

            VkUniqueHandle(VkUniqueHandle&& other) : VkUniqueHandleBase<VkFence>(std::move(other)) {}

            VkUniqueHandle& operator=(VkUniqueHandle&& other){
                VkUniqueHandleBase<VkFence>::operator=(std::move(other));
                return *this;
            }
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

            VkUniqueHandle(VkUniqueHandle&& other) : VkUniqueHandleBase<VkDeviceMemory>(std::move(other)) {}

            VkUniqueHandle& operator=(VkUniqueHandle&& other){
                VkUniqueHandleBase<VkDeviceMemory>::operator=(std::move(other));
                return *this;
            }
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

            VkUniqueHandle(VkUniqueHandle&& other) : VkUniqueHandleBase<VkBuffer>(std::move(other)) {}

            VkUniqueHandle& operator=(VkUniqueHandle&& other){
                VkUniqueHandleBase<VkBuffer>::operator=(std::move(other));
                return *this;
            }
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

            VkUniqueHandle(VkUniqueHandle&& other) : VkUniqueHandleBase<VkImage>(std::move(other)) {}

            VkUniqueHandle& operator=(VkUniqueHandle&& other){
                VkUniqueHandleBase<VkImage>::operator=(std::move(other));
                return *this;
            }
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

            VkUniqueHandle(VkUniqueHandle&& other) : VkUniqueHandleBase<VkEvent>(std::move(other)) {}

            VkUniqueHandle& operator=(VkUniqueHandle&& other){
                VkUniqueHandleBase<VkEvent>::operator=(std::move(other));
                return *this;
            }
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

            VkUniqueHandle(VkUniqueHandle&& other) : VkUniqueHandleBase<VkQueryPool>(std::move(other)) {}

            VkUniqueHandle& operator=(VkUniqueHandle&& other){
                VkUniqueHandleBase<VkQueryPool>::operator=(std::move(other));
                return *this;
            }
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

            VkUniqueHandle(VkUniqueHandle&& other) : VkUniqueHandleBase<VkBufferView>(std::move(other)) {}

            VkUniqueHandle& operator=(VkUniqueHandle&& other){
                VkUniqueHandleBase<VkBufferView>::operator=(std::move(other));
                return *this;
            }
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

            VkUniqueHandle(VkUniqueHandle&& other) : VkUniqueHandleBase<VkImageView>(std::move(other)) {}

            VkUniqueHandle& operator=(VkUniqueHandle&& other){
                VkUniqueHandleBase<VkImageView>::operator=(std::move(other));
                return *this;
            }
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

            VkUniqueHandle(VkUniqueHandle&& other) : VkUniqueHandleBase<VkShaderModule>(std::move(other)) {}

            VkUniqueHandle& operator=(VkUniqueHandle&& other){
                VkUniqueHandleBase<VkShaderModule>::operator=(std::move(other));
                return *this;
            }
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

            VkUniqueHandle(VkUniqueHandle&& other) : VkUniqueHandleBase<VkPipelineCache>(std::move(other)) {}

            VkUniqueHandle& operator=(VkUniqueHandle&& other){
                VkUniqueHandleBase<VkPipelineCache>::operator=(std::move(other));
                return *this;
            }
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

            VkUniqueHandle(VkUniqueHandle&& other) : VkUniqueHandleBase<VkPipelineLayout>(std::move(other)) {}

            VkUniqueHandle& operator=(VkUniqueHandle&& other){
                VkUniqueHandleBase<VkPipelineLayout>::operator=(std::move(other));
                return *this;
            }
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

            VkUniqueHandle(VkUniqueHandle&& other) : VkUniqueHandleBase<VkRenderPass>(std::move(other)) {}

            VkUniqueHandle& operator=(VkUniqueHandle&& other){
                VkUniqueHandleBase<VkRenderPass>::operator=(std::move(other));
                return *this;
            }
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

            VkUniqueHandle(VkUniqueHandle&& other) : VkUniqueHandleBase<VkPipeline>(std::move(other)) {}

            VkUniqueHandle& operator=(VkUniqueHandle&& other){
                VkUniqueHandleBase<VkPipeline>::operator=(std::move(other));
                return *this;
            }
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

            VkUniqueHandle(VkUniqueHandle&& other) : VkUniqueHandleBase<VkDescriptorSetLayout>(std::move(other)) {}

            VkUniqueHandle& operator=(VkUniqueHandle&& other){
                VkUniqueHandleBase<VkDescriptorSetLayout>::operator=(std::move(other));
                return *this;
            }
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

            VkUniqueHandle(VkUniqueHandle&& other) : VkUniqueHandleBase<VkSampler>(std::move(other)) {}

            VkUniqueHandle& operator=(VkUniqueHandle&& other){
                VkUniqueHandleBase<VkSampler>::operator=(std::move(other));
                return *this;
            }
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

            VkUniqueHandle(VkUniqueHandle&& other) : VkUniqueHandleBase<VkDescriptorPool>(std::move(other)) {}

            VkUniqueHandle& operator=(VkUniqueHandle&& other){
                VkUniqueHandleBase<VkDescriptorPool>::operator=(std::move(other));
                return *this;
            }
    };

    template <>
    class VkUniqueHandle<VkDescriptorSet> : public VkUniqueHandleBase<VkDescriptorSet> {
        public:
            VkUniqueHandle() : VkUniqueHandle(VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE) {}

            VkUniqueHandle(VkDescriptorSet handle, VkDevice device, VkDescriptorPool pool) 
                : VkUniqueHandleBase<VkDescriptorSet>(handle, [device, pool](VkDescriptorSet handle){
                    vkFreeDescriptorSets(device, pool, 1, &handle);
                }) {}

            VkUniqueHandle(VkUniqueHandle&& other) : VkUniqueHandleBase<VkDescriptorSet>(std::move(other)) {}

            VkUniqueHandle& operator=(VkUniqueHandle&& other){
                VkUniqueHandleBase<VkDescriptorSet>::operator=(std::move(other));
                return *this;
            }
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

            VkUniqueHandle(VkUniqueHandle&& other) : VkUniqueHandleBase<VkFramebuffer>(std::move(other)) {}

            VkUniqueHandle& operator=(VkUniqueHandle&& other){
                VkUniqueHandleBase<VkFramebuffer>::operator=(std::move(other));
                return *this;
            }
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

            VkUniqueHandle(VkUniqueHandle&& other) : VkUniqueHandleBase<VkCommandPool>(std::move(other)) {}

            VkUniqueHandle& operator=(VkUniqueHandle&& other){
                VkUniqueHandleBase<VkCommandPool>::operator=(std::move(other));
                return *this;
            }
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

            VkUniqueHandle(VkUniqueHandle&& other) : VkUniqueHandleBase<VkSamplerYcbcrConversion>(std::move(other)) {}

            VkUniqueHandle& operator=(VkUniqueHandle&& other){
                VkUniqueHandleBase<VkSamplerYcbcrConversion>::operator=(std::move(other));
                return *this;
            }
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

            VkUniqueHandle(VkUniqueHandle&& other) : VkUniqueHandleBase<VkDescriptorUpdateTemplate>(std::move(other)) {}

            VkUniqueHandle& operator=(VkUniqueHandle&& other){
                VkUniqueHandleBase<VkDescriptorUpdateTemplate>::operator=(std::move(other));
                return *this;
            }
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

            VkUniqueHandle(VkUniqueHandle&& other) : VkUniqueHandleBase<VkSurfaceKHR>(std::move(other)) {}

            VkUniqueHandle& operator=(VkUniqueHandle&& other){
                VkUniqueHandleBase<VkSurfaceKHR>::operator=(std::move(other));
                return *this;
            }
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

            VkUniqueHandle(VkUniqueHandle&& other) : VkUniqueHandleBase<VkSwapchainKHR>(std::move(other)) {}

            VkUniqueHandle& operator=(VkUniqueHandle&& other){
                VkUniqueHandleBase<VkSwapchainKHR>::operator=(std::move(other));
                return *this;
            }
    };

    template <>
    class VkUniqueHandle<VkDebugUtilsMessengerEXT> : public VkUniqueHandleBase<VkDebugUtilsMessengerEXT> {
        public:
            VkUniqueHandle() : VkUniqueHandle(VK_NULL_HANDLE, VK_NULL_HANDLE) {}

            VkUniqueHandle(VkDebugUtilsMessengerEXT handle, VkInstance instance) 
                : VkUniqueHandle(handle, instance, nullptr) {}

            VkUniqueHandle(VkDebugUtilsMessengerEXT handle, VkInstance instance, const VkAllocationCallbacks* allocCallbacks) 
                : VkUniqueHandleBase<VkDebugUtilsMessengerEXT>(handle, [instance, allocCallbacks](VkDebugUtilsMessengerEXT handle){
                    auto destroyDebugMessenger = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance,"vkDestroyDebugUtilsMessengerEXT");
                    if (destroyDebugMessenger != nullptr) {
                        destroyDebugMessenger(instance, handle, allocCallbacks);
                    }
                }) {}

            VkUniqueHandle(VkUniqueHandle&& other) : VkUniqueHandleBase<VkDebugUtilsMessengerEXT>(std::move(other)) {}

            VkUniqueHandle& operator=(VkUniqueHandle&& other){
                VkUniqueHandleBase<VkDebugUtilsMessengerEXT>::operator=(std::move(other));
                return *this;
            }
    };

    template <>
    class VkUniqueHandle<VkDebugReportCallbackEXT> : public VkUniqueHandleBase<VkDebugReportCallbackEXT> {
        public:
            VkUniqueHandle() : VkUniqueHandle(VK_NULL_HANDLE, VK_NULL_HANDLE) {}

            VkUniqueHandle(VkDebugReportCallbackEXT handle, VkInstance instance) 
                : VkUniqueHandle(handle, instance, nullptr) {}

            VkUniqueHandle(VkDebugReportCallbackEXT handle, VkInstance instance, const VkAllocationCallbacks* allocCallbacks) 
                : VkUniqueHandleBase<VkDebugReportCallbackEXT>(handle, [instance, allocCallbacks](VkDebugReportCallbackEXT handle){
                    auto destroyDebugReportCallback = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance,"vkDestroyDebugReportCallbackEXT");
                    if (destroyDebugReportCallback != nullptr) {
                        destroyDebugReportCallback(instance, handle, allocCallbacks);
                    }
                }) {}

            VkUniqueHandle(VkUniqueHandle&& other) : VkUniqueHandleBase<VkDebugReportCallbackEXT>(std::move(other)) {}

            VkUniqueHandle& operator=(VkUniqueHandle&& other){
                VkUniqueHandleBase<VkDebugReportCallbackEXT>::operator=(std::move(other));
                return *this;
            }
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

            VkUniqueHandle(VkUniqueHandle&& other) : VkUniqueHandleBase<VkIndirectCommandsLayoutNVX>(std::move(other)) {}

            VkUniqueHandle& operator=(VkUniqueHandle&& other){
                VkUniqueHandleBase<VkIndirectCommandsLayoutNVX>::operator=(std::move(other));
                return *this;
            }
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

            VkUniqueHandle(VkUniqueHandle&& other) : VkUniqueHandleBase<VkObjectTableNVX>(std::move(other)) {}

            VkUniqueHandle& operator=(VkUniqueHandle&& other){
                VkUniqueHandleBase<VkObjectTableNVX>::operator=(std::move(other));
                return *this;
            }
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

            VkUniqueHandle(VkUniqueHandle&& other) : VkUniqueHandleBase<VkValidationCacheEXT>(std::move(other)) {}

            VkUniqueHandle& operator=(VkUniqueHandle&& other){
                VkUniqueHandleBase<VkValidationCacheEXT>::operator=(std::move(other));
                return *this;
            }
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

            VkUniqueHandle(VkUniqueHandle&& other) : VkUniqueHandleBase<VkAccelerationStructureNV>(std::move(other)) {}

            VkUniqueHandle& operator=(VkUniqueHandle&& other){
                VkUniqueHandleBase<VkAccelerationStructureNV>::operator=(std::move(other));
                return *this;
            }
    };
}

#endif //VK_UNIQUE_HANDLE_H_