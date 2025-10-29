cmake_minimum_required(VERSION 3.10)
project(opengltest2 LANGUAGES C CXX)

# C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 目录变量
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(INC_DIR ${CMAKE_SOURCE_DIR}/include)

# 依赖
find_package(OpenGL REQUIRED) # 提供 OpenGL::GL
find_package(PkgConfig QUIET)

# 优先使用 CMake 的 glfw3 包，若不可用则回退到 pkg-config
find_package(glfw3 QUIET CONFIG)

# 源文件
set(SOURCES
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/shader.cpp
    ${SRC_DIR}/loadobj.cpp
    ${SRC_DIR}/glad.c
)

add_executable(${PROJECT_NAME} ${SOURCES})

# 头文件目录
target_include_directories(${PROJECT_NAME} PRIVATE ${INC_DIR})

# 将 Shader 源目录作为宏传入，便于运行时加载
target_compile_definitions(${PROJECT_NAME} PRIVATE SHADER_DIR="${SRC_DIR}/shader")
target_compile_definitions(${PROJECT_NAME} PRIVATE TEST_DIR="${CMAKE_SOURCE_DIR}/testcase")

# 链接 OpenGL
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)

# 链接 GLFW（两种方式择优其一）
if(glfw3_FOUND)
    # 使用 CMake 提供的目标名（大多数发行版命名为 glfw 或 glfw3）
    # 这里尝试两种常见目标名，存在则链接
    if(TARGET glfw)
        target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
    elseif(TARGET glfw3)
        target_link_libraries(${PROJECT_NAME} PRIVATE glfw3)
    else()
        message(FATAL_ERROR "glfw3 CMake 包已找到但未暴露可链接目标")
    endif()
else()
    if(PkgConfig_FOUND)
        pkg_check_modules(GLFW3 REQUIRED glfw3)
        target_include_directories(${PROJECT_NAME} PRIVATE ${GLFW3_INCLUDE_DIRS})
        target_link_libraries(${PROJECT_NAME} PRIVATE ${GLFW3_LIBRARIES})
        target_compile_options(${PROJECT_NAME} PRIVATE ${GLFW3_CFLAGS_OTHER})
    else()
        message(FATAL_ERROR "未找到 GLFW3，请安装 glfw3 或提供其 CMake 配置/pc 文件")
    endif()
endif()

# Linux 额外库
if(UNIX AND NOT APPLE)
    find_package(Threads REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads dl)
endif()

# Debug 宏（可选）
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)
endif()

# 安装规则（可选）
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)