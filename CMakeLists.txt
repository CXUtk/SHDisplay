cmake_minimum_required (VERSION 3.8)

project (SHDisplay VERSION 1.0)

add_executable (SHDisplay "SHDisplay.h" 
        "SHDisplay.cpp" 
        "Canvas.h"
        "Canvas.cpp"
        "Renderer.h"
        "Renderer.cpp"
        "Shader.h"
        "ShaderLoader.h"
        "ShaderLoader.cpp"
         "ObjLoader.cpp"
         "ObjLoader.h"
         "Primitive.h"
         "TriangleMesh.h"
         "TriangleMesh.cpp"
         "EnvironmentMap.h"
         "EnvironmentMap.cpp"
         "Camera.h"
         "Camera.cpp")

set_target_properties (SHDisplay PROPERTIES
                      CXX_STANDARD 17
                      CXX_EXTENSIONS OFF
                      )

include_directories (${PROJECT_SOURCE_DIR}/deps)

include_directories (${PROJECT_SOURCE_DIR}/deps/GLFW/include)

if (UNIX)
    FIND_PACKAGE (glfw3)
    if (glfw3_FOUND)
        target_link_libraries(SHDisplay PRIVATE glfw)
    else()
        add_subdirectory("${PROJECT_SOURCE_DIR}/deps/GLFW")
        target_link_libraries(SHDisplay PRIVATE glfw)
    endif()
    target_link_libraries (SHDisplay PRIVATE dl X11 pthread)
else()
    add_subdirectory("${PROJECT_SOURCE_DIR}/deps/GLFW")
    target_link_libraries(SHDisplay PRIVATE glfw)
endif()

find_package(OpenGL REQUIRED)
target_link_libraries(SHDisplay PRIVATE OpenGL::GL)

add_subdirectory ("deps/imgui")
add_subdirectory ("deps/glad")

target_link_libraries (SHDisplay PRIVATE glad imgui)