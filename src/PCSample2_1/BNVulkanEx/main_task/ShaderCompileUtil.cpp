#include "ShaderCompileUtil.h"
#include "SPIRV/GlslangToSpv.h"
#include <Windows.h>
#include "..\legencyUtil\util.hpp"
struct shader_type_mapping
{
    VkShaderStageFlagBits vkshader_type;
    shaderc_shader_kind shaderc_type;
};
static const shader_type_mapping shader_map_table[] = {
        {VK_SHADER_STAGE_VERTEX_BIT, shaderc_glsl_vertex_shader},
        {VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT, shaderc_glsl_tess_control_shader},
        {VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, shaderc_glsl_tess_evaluation_shader},
        {VK_SHADER_STAGE_GEOMETRY_BIT, shaderc_glsl_geometry_shader},
        {VK_SHADER_STAGE_FRAGMENT_BIT, shaderc_glsl_fragment_shader},
        {VK_SHADER_STAGE_COMPUTE_BIT, shaderc_glsl_compute_shader},
};
shaderc_shader_kind MapShadercType(VkShaderStageFlagBits vkShader)
{
    for (auto shader : shader_map_table)
    {
        if (shader.vkshader_type == vkShader)
        {
            return shader.shaderc_type;
        }
    }
    return shaderc_glsl_infer_from_source;
}
bool GLSLtoSPV(const VkShaderStageFlagBits shader_type, const char *pshader, std::vector<unsigned int> &spirv)
{
    EShLanguage stage = FindLanguage(shader_type);
    glslang::TShader shader(stage);
    glslang::TProgram program;
    const char *shaderStrings[1];
    TBuiltInResource Resources;
    init_resources(Resources);
    EShMessages messages = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules);
    shaderStrings[0]     = pshader;
    shader.setStrings(shaderStrings, 1);
    if (!shader.parse(&Resources, 100, false, messages))
    {
        puts(shader.getInfoLog());
        puts(shader.getInfoDebugLog());
        return false;
    }
    program.addShader(&shader);
    if (!program.link(messages))
    {
        puts(shader.getInfoLog());
        puts(shader.getInfoDebugLog());
        fflush(stdout);
        return false;
    }
    glslang::GlslangToSpv(*program.getIntermediate(stage), spirv);
    return true;
}
