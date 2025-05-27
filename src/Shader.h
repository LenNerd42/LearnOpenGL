#pragma once

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/fwd.hpp>

class Shader
{
public:
    // The shader program ID.
    unsigned int ID;

    // Constructor that reads and builds the shaders.
    Shader(const char* vertexPath, const char* fragmentPath);

    // Activate the shader.
    void use() const;

    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setMat4(const std::string& name, const glm::mat4& value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
};
