#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#ifdef _WIN32
#pragma comment(linker, "/subsystem:console")
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#define APP_NAME_STR_LEN 80
#endif
#include <vulkan/vulkan.h>
#include "SPIRV/GlslangToSpv.h"
#define NUM_DESCRIPTOR_SETS 1
#define NUM_SAMPLES VK_SAMPLE_COUNT_1_BIT
#define NUM_VIEWPORTS 1
#define NUM_SCISSORS NUM_VIEWPORTS
#define FENCE_TIMEOUT 100000000
/*
窗体信息对象
*/
struct WindowInfo
{
#define APP_NAME_STR_LEN 80
    char name[APP_NAME_STR_LEN];
    HWND window;
    int width, height;
};
void init_window(struct WindowInfo &info, int32_t default_width, int32_t default_height);
void destroy_window(struct WindowInfo &info);
void init_glslang();
void finalize_glslang();
void wait_seconds(int seconds);
void print_UUID(uint8_t *pipelineCacheUUID);
std::string get_file_directory();
typedef unsigned long long timestamp_t;
timestamp_t get_milliseconds();
int sample_main();
void init_resources(TBuiltInResource &Resources);
EShLanguage FindLanguage(const VkShaderStageFlagBits shader_type);
void init_glslang();
void finalize_glslang();
timestamp_t get_milliseconds();
void wait_seconds(int seconds);
