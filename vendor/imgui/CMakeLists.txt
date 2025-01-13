add_library(imgui
        imgui/imgui.cpp
        imgui/imgui_demo.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
        imgui/backends/imgui_impl_glfw.cpp
        imgui/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
        ${CMAKE_SOURCE_DIR}/vendor/imgui/imgui
        ${CMAKE_SOURCE_DIR}/vendor/imgui/imgui/backends
)

# ImGui needs these libraries
target_link_libraries(imgui PUBLIC glfw glad)