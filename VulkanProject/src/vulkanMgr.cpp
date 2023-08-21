#include "vulkanMgr.h"

VulkanMgr::VulkanMgr() {
	mVkShaderSet = VulkanShaderSet(&mVkComp);
	mVkRenderSet = VulkanRenderSet(&mVkComp);
	mVkImageLoader = VulkanImageLoader(&mVkComp);
}

VulkanMgr::~VulkanMgr() {

}

void VulkanMgr::StartVulkanContext(VulkanInfo vkInfo) {
	mVkInfo = vkInfo;
	mVkComp.PrepareVulkan(false);
	mVkComp.CreateWindow(vkInfo.windowWidth, vkInfo.windowHeight, false);
	mVkComp.InitVulkanDevice();

	mVkComp.PrepareRenderOnScreen();
}

void VulkanMgr::SetVulkanShader() {
	mVkShaderSet.LoadVertShaderStageInfo("D:/WorkProject/VulkanProject/VulkanProject/shader/simple_shader.vert.spv");
	mVkShaderSet.LoadFragShaderStageInfo("D:/WorkProject/VulkanProject/VulkanProject/shader/simple_shader.frag.spv");
}

void VulkanMgr::PrepareTexture() {
	mVkImageLoader.LoadImageTexture(2560, 1440, "D:/WorkProject/VulkanProject/VulkanProject/seq/rawData.yuv");
	VkSampler textureSampler = mVkComp.CreateSampler(1);
	mVkShaderSet.CreateAndLoadLayoutBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, textureSampler);
	mVkImageLoader.BindTextureSampler(textureSampler);
}

void VulkanMgr::ReadyToRender() {
	tComBuffers = mVkComp.CreateCommandBuffers();
	mVkRenderSet.LoadShaderSet(&mVkShaderSet);
	mVkRenderSet.CreateRenderPass();
	mVkRenderSet.CreateRnderPipelineLayout();
	mVkRenderSet.CreateRnderPipeline();
	mVkRenderSet.SetRenderTargetFrameBuffer();
	mVkRenderSet.AllocateDescriptorSets();
	mVkRenderSet.UpdateDescriptorSets(mVkImageLoader.ImageDesInfo());
}

void VulkanMgr::PresentLoop() {
	mVkComp.ShowLoop(std::bind(&VulkanMgr::DrawFrame, this));
}

void VulkanMgr::StopAndCleanVulkan() {
	mVkComp.CleanComponet();
	mVkRenderSet.CleanRenderComponet();
	mVkComp.DestoryInstance();
}

void VulkanMgr::DrawFrame() {
	int frameIndex = mVkComp.GetSyncFrameIndex(currentFrame);
	vkResetCommandBuffer(tComBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
	recordCommandBuffer(tComBuffers[currentFrame], frameIndex);

	mVkComp.SubmitCommand(tComBuffers[currentFrame], currentFrame);

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void VulkanMgr::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	mVkRenderSet.BindRrenderSystem(commandBuffer, imageIndex);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer!");
	}
}