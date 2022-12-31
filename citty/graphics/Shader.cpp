//
// Created by hiram on 4/21/22.
//

#include <citty/graphics/Shader.hpp>
#include <citty/graphics/OpenGlError.hpp>
#include <cstring>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <epoxy/gl.h>

graphics::Shader::Shader(const char *vertexPath, const char *fragmentPath) : vertexPath(vertexPath),
                                                                             fragmentPath(fragmentPath) {
    std::string vertexCode, fragmentCode;
    std::ifstream vShaderFile, fShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);

        std::stringstream vShaderStream, fShaderStream;

        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    } catch (std::ifstream::failure &e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();

    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);

    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void graphics::Shader::use() {
    glUseProgram(ID);
    auto error = glGetError();
    if (error) throw OpenGLError(error);
}

void graphics::Shader::setBool(const std::string &name, bool value) {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int) value);
    auto error = glGetError();
    if (error) throw OpenGLError(error);
}

void graphics::Shader::setInt(const std::string &name, int value) {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    auto error = glGetError();
    if (error) throw OpenGLError(error);
}

void graphics::Shader::setFloat(const std::string &name, float value) {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    auto error = glGetError();
    if (error) throw OpenGLError(error);
}

void graphics::Shader::setMatrix(const std::string &name, glm::mat4 value) {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    auto error = glGetError();
    if (error) throw OpenGLError(error);
}

void graphics::Shader::setVec3(const std::string &name, glm::vec3 value) {
    glUniform3f(ID, value.x, value.y, value.z);
    auto error = glGetError();
    if (error) throw OpenGLError(error);
}

void graphics::Shader::setVec4(const std::string &name, glm::vec4 value) {
    glUniform4f(ID, value.x, value.y, value.z, value.w);
    auto error = glGetError();
    if (error) throw OpenGLError(error);
}

void graphics::Shader::setVec2(const std::string &name, glm::vec2 value) {
    glUniform2f(ID, value.x, value.y);
    auto error = glGetError();
    if (error) throw OpenGLError(error);
}


