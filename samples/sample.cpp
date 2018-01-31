#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include "animation/Animation.h"
#include "animation/AnimationGroup.h"
#include "ProgramLoader.h"

static const GLuint WINDOW_WIDTH = 640;
static const GLuint WINDOW_HEIGHT = 480;

static GLFWwindow *g_window = nullptr;
static GLuint g_program = 0;
static GLuint g_MVPLocation = 0;
static GLuint g_vao = 0;
static GLuint g_vbo = 0;

std::vector<glm::vec2> centers;

class GLFWHandler
{
public:
    static void onError(int code, const char *msg)
    {
        throw std::runtime_error(msg);
    }

    static void onKey(GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
        {
            glfwSetWindowShouldClose(window, true);
            return;
        }
    }

    static void onResize(GLFWwindow *window, int width, int height)
    {
        glViewport(0, 0, width, height);
        std::cout << "Resize: " << width << ' ' << height << std::endl;
    }

};

void initialize()
{
    std::cout << "GLFW version: " << glfwGetVersionString() << std::endl;
    glfwSetErrorCallback(GLFWHandler::onError);
    if (!glfwInit()) throw std::runtime_error("glfwInit failed");
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    g_window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "animation", nullptr, nullptr);
    if (!g_window) {
        glfwTerminate();
        throw std::runtime_error("glfwCreateWindow failed");
    }
    glfwSetKeyCallback(g_window, GLFWHandler::onKey);
    glfwSetFramebufferSizeCallback(g_window, GLFWHandler::onResize);
    glfwMakeContextCurrent(g_window);
    //glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        glfwTerminate();
        throw std::runtime_error("glewInit failed");
    }
    glfwSwapInterval(1);
    int framewidth = 0;
    int frameheight = 0;
    glfwGetFramebufferSize(g_window, &framewidth, &frameheight);
    GLFWHandler::onResize(g_window, framewidth, frameheight);
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Graphics engine: " << glGetString(GL_RENDERER) << std::endl;
}

void setup()
{
    g_program = ProgramLoader::load("shader/animation.vert", "shader/animation.frag");
    g_MVPLocation = glGetUniformLocation(g_program, "MVP");
    glUseProgram(g_program);
    std::cout << "Program: " << g_program << std::endl;
    std::cout << "mvp: " << g_MVPLocation << std::endl;

    //static const GLfloat vertices[] = {
    //    -1.0, -1.0,
    //    1.0, -1.0,
    //    1.0, 1.0,
    //    -1.0, 1.0,
    //};
    static const GLfloat vertices[] = {
        -0.1, -0.1,
        0.1, -0.1,
        0.1, 0.1,
        -0.1, 0.1,
    };

    glGenVertexArrays(1, &g_vao);
    glBindVertexArray(g_vao);
    glGenBuffers(1, &g_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    std::cout << "VAO: " << g_vao << std::endl;
    std::cout << "VBO: " << g_vbo << std::endl;

    glClearColor(0, 0, 0, 1);
    //glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LESS);
    //glEnable(GL_CULL_FACE);
}

void finalize()
{
    glDeleteProgram(g_program);
    glfwDestroyWindow(g_window);
    glfwTerminate();
}

animation::Animation* setupAnimation()
{
    static const GLfloat border = 0.2;
    static const GLfloat gap = 0.02;
    static const int count = 6;
    static const GLfloat side = (2.0 - border * 2 - (count - 1) * gap) / count;
    assert(side > 0);
    std::cout << "border=" << border << ' ' << "gap=" << gap << ' ' << "side=" << side << ' ' << "count=" << count << std::endl;

    centers.clear();
    for (int i = 0; i < count; i++)
    {
        centers.emplace_back(-1.0 + border + (gap + side) * i, -1.0 + border);
    }
    animation::SequentialAnimationGroup *group = new animation::SequentialAnimationGroup;
    {
        animation::SequentialAnimationGroup *sub_group = new animation::SequentialAnimationGroup;
        sub_group->add(new animation::PropertyAnimation<GLfloat>(
                    centers[0].y, -1.0 + border, 1.0 - border, 2000,
                    new animation::LinearInterpolator));
        sub_group->add(new animation::PropertyAnimation<GLfloat>(
                    centers[1].y, -1.0 + border, 1.0 - border, 2000,
                    new animation::PowerInterpolator));
        sub_group->add(new animation::PropertyAnimation<GLfloat>(
                    centers[2].y, -1.0 + border, 1.0 - border, 2000,
                    new animation::InverseInterpolator(new animation::BackInterpolator)));
        sub_group->add(new animation::PropertyAnimation<GLfloat>(
                    centers[3].y, -1.0 + border, 1.0 - border, 2000,
                    new animation::BackInterpolator));
        sub_group->add(new animation::PropertyAnimation<GLfloat>(
                    centers[4].y, -1.0 + border, 1.0 - border, 2000,
                    new animation::SineInterpolator));
        sub_group->add(new animation::PropertyAnimation<GLfloat>(
                    centers[5].y, -1.0 + border, 1.0 - border, 2000,
                    new animation::ExponentialInterpolator));
        group->add(dynamic_cast<animation::Animation*>(sub_group));
    }
    {
        animation::ParallelAnimationGroup *sub_group = new animation::ParallelAnimationGroup;
        sub_group->add(new animation::PropertyAnimation<GLfloat>(
                    centers[0].y, -1.0 + border, 1.0 - border, 2000,
                    new animation::LinearInterpolator));
        sub_group->add(new animation::PropertyAnimation<GLfloat>(
                    centers[1].y, -1.0 + border, 1.0 - border, 2000,
                    new animation::PowerInterpolator));
        sub_group->add(new animation::PropertyAnimation<GLfloat>(
                    centers[2].y, -1.0 + border, 1.0 - border, 2000,
                    new animation::InverseInterpolator(new animation::BackInterpolator)));
        sub_group->add(new animation::PropertyAnimation<GLfloat>(
                    centers[3].y, -1.0 + border, 1.0 - border, 2000,
                    new animation::BackInterpolator));
        sub_group->add(new animation::PropertyAnimation<GLfloat>(
                    centers[4].y, -1.0 + border, 1.0 - border, 2000,
                    new animation::SineInterpolator));
        sub_group->add(new animation::PropertyAnimation<GLfloat>(
                    centers[5].y, -1.0 + border, 1.0 - border, 2000,
                    new animation::ExponentialInterpolator));
        group->add(dynamic_cast<animation::Animation*>(sub_group));
    }
    return dynamic_cast<animation::Animation*>(group);
}

void render()
{
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT);

    static animation::Animation *animation = nullptr;
    if (!animation) 
    {
        animation = setupAnimation();
        animation->start();
    }
    if (!animation->animate()) {
        delete animation;
        animation = nullptr;
    }

    //static int count = 0;
    //std::cout << "loop #" << ++count << std::endl;
    //for (auto center: centers) {
    //    std::cout << glm::to_string(center) << std::endl;
    //}

    int framewidth = 0;
    int frameheight = 0;
    glfwGetFramebufferSize(g_window, &framewidth, &frameheight);
    GLfloat ratio = (GLfloat)framewidth / frameheight;
    glm::mat4 mProjection;
    if (ratio > 1) {
        mProjection = glm::ortho(-ratio, ratio, -1.0f, 1.0f);
    } else {
        ratio = 1.0 / ratio;
        mProjection = glm::ortho(-1.0f, 1.0f, -ratio, ratio);
    }
    glm::vec3 eye(0, 0, 1);
    glm::vec3 center(0, 0, 0);
    glm::vec3 up(0, 1, 0);
    glm::mat4 mView = glm::lookAt(eye, center, up);

    for (auto center: centers) {
        glm::mat4 mModel = glm::translate(glm::mat4(1), glm::vec3(center, 0));
        glm::mat4 mvp = mProjection * mView * mModel;
        glUniformMatrix4fv(g_MVPLocation, 1, GL_FALSE, glm::value_ptr(mvp));
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        //glDrawElements(GL_TRIANGLES, sizeof() / sizeof(GLushort), GL_UNSIGNED_SHORT, nullptr);
    }

    //glm::mat4 mvp = mProjection * mView;
    //glUniformMatrix4fv(g_MVPLocation, 1, GL_FALSE, glm::value_ptr(mvp));
    //glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

int main(int argc, char *argv[])
{
    initialize();
    setup();
    while (!glfwWindowShouldClose(g_window)) {
        render();
        glfwSwapBuffers(g_window);
        glfwPollEvents();
    }
    finalize();
    return 0;
}
