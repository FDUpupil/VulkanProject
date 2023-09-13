#pragma once

#include "vulkanBase.h"

class VulkanComp;
class VulkanShaderSet;

class VulkanRenderSet {
public:
	VulkanRenderSet() = default;
	VulkanRenderSet(VulkanComp* comp);
	~VulkanRenderSet();

	void LoadShaderSet(VulkanShaderSet* shaderSet);
	void CreateRenderPass();
	void CreateRnderPipelineLayout();
	void CreateRnderPipeline();
	void SetRenderTargetFrameBuffer();
	void AllocateDescriptorSets();
	void UpdateDescriptorSets(std::vector<VkDescriptorImageInfo> imageDesInfos);
	void CleanRenderComponet();

	void AddPushConstant(const VkPushConstantRange& constantRange, const char* data);

	void BindRrenderSystem(VkCommandBuffer commandBuffer, int imageIndex);

private:

	VkShaderModule createShaderModule(const std::vector<char>& code);

	VulkanComp* vkCompPtr;
	VulkanShaderSet* vkShaderSetPtr;

	VkViewport viewport{};
	VkRect2D scissor{};

	VkRenderPass mRenderPass;
	VkPipelineLayout mPipelineLayout;
	VkPipeline mGraphicsPipeline;

	std::vector<VkPushConstantRange> pushConstants;
	VkDescriptorSetLayout descriptorSetLayout = NULL;

	std::vector<VkFramebuffer> renderTargetBuffer;
	std::vector<VkDescriptorSet> descriptorSets;

	std::vector<VkDescriptorImageInfo> mImageDesInfos;
};