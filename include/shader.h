#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <unordered_map>

class Shader {
public:
    unsigned int ID;

    // Construct a shader program from vertex and fragment shader file paths
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();

    // Activate the shader program
    void use() const;

    // Utility uniform functions (minimal set)
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;
    void setVec3(const std::string &name, const glm::vec3 &vec) const;

private:
    static std::string readFile(const std::string& path);
    static unsigned int compileShader(GLenum type, const std::string& source);
};

#endif // SHADER_H
