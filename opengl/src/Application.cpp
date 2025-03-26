/* docs.gl */
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Shader.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"


int xWindowSize = 800;
int yWindowSize = 800;

struct Circle {

    float x, y;        // Position
    float vx, vy;      // Velocity
    float radius;      // Radius
};

std::vector<Circle> circles = {

    { 0.0f, 0.0f, 0.0f, 0.0f, 0.4f }   // Circle 3
};

int main(void)
{
    GLFWwindow* window;

    /* Initialize GLFW library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(xWindowSize, yWindowSize, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(10);

    /*check if GLEW is init*/
    if (glewInit() != GLEW_OK)
        std::cout << "err" << std::endl;

    /*scope*/
    {
        float quadVertices[] = {

            -1.0f, -1.0f,
             1.0f, -1.0f,
            -1.0f,  1.0f,
             1.0f,  1.0f
        };

        unsigned int indices[] = { 0, 1, 2, 1, 2, 3 };

        /*blending*/
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        /*vertex array*/
        VertexArray va;

        /*vertex buffer*/
        VertexBuffer vb(quadVertices, 8 * sizeof(float));

        /*vertex buffer layout*/
        VertexBufferLayout layout;
        layout.Push<float>(2);
        va.AddBuffer(vb, layout);

        /*index buffer*/
        IndexBuffer ib(indices, 6);

        /*projection matrix*/
        //glm::mat4 proj = glm::ortho(0.0f, xWindowSize * 1.0f, 0.0f, yWindowSize * 1.0f, -1.0f, 1.0f);

        /*shader*/
        Shader shader("res/shaders/Basic.shader");
        shader.Bind();
        shader.SetUniform3f("u_Color", 0.2f, 0.3f, 0.8f);
        shader.SetUniform1f("u_Radius", 0.2f);
        shader.SetUniform2f("u_Center", 0.0f, 0.0f);
        shader.SetUniform1f("u_EdgeW", 0.005f);

        /*unbind*/
        va.Unbind();
        shader.Unbind();
        vb.Unbind();
        ib.Unbind();

        /*renderer*/
        Renderer renderer;

        float red = 0.0f;
        float increment = 0.05f;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            renderer.Clear();

            shader.Bind();
            shader.SetUniform3f("u_Color", 0.3f, 0.3f, 0.8f);
            //shader.SetUniformMat4f("u_MVP", proj);

            renderer.DrawTriangles(va, ib, shader);

            /*change red channel*/
            if (red > 1.0f) {
                increment = -0.05f;
            }
            else if (red < 0.0f) {
                increment = 0.05f;
            }

            red += increment;

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }

    }

    glfwTerminate();
    return 0;
}