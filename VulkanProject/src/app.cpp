#include "vulkanMgr.h"

int main() {
	VulkanInfo vkInfo = {};
	vkInfo.windowWidth = 1920;
	vkInfo.windowHeight = 1080;
	VulkanMgr vkMgr;
	vkMgr.StartVulkanContext(vkInfo);
	vkMgr.SetVulkanShader();
	vkMgr.PrepareTexture();
	vkMgr.ReadyToRender();
	vkMgr.PresentLoop();
	vkMgr.StopAndCleanVulkan();
	return 0;
}