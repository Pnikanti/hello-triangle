﻿#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <glew.h>
#include <glfw3.h>

struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

enum class ShaderType
{
    NONE = -1, VERTEX = 0, FRAGMENT = 1
};

static ShaderProgramSource ParseShaders(const std::string& vertexFile, const std::string& fragmentFile)
{
    std::ifstream vertexStream(vertexFile);
    std::ifstream fragmentStream(fragmentFile);

    std::string line;
    std::stringstream ss[2];

    while (getline(vertexStream, line))
    {
        ss[(int)ShaderType::VERTEX] << line << '\n';
    }

    while (getline(fragmentStream, line))
    {
        ss[(int)ShaderType::FRAGMENT] << line << '\n';
    }

    return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;

}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebufferSizeCb(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main(void)
{
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(640, 480, "OpenGL Hello Triangle", NULL, NULL);

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCb);

    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    GLenum err = glewInit();

    if (!err == GLEW_OK)
    {
        std::cout << "Glew Error: " << glewGetErrorString(err) << std::endl;
    }

    std::cout << "Glew Version: " << glewGetString(GLEW_VERSION) << std::endl;

    float vertices[] = {
       -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
        0.0f,  0.5f,  0.0f, 1.0f, 0.0f,
        0.5f, -0.5f,  0.0f, 0.0f, 1.0f
    };

    unsigned int vbo, vao;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    ShaderProgramSource source = ParseShaders("res/shaders/vertex.shader", "res/shaders/fragment.shader");
   
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    glUseProgram(shader);

    while (!glfwWindowShouldClose(window))
    {

        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteProgram(shader);
    glfwTerminate();
    return 0;
}