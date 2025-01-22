# Add source files
add_executable(Renderer3D
        main.cpp
        camera.cpp
        camera.h
        shader.cpp
        shader.h
        texture.cpp
        texture.h
        mesh.cpp
        mesh.h
        model.cpp
        model.h
        window.cpp
        window.h
        renderer.cpp
        renderer.h
        deferred_shaderer.cpp
        deferred_shaderer.h
        point_light_source.cpp
        point_light_source.h
        point_lights_container.cpp
        point_lights_container.h
        entity.cpp
        entity.h
        scene.cpp
        scene.h
        floor.cpp
        floor.h
        skybox.cpp
        skybox.h
        controls.cpp
        controls.h
        models_manager.cpp
        models_manager.h
        spot_light_source.cpp
        spot_light_source.h
        spot_lights_factory.cpp
        spot_lights_factory.h
)

# Link libraries
target_link_libraries(Renderer3D PRIVATE glfw glad stb_image glm assimp spdlog::spdlog imgui)
