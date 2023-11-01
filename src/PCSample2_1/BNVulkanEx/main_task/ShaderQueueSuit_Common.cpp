#include "ShaderQueueSuit_Common.h"
#include <assert.h>
#include "../util/HelpFunction.h"
#include "../util/FileUtil.h"
#include "MyVulkanManager.h"
#include "ShaderCompileUtil.h"
#include "../legencyUtil/util.hpp"
#include "PathData.h"
void ShaderQueueSuit_Common::create_uniform_buffer(VkDevice &device, VkPhysicalDeviceMemoryProperties &memoryroperties)
{
    bufferByteCount                = sizeof(float) * 16;
    VkBufferCreateInfo buf_info    = {};
    buf_info.sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buf_info.pNext                 = NULL;
    buf_info.usage                 = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    buf_info.size                  = bufferByteCount;
    buf_info.queueFamilyIndexCount = 0;
    buf_info.pQueueFamilyIndices   = NULL;
    buf_info.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
    buf_info.flags                 = 0;
    VkResult result                = vkCreateBuffer(device, &buf_info, NULL, &uniformBuf);
    assert(result == VK_SUCCESS);

    VkMemoryRequirements mem_reqs;
    vkGetBufferMemoryRequirements(device, uniformBuf, &mem_reqs);
    VkMemoryAllocateInfo alloc_info = {};
    alloc_info.sType                = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.pNext                = NULL;
    alloc_info.memoryTypeIndex      = 0;
    alloc_info.allocationSize       = mem_reqs.size;
    VkFlags requirements_mask       = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    bool flag                       = memoryTypeFromProperties(memoryroperties, mem_reqs.memoryTypeBits, requirements_mask, &alloc_info.memoryTypeIndex);
    if (flag)
    {
        printf("ȷ���ڴ����ͳɹ� ��������Ϊ%d", alloc_info.memoryTypeIndex);
    }
    else
    {
        printf("ȷ���ڴ�����ʧ��!");
    }
    result = vkAllocateMemory(device, &alloc_info, NULL, &memUniformBuf);
    assert(result == VK_SUCCESS);
    result = vkBindBufferMemory(device, uniformBuf, memUniformBuf, 0);
    assert(result == VK_SUCCESS);
    uniformBufferInfo.buffer = uniformBuf;
    uniformBufferInfo.offset = 0;
    uniformBufferInfo.range  = bufferByteCount;
}
void ShaderQueueSuit_Common::destroy_uniform_buffer(VkDevice &device)
{
    vkDestroyBuffer(device, uniformBuf, NULL);
    vkFreeMemory(device, memUniformBuf, NULL);
}
void ShaderQueueSuit_Common::create_pipeline_layout(VkDevice &device)
{
    VkDescriptorSetLayoutBinding layout_bindings[1];
    layout_bindings[0].binding                        = 0;
    layout_bindings[0].descriptorType                 = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layout_bindings[0].descriptorCount                = 1;
    layout_bindings[0].stageFlags                     = VK_SHADER_STAGE_VERTEX_BIT;
    layout_bindings[0].pImmutableSamplers             = NULL;
    VkDescriptorSetLayoutCreateInfo descriptor_layout = {};
    descriptor_layout.sType                           = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptor_layout.pNext                           = NULL;
    descriptor_layout.bindingCount                    = 1;
    descriptor_layout.pBindings                       = layout_bindings;
    descLayouts.resize(NUM_DESCRIPTOR_SETS);
    VkResult result = vkCreateDescriptorSetLayout(device, &descriptor_layout, NULL, descLayouts.data());
    assert(result == VK_SUCCESS);
    VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {};
    pPipelineLayoutCreateInfo.sType                      = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pPipelineLayoutCreateInfo.pNext                      = NULL;
    pPipelineLayoutCreateInfo.pushConstantRangeCount     = 0;
    pPipelineLayoutCreateInfo.pPushConstantRanges        = NULL;
    pPipelineLayoutCreateInfo.setLayoutCount             = NUM_DESCRIPTOR_SETS;
    pPipelineLayoutCreateInfo.pSetLayouts                = descLayouts.data();
    result                                               = vkCreatePipelineLayout(device, &pPipelineLayoutCreateInfo, NULL, &pipelineLayout);
    assert(result == VK_SUCCESS);
}
void ShaderQueueSuit_Common::destroy_pipeline_layout(VkDevice &device)
{
    for (int i = 0; i < NUM_DESCRIPTOR_SETS; i++)
    {
        vkDestroyDescriptorSetLayout(device, descLayouts[i], NULL);
    }
    vkDestroyPipelineLayout(device, pipelineLayout, NULL);
}
void ShaderQueueSuit_Common::init_descriptor_set(VkDevice &device)
{
    VkDescriptorPoolSize type_count[1];
    type_count[0].type                         = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    type_count[0].descriptorCount              = 1;
    VkDescriptorPoolCreateInfo descriptor_pool = {};
    descriptor_pool.sType                      = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptor_pool.pNext                      = NULL;
    descriptor_pool.maxSets                    = 1;
    descriptor_pool.poolSizeCount              = 1;
    descriptor_pool.pPoolSizes                 = type_count;
    VkResult result                            = vkCreateDescriptorPool(device, &descriptor_pool, NULL, &descPool);
    assert(result == VK_SUCCESS);
    std::vector<VkDescriptorSetLayout> layouts;
    layouts.push_back(descLayouts[0]);
    VkDescriptorSetAllocateInfo alloc_info[1];
    alloc_info[0].sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info[0].pNext              = NULL;
    alloc_info[0].descriptorPool     = descPool;
    alloc_info[0].descriptorSetCount = 1;
    alloc_info[0].pSetLayouts        = layouts.data();
    descSet.resize(1);
    result = vkAllocateDescriptorSets(device, alloc_info, descSet.data());
    assert(result == VK_SUCCESS);
    writes[0]                 = {};
    writes[0].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].pNext           = NULL;
    writes[0].descriptorCount = 1;
    writes[0].descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes[0].pBufferInfo     = &uniformBufferInfo;
    writes[0].dstArrayElement = 0;
    writes[0].dstBinding      = 0;
}
void ShaderQueueSuit_Common::create_shader(VkDevice &device)
{
    SpvData spvVertData = FileUtil::loadSPV(VertShaderPath); //���ض���SPV
    SpvData spvFragData = FileUtil::loadSPV(FragShaderPath); //����ƬԪSPV
    init_glslang();
    shaderStages[0].sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[0].pNext               = NULL;
    shaderStages[0].pSpecializationInfo = NULL;
    shaderStages[0].flags               = 0;
    shaderStages[0].stage               = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStages[0].pName               = "main";
    VkShaderModuleCreateInfo moduleCreateInfo; //׼��������ɫ��ģ�鴴����Ϣ
    moduleCreateInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleCreateInfo.pNext    = NULL;             //�Զ������ݵ�ָ��
    moduleCreateInfo.flags    = 0;                //������ʹ�õı�־
    moduleCreateInfo.codeSize = spvVertData.size; //������ɫ��SPV �������ֽ���
    moduleCreateInfo.pCode    = spvVertData.data; //������ɫ��SPV ����
    VkResult result           = vkCreateShaderModule(device, &moduleCreateInfo, NULL, &shaderStages[0].module);
    assert(result == VK_SUCCESS);
    shaderStages[1].sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[1].pNext               = NULL;
    shaderStages[1].pSpecializationInfo = NULL;
    shaderStages[1].flags               = 0;
    shaderStages[1].stage               = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStages[1].pName               = "main";
    moduleCreateInfo.sType              = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO; //׼��ƬԪ��ɫ��ģ�鴴����Ϣ
    moduleCreateInfo.pNext              = NULL;                                        //�Զ������ݵ�ָ��
    moduleCreateInfo.flags              = 0;                                           //������ʹ�õı�־
    moduleCreateInfo.codeSize           = spvFragData.size;                            //ƬԪ��ɫ��SPV �������ֽ���
    moduleCreateInfo.pCode              = spvFragData.data;                            //ƬԪ��ɫ��SPV ����
    result                              = vkCreateShaderModule(device, &moduleCreateInfo, NULL, &shaderStages[1].module);
    assert(result == VK_SUCCESS);
}
void ShaderQueueSuit_Common::destroy_shader(VkDevice &device)
{
    vkDestroyShaderModule(device, shaderStages[0].module, NULL);
    vkDestroyShaderModule(device, shaderStages[1].module, NULL);
}
void ShaderQueueSuit_Common::initVertexAttributeInfo()
{
    vertexBinding.binding     = 0;
    vertexBinding.inputRate   = VK_VERTEX_INPUT_RATE_VERTEX;
    vertexBinding.stride      = sizeof(float) * 6;
    vertexAttribs[0].binding  = 0;
    vertexAttribs[0].location = 0;
    vertexAttribs[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
    vertexAttribs[0].offset   = 0;
    vertexAttribs[1].binding  = 0;
    vertexAttribs[1].location = 1;
    vertexAttribs[1].format   = VK_FORMAT_R32G32B32_SFLOAT;
    vertexAttribs[1].offset   = 12;
}
void ShaderQueueSuit_Common::create_pipe_line(VkDevice &device, VkRenderPass &renderPass)
{
    VkDynamicState dynamicStateEnables[VK_DYNAMIC_STATE_RANGE_SIZE];
    memset(dynamicStateEnables, 0, sizeof dynamicStateEnables);
    VkPipelineDynamicStateCreateInfo dynamicState = {};
    dynamicState.sType                            = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.pNext                            = NULL;
    dynamicState.pDynamicStates                   = dynamicStateEnables;
    dynamicState.dynamicStateCount                = 0;
    VkPipelineVertexInputStateCreateInfo vi;
    vi.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vi.pNext                           = NULL;
    vi.flags                           = 0;
    vi.vertexBindingDescriptionCount   = 1;
    vi.pVertexBindingDescriptions      = &vertexBinding;
    vi.vertexAttributeDescriptionCount = 2;
    vi.pVertexAttributeDescriptions    = vertexAttribs;
    VkPipelineInputAssemblyStateCreateInfo ia;
    ia.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    ia.pNext                  = NULL;
    ia.flags                  = 0;
    ia.primitiveRestartEnable = VK_FALSE;
    ia.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    VkPipelineRasterizationStateCreateInfo rs;
    rs.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rs.pNext                   = NULL;
    rs.flags                   = 0;
    rs.polygonMode             = VK_POLYGON_MODE_FILL;
    rs.cullMode                = VK_CULL_MODE_NONE;
    rs.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rs.depthClampEnable        = VK_TRUE;
    rs.rasterizerDiscardEnable = VK_FALSE;
    rs.depthBiasEnable         = VK_FALSE;
    rs.depthBiasConstantFactor = 0;
    rs.depthBiasClamp          = 0;
    rs.depthBiasSlopeFactor    = 0;
    rs.lineWidth               = 1.0f;
    VkPipelineColorBlendAttachmentState att_state[1];
    att_state[0].colorWriteMask      = 0xf;
    att_state[0].blendEnable         = VK_FALSE;
    att_state[0].alphaBlendOp        = VK_BLEND_OP_ADD;
    att_state[0].colorBlendOp        = VK_BLEND_OP_ADD;
    att_state[0].srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    att_state[0].dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    att_state[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    att_state[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    VkPipelineColorBlendStateCreateInfo cb;
    cb.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    cb.pNext             = NULL;
    cb.flags             = 0;
    cb.attachmentCount   = 1;
    cb.pAttachments      = att_state;
    cb.logicOpEnable     = VK_FALSE;
    cb.logicOp           = VK_LOGIC_OP_NO_OP;
    cb.blendConstants[0] = 1.0f;
    cb.blendConstants[1] = 1.0f;
    cb.blendConstants[2] = 1.0f;
    cb.blendConstants[3] = 1.0f;
    VkViewport viewports;
    viewports.minDepth = 0.0f;
    viewports.maxDepth = 1.0f;
    viewports.x        = 0;
    viewports.y        = 0;
    viewports.width    = (float)MyVulkanManager::screenWidth;
    viewports.height   = (float)MyVulkanManager::screenHeight;
    VkRect2D scissor;
    scissor.extent.width                 = MyVulkanManager::screenWidth;
    scissor.extent.height                = MyVulkanManager::screenHeight;
    scissor.offset.x                     = 0;
    scissor.offset.y                     = 0;
    VkPipelineViewportStateCreateInfo vp = {};
    vp.sType                             = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    vp.pNext                             = NULL;
    vp.flags                             = 0;
    vp.viewportCount                     = 1;
    vp.scissorCount                      = 1;
    vp.pScissors                         = &scissor;
    vp.pViewports                        = &viewports;
    VkPipelineDepthStencilStateCreateInfo ds;
    ds.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    ds.pNext                 = NULL;
    ds.flags                 = 0;
    ds.depthTestEnable       = VK_TRUE;
    ds.depthWriteEnable      = VK_TRUE;
    ds.depthCompareOp        = VK_COMPARE_OP_LESS_OR_EQUAL;
    ds.depthBoundsTestEnable = VK_FALSE;
    ds.minDepthBounds        = 0;
    ds.maxDepthBounds        = 0;
    ds.stencilTestEnable     = VK_FALSE;
    ds.back.failOp           = VK_STENCIL_OP_KEEP;
    ds.back.passOp           = VK_STENCIL_OP_KEEP;
    ds.back.compareOp        = VK_COMPARE_OP_ALWAYS;
    ds.back.compareMask      = 0;
    ds.back.reference        = 0;
    ds.back.depthFailOp      = VK_STENCIL_OP_KEEP;
    ds.back.writeMask        = 0;
    ds.front                 = ds.back;
    VkPipelineMultisampleStateCreateInfo ms;
    ms.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    ms.pNext                 = NULL;
    ms.flags                 = 0;
    ms.pSampleMask           = NULL;
    ms.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
    ms.sampleShadingEnable   = VK_FALSE;
    ms.alphaToCoverageEnable = VK_FALSE;
    ms.alphaToOneEnable      = VK_FALSE;
    ms.minSampleShading      = 0.0;
    VkGraphicsPipelineCreateInfo pipelineInfo;
    pipelineInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.pNext               = NULL;
    pipelineInfo.layout              = pipelineLayout;
    pipelineInfo.basePipelineHandle  = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex   = 0;
    pipelineInfo.flags               = 0;
    pipelineInfo.pVertexInputState   = &vi;
    pipelineInfo.pInputAssemblyState = &ia;
    pipelineInfo.pRasterizationState = &rs;
    pipelineInfo.pColorBlendState    = &cb;
    pipelineInfo.pTessellationState  = NULL;
    pipelineInfo.pMultisampleState   = &ms;
    pipelineInfo.pDynamicState       = &dynamicState;
    pipelineInfo.pViewportState      = &vp;
    pipelineInfo.pDepthStencilState  = &ds;
    pipelineInfo.stageCount          = 2;
    pipelineInfo.pStages             = shaderStages;
    pipelineInfo.renderPass          = renderPass;
    pipelineInfo.subpass             = 0;
    VkPipelineCacheCreateInfo pipelineCacheInfo;
    pipelineCacheInfo.sType           = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    pipelineCacheInfo.pNext           = NULL;
    pipelineCacheInfo.initialDataSize = 0;
    pipelineCacheInfo.pInitialData    = NULL;
    pipelineCacheInfo.flags           = 0;
    VkResult result                   = vkCreatePipelineCache(device, &pipelineCacheInfo, NULL, &pipelineCache);
    assert(result == VK_SUCCESS);
    result = vkCreateGraphicsPipelines(device, pipelineCache, 1, &pipelineInfo, NULL, &pipeline);
    assert(result == VK_SUCCESS);
}
void ShaderQueueSuit_Common::destroy_pipe_line(VkDevice &device)
{
    vkDestroyPipeline(device, pipeline, NULL);
    vkDestroyPipelineCache(device, pipelineCache, NULL);
}
ShaderQueueSuit_Common::ShaderQueueSuit_Common(VkDevice *deviceIn, VkRenderPass &renderPass, VkPhysicalDeviceMemoryProperties &memoryroperties)
{
    this->devicePointer = deviceIn;
    create_uniform_buffer(*devicePointer, memoryroperties);
    create_pipeline_layout(*devicePointer);
    init_descriptor_set(*devicePointer);
    create_shader(*devicePointer);
    initVertexAttributeInfo();
    create_pipe_line(*devicePointer, renderPass);
}
ShaderQueueSuit_Common::~ShaderQueueSuit_Common()
{
    destroy_pipe_line(*devicePointer);
    destroy_shader(*devicePointer);
    destroy_pipeline_layout(*devicePointer);
    destroy_uniform_buffer(*devicePointer);
}
