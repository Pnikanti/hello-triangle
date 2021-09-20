#include <iostream>
#include <glew.h>
#include <glfw3.h>

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "OpenGL Hello Triangle", NULL, NULL);

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

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

    float positions[6] = {
       -0.5f, -0.5f,
        0.0f,  0.5f,
        0.5f, -0.5f
    };

    unsigned int buffer;

    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}