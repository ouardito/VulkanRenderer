#pragma once
// STD
#include <cstdint>
#include <cstdlib>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>
#include <array>
#include <set>
#include <functional>
#include <optional>

// Vendor
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <gsl/gsl>
// #define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE // Uncomment for trace level logging
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h> // For color formatting

// User Defined
#include <utilities.h>

// MACROS
#define NULL_STRUCT {}
