#pragma once

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

class ProgramLoader
{
public:
    static GLuint load(const std::string &vsFilename, const std::string &fsFilename);

private:
    static GLuint loadShader(GLenum shaderType, const std::string &path);

};
