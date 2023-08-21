#include "vulkanMgr.h"

int main() {
	VulkanInfo vkInfo = {};
	vkInfo.windowWidth = 800;
	vkInfo.windowHeight = 600;
	VulkanMgr vkMgr;
	vkMgr.StartVulkanContext(vkInfo);
	vkMgr.SetVulkanShader();
	vkMgr.PrepareTexture();
	vkMgr.ReadyToRender();
	vkMgr.PresentLoop();
	vkMgr.StopAndCleanVulkan();
	return 0;
}