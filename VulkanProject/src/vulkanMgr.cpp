#include "vulkanMgr.h"

VulkanMgr::VulkanMgr() {
	pVkComp = new VulkanComp();
	pVkShaderSet = new VulkanShaderSet(pVkComp);
	pVkRenderSet = new VulkanRenderSet(pVkComp);
	pVkImageLoader = new VulkanImageLoader(pVkComp);
}

VulkanMgr::~VulkanMgr() {
	if (pVkComp) {
		delete pVkComp;
		pVkComp = nullptr;
	}

	if (pVkShaderSet) {
		delete pVkShaderSet;
		pVkShaderSet = nullptr;
	}

	if (pVkRenderSet) {
		delete pVkRenderSet;
		pVkRenderSet = nullptr;
	}

	if (pVkImageLoader) {
		delete pVkImageLoader;
		pVkImageLoader = nullptr;
	}
}

void VulkanMgr::StartVulkanContext(VulkanInfo vkInfo) {
	mVkInfo = vkInfo;
	pVkComp->PrepareVulkan(false);
	pVkComp->CreateWindowMy(vkInfo.windowWidth, vkInfo.windowHeight, false, this);
	pVkComp->InitVulkanDevice();

	pVkComp->PrepareRenderOnScreen();
}

void VulkanMgr::SetVulkanShader() {
	std::string shaderSuffix = pVkImageLoader->CurrentShaderSuffix();
	std::string vertShaderFilePath = "shader/simple_shader_" + shaderSuffix + ".vert.spv";
	std::string fragShaderFilePath = "shader/simple_shader_" + shaderSuffix + ".frag.spv";
	printf("path : %s", fragShaderFilePath.c_str());
	pVkShaderSet->LoadVertShaderStageInfo(vertShaderFilePath);
	pVkShaderSet->LoadFragShaderStageInfo(fragShaderFilePath);
}

void VulkanMgr::PrepareTexture() {
	pVkImageLoader->LoadImageTexture(mVideoImageWidth, mVideoImageHeight, mInputSeqFile);
	int pixelPlaneCnt = pVkImageLoader->CurrentPixelPlaneCnt();
	pVkShaderSet->CLearDescriptorSetLayoutBindings();
	for (int i = 0; i < pixelPlaneCnt; i++) {
		VkSampler textureSampler = pVkComp->CreateSampler(0);
		pVkShaderSet->CreateAndLoadLayoutBinding(i, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, textureSampler);
		pVkImageLoader->BindTextureSampler(i, textureSampler);
	}
}

void VulkanMgr::ReadyToRender() {
	tComBuffers = pVkComp->CreateCommandBuffers();
	pVkRenderSet->LoadShaderSet(pVkShaderSet);
	pVkRenderSet->CreateRenderPass();
	pVkRenderSet->CreateRnderPipelineLayout();
	pVkRenderSet->CreateRnderPipeline();
	pVkRenderSet->SetRenderTargetFrameBuffer();
	pVkRenderSet->AllocateDescriptorSets();
	pVkRenderSet->UpdateDescriptorSets(pVkImageLoader->ImageDesInfos());

	//pVkImageLoader->ReadOneFrame();
}

void VulkanMgr::PresentLoop() {
	pVkComp->ShowLoop(std::bind(&VulkanMgr::DrawFrame, this));
}

void VulkanMgr::StopAndCleanVulkan() {
	pVkComp->CleanComponet();
	pVkRenderSet->CleanRenderComponet();
	pVkComp->DestoryInstance();
}

void VulkanMgr::DrawFrame() {
	if (mHoldLoop) return;
	int frameIndex = pVkComp->GetSyncFrameIndex(currentFrame);
	vkResetCommandBuffer(tComBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);

	if(mPlay) pVkImageLoader->ReadOneFrame();
	
	recordCommandBuffer(tComBuffers[currentFrame], frameIndex);

	pVkComp->SubmitCommand(tComBuffers[currentFrame], currentFrame);

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void VulkanMgr::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	pVkImageLoader->RecordCommand(commandBuffer);

	pVkRenderSet->BindRrenderSystem(commandBuffer, imageIndex);

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
	pVkComp->HoldLoop();
	SetVulkanShader();
	PrepareTexture();
	ReadyToRender();
	mHoldLoop = false;
}

void VulkanMgr::SetFmt(VkFormat imageFmt) {
	pVkImageLoader->SetImagePixelFormat(imageFmt);
	Reset();
}

void VulkanMgr::SetRes(int imageWidth, int imageHeight) {
	mVideoImageWidth = imageWidth;
	mVideoImageHeight = imageHeight;
	Reset();
}

void VulkanMgr::SetInputFile(std::string imageFilePath) {
	mInputSeqFile = imageFilePath;
	Reset();
}

void VulkanMgr::CloseWnd() {
	pVkComp->CloseGLFW();
}