cmake_minimum_required(VERSION 3.31)
project(VulkanEngine)

find_package(Vulkan REQUIRED)
if(NOT Vulkan_FOUND)
	message(FATAL_ERROR "Vulkan not found. Please install the Vulkan SDK.")
endif()


#########################################################

include(FetchContent)

# GLM
FetchContent_Declare(
	glm
	GIT_REPOSITORY "https://github.com/g-truc/glm.git"
	GIT_TAG "0.9.9.8"
	GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(glm)

# GLFW
FetchContent_Declare(
	glfw
	GIT_REPOSITORY "https://github.com/glfw/glfw.git"
	GIT_TAG "3.3.8"
	GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(glfw)

# Microsoft.GSL
FetchContent_Declare(
	microsoft-gsl
	GIT_REPOSITORY "https://github.com/microsoft/GSL.git"
	GIT_TAG "v4.0.0"
	GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(microsoft-gsl)

# SPDLOG
FetchContent_Declare(
	spdlog
	GIT_REPOSITORY "https://github.com/gabime/spdlog.git"
	GIT_TAG "v1.12.0"
	GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(spdlog)

# Tracy Profiler
FetchContent_Declare(
  tracy
  GIT_REPOSITORY "https://github.com/wolfpld/tracy.git"
  GIT_TAG "v0.12.0"
  GIT_SHALLOW TRUE
  GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(tracy)

#########################################################
include(cmake/Shaders.cmake)

#########################################################


file(GLOB_RECURSE VulkanEngineSources CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

add_executable(VulkanEngine ${VulkanEngineSources})
target_link_libraries(VulkanEngine PRIVATE Vulkan::Vulkan)
target_link_libraries(VulkanEngine PRIVATE glm)
target_link_libraries(VulkanEngine PRIVATE glfw)
target_link_libraries(VulkanEngine PRIVATE Microsoft.GSL::GSL)
target_link_libraries(VulkanEngine PRIVATE spdlog)
target_link_libraries(VulkanEngine PUBLIC TracyClient)

target_include_directories(VulkanEngine PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(VulkanEngine PRIVATE cxx_std_20)
target_compile_definitions(VulkanEngine PRIVATE TRACY_ENABLE TRACY_IMPORTS TRACY_ON_DEMAND TRACY_DELAYED_INIT TRACY_GPU TRACY_GPU_VULKAN)
target_precompile_headers(VulkanEngine PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/precomp.h")


file(GLOB_RECURSE ShaderSources CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.vert"
	"${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.frag"
)
add_shaders(VulkanEngineShaders ${ShaderSources})
add_dependencies(VulkanEngine VulkanEngineShaders)
