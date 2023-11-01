#include <stdio.h>
#include <assert.h>
#include <cstdlib>
#include <iomanip>
#include <fstream>
#include <iostream>
#include "util.hpp"
#include "SPIRV/GlslangToSpv.h"
#include <Windows.h>
#include "../main_task/MyVulkanManager.h"
using namespace std;
int preX           = 0;
int preY           = 0;
bool mouseLeftDown = false;
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    struct WindowInfo *info = reinterpret_cast<struct WindowInfo *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    switch (uMsg)
    {
        case WM_CLOSE: PostQuitMessage(0); break;
        case WM_PAINT: break;
        case WM_LBUTTONDOWN:
            preX          = LOWORD(lParam);
            preY          = HIWORD(lParam);
            mouseLeftDown = true;
            break;
        case WM_LBUTTONUP: mouseLeftDown = false; break;
        case WM_MOUSEMOVE:
        {
            if (mouseLeftDown)
            {
                int x                   = LOWORD(lParam);
                int y                   = HIWORD(lParam);
                float xDis              = (float)(x - preX);
                float yDis              = (float)(y - preY);
                MyVulkanManager::yAngle = MyVulkanManager::yAngle + xDis * 180 / 200;
                MyVulkanManager::zAngle = MyVulkanManager::zAngle + yDis * 180 / 200;
                preX                    = x;
                preY                    = y;
            }
            break;
        }
        default: break;
    }
    return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}
void init_window(struct WindowInfo &info, int32_t default_width, int32_t default_height, HINSTANCE hInstance)
{
    info.width  = default_width;
    info.height = default_height;
    assert(info.width > 0);
    assert(info.height > 0);
    sprintf(info.name, "VulkanBase NCST BNKJ");
    WNDCLASSEX win_class;
    win_class.cbSize        = sizeof(WNDCLASSEX);
    win_class.style         = CS_HREDRAW | CS_VREDRAW;
    win_class.lpfnWndProc   = WndProc;
    win_class.cbClsExtra    = 0;
    win_class.cbWndExtra    = 0;
    win_class.hInstance     = hInstance;
    win_class.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    win_class.hCursor       = LoadCursor(NULL, IDC_ARROW);
    win_class.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    win_class.lpszMenuName  = NULL;
    win_class.lpszClassName = info.name;
    win_class.hIconSm       = LoadIcon(NULL, IDI_WINLOGO);
    if (!RegisterClassEx(&win_class))
    {
        printf("Unexpected error trying to start the application!\n");
        fflush(stdout);
        exit(1);
    }
    RECT wr = {0, 0, info.width, info.height};
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
    info.window = CreateWindowEx(0, info.name, info.name, WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_SYSMENU, 100, 100, wr.right - wr.left, wr.bottom - wr.top, NULL, NULL, hInstance, &info);
    if (!info.window)
    {
        printf("Cannot create a window in which to draw!\n");
        fflush(stdout);
        exit(1);
    }
    SetWindowLongPtr(info.window, GWLP_USERDATA, (LONG_PTR)&info);
    ShowWindow(info.window, SW_SHOWNA);
    UpdateWindow(info.window);
}
void destroy_window(struct WindowInfo &info)
{
    DestroyWindow(info.window);
}
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    init_window(MyVulkanManager::info, 1366, 768, hPrevInstance);
    sample_main();
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}
int main(int argc, char **argv)
{
    init_window(MyVulkanManager::info, 1366, 768, GetModuleHandle(NULL));
    sample_main();
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}
void init_resources(TBuiltInResource &Resources)
{
    Resources.maxLights                                   = 32;
    Resources.maxClipPlanes                               = 6;
    Resources.maxTextureUnits                             = 32;
    Resources.maxTextureCoords                            = 32;
    Resources.maxVertexAttribs                            = 64;
    Resources.maxVertexUniformComponents                  = 4096;
    Resources.maxVaryingFloats                            = 64;
    Resources.maxVertexTextureImageUnits                  = 32;
    Resources.maxCombinedTextureImageUnits                = 80;
    Resources.maxTextureImageUnits                        = 32;
    Resources.maxFragmentUniformComponents                = 4096;
    Resources.maxDrawBuffers                              = 32;
    Resources.maxVertexUniformVectors                     = 128;
    Resources.maxVaryingVectors                           = 8;
    Resources.maxFragmentUniformVectors                   = 16;
    Resources.maxVertexOutputVectors                      = 16;
    Resources.maxFragmentInputVectors                     = 15;
    Resources.minProgramTexelOffset                       = -8;
    Resources.maxProgramTexelOffset                       = 7;
    Resources.maxClipDistances                            = 8;
    Resources.maxComputeWorkGroupCountX                   = 65535;
    Resources.maxComputeWorkGroupCountY                   = 65535;
    Resources.maxComputeWorkGroupCountZ                   = 65535;
    Resources.maxComputeWorkGroupSizeX                    = 1024;
    Resources.maxComputeWorkGroupSizeY                    = 1024;
    Resources.maxComputeWorkGroupSizeZ                    = 64;
    Resources.maxComputeUniformComponents                 = 1024;
    Resources.maxComputeTextureImageUnits                 = 16;
    Resources.maxComputeImageUniforms                     = 8;
    Resources.maxComputeAtomicCounters                    = 8;
    Resources.maxComputeAtomicCounterBuffers              = 1;
    Resources.maxVaryingComponents                        = 60;
    Resources.maxVertexOutputComponents                   = 64;
    Resources.maxGeometryInputComponents                  = 64;
    Resources.maxGeometryOutputComponents                 = 128;
    Resources.maxFragmentInputComponents                  = 128;
    Resources.maxImageUnits                               = 8;
    Resources.maxCombinedImageUnitsAndFragmentOutputs     = 8;
    Resources.maxCombinedShaderOutputResources            = 8;
    Resources.maxImageSamples                             = 0;
    Resources.maxVertexImageUniforms                      = 0;
    Resources.maxTessControlImageUniforms                 = 0;
    Resources.maxTessEvaluationImageUniforms              = 0;
    Resources.maxGeometryImageUniforms                    = 0;
    Resources.maxFragmentImageUniforms                    = 8;
    Resources.maxCombinedImageUniforms                    = 8;
    Resources.maxGeometryTextureImageUnits                = 16;
    Resources.maxGeometryOutputVertices                   = 256;
    Resources.maxGeometryTotalOutputComponents            = 1024;
    Resources.maxGeometryUniformComponents                = 1024;
    Resources.maxGeometryVaryingComponents                = 64;
    Resources.maxTessControlInputComponents               = 128;
    Resources.maxTessControlOutputComponents              = 128;
    Resources.maxTessControlTextureImageUnits             = 16;
    Resources.maxTessControlUniformComponents             = 1024;
    Resources.maxTessControlTotalOutputComponents         = 4096;
    Resources.maxTessEvaluationInputComponents            = 128;
    Resources.maxTessEvaluationOutputComponents           = 128;
    Resources.maxTessEvaluationTextureImageUnits          = 16;
    Resources.maxTessEvaluationUniformComponents          = 1024;
    Resources.maxTessPatchComponents                      = 120;
    Resources.maxPatchVertices                            = 32;
    Resources.maxTessGenLevel                             = 64;
    Resources.maxViewports                                = 16;
    Resources.maxVertexAtomicCounters                     = 0;
    Resources.maxTessControlAtomicCounters                = 0;
    Resources.maxTessEvaluationAtomicCounters             = 0;
    Resources.maxGeometryAtomicCounters                   = 0;
    Resources.maxFragmentAtomicCounters                   = 8;
    Resources.maxCombinedAtomicCounters                   = 8;
    Resources.maxAtomicCounterBindings                    = 1;
    Resources.maxVertexAtomicCounterBuffers               = 0;
    Resources.maxTessControlAtomicCounterBuffers          = 0;
    Resources.maxTessEvaluationAtomicCounterBuffers       = 0;
    Resources.maxGeometryAtomicCounterBuffers             = 0;
    Resources.maxFragmentAtomicCounterBuffers             = 1;
    Resources.maxCombinedAtomicCounterBuffers             = 1;
    Resources.maxAtomicCounterBufferSize                  = 16384;
    Resources.maxTransformFeedbackBuffers                 = 4;
    Resources.maxTransformFeedbackInterleavedComponents   = 64;
    Resources.maxCullDistances                            = 8;
    Resources.maxCombinedClipAndCullDistances             = 8;
    Resources.maxSamples                                  = 4;
    Resources.limits.nonInductiveForLoops                 = 1;
    Resources.limits.whileLoops                           = 1;
    Resources.limits.doWhileLoops                         = 1;
    Resources.limits.generalUniformIndexing               = 1;
    Resources.limits.generalAttributeMatrixVectorIndexing = 1;
    Resources.limits.generalVaryingIndexing               = 1;
    Resources.limits.generalSamplerIndexing               = 1;
    Resources.limits.generalVariableIndexing              = 1;
    Resources.limits.generalConstantMatrixVectorIndexing  = 1;
}
EShLanguage FindLanguage(const VkShaderStageFlagBits shader_type)
{
    switch (shader_type)
    {
        case VK_SHADER_STAGE_VERTEX_BIT: return EShLangVertex;
        case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT: return EShLangTessControl;
        case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT: return EShLangTessEvaluation;
        case VK_SHADER_STAGE_GEOMETRY_BIT: return EShLangGeometry;
        case VK_SHADER_STAGE_FRAGMENT_BIT: return EShLangFragment;
        case VK_SHADER_STAGE_COMPUTE_BIT: return EShLangCompute;
        default: return EShLangVertex;
    }
}
void init_glslang()
{
    glslang::InitializeProcess();
}
void finalize_glslang()
{
    glslang::FinalizeProcess();
}
void wait_seconds(int seconds)
{
    Sleep(seconds * 1000);
}
timestamp_t get_milliseconds()
{
    LARGE_INTEGER frequency;
    BOOL useQPC = QueryPerformanceFrequency(&frequency);
    if (useQPC)
    {
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        return (1000LL * now.QuadPart) / frequency.QuadPart;
    }
    else
    {
        return GetTickCount();
    }
}
