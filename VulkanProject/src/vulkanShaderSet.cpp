#include "vulkanShaderSet.h"
#include "vulkanComp.h"

VulkanShaderSet::VulkanShaderSet(VulkanComp* comp) : vkCompPtr(comp) {
}

VulkanShaderSet::~VulkanShaderSet() {
}

std::vector<char> VulkanShaderSet::readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}


void VulkanShaderSet::LoadVertShaderStageInfo(const std::string vertFilePath) {
	auto vertShaderCode = readFile(vertFilePath);
    VkShaderModule vertShaderModule = vkCompPtr->LoadShaderModule(vertShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";
    vertShaderStageInfo.flags = 0;
    vertShaderStageInfo.pNext = nullptr;
    vertShaderStageInfo.pSpecializationInfo = nullptr;

    shaderStageCreateInfos.push_back(vertShaderStageInfo);
}

void VulkanShaderSet::LoadFragShaderStageInfo(const std::string fragFilePath) {
    auto fragShaderCode = readFile(fragFilePath);
    VkShaderModule fragShaderModule = vkCompPtr->LoadShaderModule(fragShaderCode);

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    shaderStageCreateInfos.push_back(fragShaderStageInfo);
}

void VulkanShaderSet::CreateAndLoadLayoutBinding(uint32_t id, VkDescriptorType type, VkShaderStageFlagBits flag, VkSampler sampler) {
    VkDescriptorSetLayoutBinding binding;
    binding.binding = id;
    binding.descriptorCount = 1;
    binding.descriptorType = type;
    binding.pImmutableSamplers = nullptr;
    binding.stageFlags = flag;
    //if (sampler) {
    //    mTextureSamplers.push_back(sampler);
    //    binding.pImmutableSamplers = &mTextureSamplers[0];
    //}
    
    descriptorSetLayoutBindings.push_back(binding);
}