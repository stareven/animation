#include "ProgramLoader.h"
#include <iostream>
#include <fstream>
#include <sstream>

GLuint ProgramLoader::loadShader(GLenum shaderType, const std::string &path)
{
    std::ifstream f(path.c_str(), std::ios::in | std::ios::binary);
    if (!f) throw std::runtime_error(std::string("Can't open shader file ") + path);
    std::stringstream buffer;
    buffer << f.rdbuf();
    GLuint shader = glCreateShader(shaderType);
    if (!shader) throw std::runtime_error(std::string("Can't create shader for file ") + path);
    const std::string &bufferAsString = buffer.str();
    const GLchar *shaderCode = bufferAsString.c_str();
    const GLchar *codeArray[] = { shaderCode };
    glShaderSource(shader, 1, codeArray, NULL);
    glCompileShader(shader);
    GLint status = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (!status) {
        std::cerr << "Compilation error in shader file " << path << std::endl;
        GLint logLen;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
        if (logLen > 0) {
            char *log = new char[logLen];
            GLsizei written;
            glGetShaderInfoLog(shader, logLen, &written, log);
            std::cerr << "Shader log: " << std::endl;
            std::cerr << log << std::endl;
            delete[] log;
        }
        throw std::runtime_error(
                std::string("Can't compile the shader defined in file ") + path);
    }
    return shader;
}

GLuint ProgramLoader::load(const std::string &vsFilename, const std::string &fsFilename)
{
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vsFilename);
    GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fsFilename);
    GLuint program = glCreateProgram();
    if (!program) throw std::runtime_error("Can't create GLSL program.");
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    GLint status = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (!status) {
        std::cerr << "Linking error in shader program!" << std::endl;
        GLint logLen = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);
        if (logLen > 0) {
            char *log = new char[logLen];
            GLsizei written;
            glGetProgramInfoLog(program, logLen, &written, log);
            std::cerr << "Shader log: " << std::endl;
            std::cerr << log << std::endl;
            delete [] log;
        }
        throw std::runtime_error("Can't link shader program.");
    }
    return program;
}
