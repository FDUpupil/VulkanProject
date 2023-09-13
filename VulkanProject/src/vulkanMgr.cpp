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
	mVkComp.CreateWindowMy(vkInfo.windowWidth, vkInfo.windowHeight, false, this);
	mVkComp.InitVulkanDevice();

	mVkComp.PrepareRenderOnScreen();
}

void VulkanMgr::SetVulkanShader() {
	std::string shaderSuffix = mVkImageLoader.CurrentShaderSuffix();
	std::string vertShaderFilePath = "shader/simple_shader_" + shaderSuffix + ".vert.spv";
	std::string fragShaderFilePath = "shader/simple_shader_" + shaderSuffix + ".frag.spv";
	mVkShaderSet.LoadVertShaderStageInfo(vertShaderFilePath);
	mVkShaderSet.LoadFragShaderStageInfo(fragShaderFilePath);
}

void VulkanMgr::PrepareTexture() {
	mVkImageLoader.LoadImageTexture(mVideoImageWidth, mVideoImageHeight, "seq/original_1280x720.yuv");
	int pixelPlaneCnt = mVkImageLoader.CurrentPixelPlaneCnt();
	mVkShaderSet.CLearDescriptorSetLayoutBindings();
	for (int i = 0; i < pixelPlaneCnt; i++) {
		VkSampler textureSampler = mVkComp.CreateSampler(0);
		mVkShaderSet.CreateAndLoadLayoutBinding(i, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, textureSampler);
		mVkImageLoader.BindTextureSampler(i, textureSampler);
	}
}

void VulkanMgr::ReadyToRender() {
	tComBuffers = mVkComp.CreateCommandBuffers();
	mVkRenderSet.LoadShaderSet(&mVkShaderSet);
	mVkRenderSet.CreateRenderPass();
	mVkRenderSet.CreateRnderPipelineLayout();
	mVkRenderSet.CreateRnderPipeline();
	mVkRenderSet.SetRenderTargetFrameBuffer();
	mVkRenderSet.AllocateDescriptorSets();
	mVkRenderSet.UpdateDescriptorSets(mVkImageLoader.ImageDesInfos());

	mVkImageLoader.ReadOneFrame();
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
	if (mHoldLoop) return;
	int frameIndex = mVkComp.GetSyncFrameIndex(currentFrame);
	vkResetCommandBuffer(tComBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);

	if(mPlay) mVkImageLoader.ReadOneFrame();
	
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

	mVkImageLoader.RecordCommand(commandBuffer);

	mVkRenderSet.BindRrenderSystem(commandBuffer, imageIndex);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer!");
	}
}

//´°¿ÚÂß¼­
void VulkanMgr::RunOrStop() {
	mPlay = !mPlay;
}

void VulkanMgr::Reset() {
	mHoldLoop = true;
	mVkComp.HoldLoop();
	SetVulkanShader();
	PrepareTexture();
	ReadyToRender();
	mHoldLoop = false;
}

void VulkanMgr::SetFmt(VkFormat imageFmt) {
	mVkImageLoader.SetImagePixelFormat(imageFmt);
	Reset();
}

void VulkanMgr::SetRes(int imageWidth, int imageHeight) {
	mVideoImageWidth = imageWidth;
	mVideoImageHeight = imageHeight;
	Reset();
}