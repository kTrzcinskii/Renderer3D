//
// Created by Kacper Trzciński on 18.01.2025.
//

#ifndef FLOOR_H
#define FLOOR_H

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include "texture.h"

namespace Renderer3D {

class Floor {
public:
    Floor();
    Floor(Floor&& other) noexcept;
    ~Floor();
    void Draw(const std::shared_ptr<Shader>& shader) const;
private:
    GLuint _vaoId = 0;
    GLuint _vboId = 0;
    GLuint _eboId = 0;
    bool _isMoved = false;
    Texture _texture = Texture("../assets/textures/grass.jpg", TextureType::DIFFUSE);
    glm::mat4 _model = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.01f, 0.0f)), glm::vec3(Floor::SCALE_FACTOR * 1.0f, 1.0f, Floor::SCALE_FACTOR * 1.0f));;

    // Consts
    static constexpr unsigned int SCALE_FACTOR = 1000;
    static constexpr float FLOOR_VERTICES[] = {
        // positions            // normals           // texture coords
        -1.0f, 0.0f, -1.0f,     0.0f, 1.0f, 0.0f,    0.0f, 0.0f,
         1.0f, 0.0f, -1.0f,     0.0f, 1.0f, 0.0f,    Floor::SCALE_FACTOR * 1.0f, 0.0f,
         1.0f, 0.0f,  1.0f,     0.0f, 1.0f, 0.0f,    Floor::SCALE_FACTOR * 1.0f, Floor::SCALE_FACTOR * 1.0f,
        -1.0f, 0.0f,  1.0f,     0.0f, 1.0f, 0.0f,    0.0f, Floor::SCALE_FACTOR * 1.0f
    };
    static constexpr unsigned int FLOOR_INDICES[] = {
        0, 1, 2,  // first triangle
        0, 2, 3   // second triangle
    };
};

} // Renderer3D

#endif //FLOOR_H
