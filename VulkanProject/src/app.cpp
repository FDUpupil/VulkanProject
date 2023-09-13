#include "vulkanMgr.h"

int main() {
	VulkanInfo vkInfo = {};
	vkInfo.windowWidth = 1280;
	vkInfo.windowHeight = 720;
	VulkanMgr vkMgr;
	vkMgr.StartVulkanContext(vkInfo);
	vkMgr.SetVulkanShader();
	vkMgr.PrepareTexture();
	vkMgr.ReadyToRender();
	vkMgr.PresentLoop();
	vkMgr.StopAndCleanVulkan();
	return 0;
}