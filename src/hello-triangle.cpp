#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

static int SCR_WIDTH = 800;
static int SCR_HEIGHT = 800;

static unsigned int shader;
static unsigned int vpUniform;
static unsigned int modelUniform;

static glm::mat4 view = glm::mat4(1.0f);
static glm::mat4 model = glm::mat4(1.0f);
static glm::mat4 proj;
static glm::mat4 vp;

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
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    // Recalculate projection
    proj = glm::ortho(0.0f, (float)SCR_WIDTH, 0.0f, float(SCR_HEIGHT), -1.0f, 100.0f);
    vp = proj * view;
    glUniformMatrix4fv(vpUniform, 1, GL_FALSE, glm::value_ptr(vp));
}

int main(void)
{
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL Hello Triangle", NULL, NULL);

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
        // position x, y  // color RGB
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
   
    shader = CreateShader(source.VertexSource, source.FragmentSource);
    glUseProgram(shader);

    // Camera transformations

    proj = glm::ortho(0.0f, (float)SCR_WIDTH, 0.0f, float(SCR_HEIGHT), -1.0f, 100.0f);

    // World transformations

    view = glm::translate(view, glm::vec3(0.0f, 0.0f, 0.0f));

    // view-proj
    vp = proj * view;

    vpUniform = glGetUniformLocation(shader, "vp");
    glUniformMatrix4fv(vpUniform, 1, GL_FALSE, glm::value_ptr(vp));

    // Model transforms
    glm::vec2 size(300.0f, 300.0f);
    glm::vec2 position(400.0f, 400.0f);

    model = glm::translate(model, glm::vec3(position, 0.0f)); // Order: translation, rotation, scale
    model = glm::scale(model, glm::vec3(size, 1.0f));

    modelUniform = glGetUniformLocation(shader, "model");
    glUniformMatrix4fv(modelUniform, 1, GL_FALSE, glm::value_ptr(model));


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