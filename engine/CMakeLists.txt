# bgfxの実行時自動選択を前提とし、CMakeLists.txtを簡素化
# 方針:
# - bgfx/bimg/bxのFetchContentはbgfx.cmakeに集約
# - OS分岐(if/else)を全廃
# - レンダリングAPI(Metal/OpenGL等)の切り替えをCMakeで行わない
# - bgfx::Init(type=Count)による実行時選択を前提とする
cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_OBJCXX_STANDARD 20)
set(CMAKE_OBJCXX_STANDARD_REQUIRED ON)


project(nyanthu_engine LANGUAGES CXX OBJCXX)

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

include(FetchContent)

FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG        0.9.9.8
)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(
  flecs
  GIT_REPOSITORY https://github.com/SanderMertens/flecs.git
  GIT_TAG        v3.2.5
)
FetchContent_MakeAvailable(flecs)

# --- 削除 ---
# 理由: BGFX_BACKEND変数の設定を削除。
# bgfx::Init()での実行時自動選択を前提とするため、CMake側でのバックエンド固定は不要。
# if(APPLE)
#     set(BGFX_BACKEND "metal" CACHE STRING "bgfx rendering backend")
# else()
#     set(BGFX_BACKEND "opengl" CACHE STRING "bgfx rendering backend")
# endif()

FetchContent_Declare(
  bgfx
  GIT_REPOSITORY https://github.com/bkaradzic/bgfx.cmake.git
  GIT_TAG master
)
set(BGFX_BUILD_EXAMPLES OFF CACHE BOOL "Build bgfx examples")
set(BGFX_BUILD_TOOLS ON CACHE BOOL "Build bgfx tools")
set(BGFX_INSTALL OFF CACHE BOOL "Enable bgfx installation")
FetchContent_MakeAvailable(bgfx)

find_package(Threads REQUIRED)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)

# Engine Library
add_library(nyanthu_engine
    engine/src/engine.cpp
    engine/src/ecs_flecs.cpp
    engine/src/audio_miniaudio.cpp
    engine/src/physics_jolt.cpp
    engine/src/mesh.cpp
    engine/src/renderer_bgfx.cpp
)

if(APPLE)
    target_sources(nyanthu_engine PRIVATE
        engine/src/platform/platform_utils_macos.mm
    )
elseif(UNIX AND NOT APPLE)
    target_sources(nyanthu_engine PRIVATE
        engine/src/platform/platform_utils_linux.cpp
    )
elseif(WIN32)
    target_sources(nyanthu_engine PRIVATE
        engine/src/platform/platform_utils_windows.cpp
    )
endif()



# --- 削除 ---
# 理由: BGFX_BACKEND変数に基づく分岐を削除。
# プラットフォーム固有のソースファイルやフレームワークのリンクは、
# bgfx.cmakeが内部で適切に処理するため、engineのCMakeLists.txtで管理する必要はない。
# if(BGFX_BACKEND STREQUAL "metal") ... endif()

target_include_directories(nyanthu_engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/include
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/include/external
    # ${bgfx_SOURCE_DIR}/bgfx/include # --- 削除(bgfxターゲットが自動で追加)
    # ${bgfx_SOURCE_DIR}/bimg/include # --- 削除(bgfxターゲットが自動で追加)
    # ${bgfx_SOURCE_DIR}/bx/include   # --- 削除(bgfxターゲットが自動で追加)
    ${glm_SOURCE_DIR}
    ${flecs_SOURCE_DIR}
    ${glfw_SOURCE_DIR}/include
)

target_link_libraries(nyanthu_engine
    PRIVATE
        bgfx # bimgとbxはbgfxターゲットに集約されているため不要
        glfw
        flecs
        Threads::Threads
        # m # --- 削除(多くのLinux環境で不要/必要ならbgfx.cmakeが処理)
)
# --- 削除 ---
# 理由: プラットフォーム固有のフレームワークリンクもbgfxターゲットが内部で処理する。
# if(APPLE) ... endif()
