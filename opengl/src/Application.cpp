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

float NDC(const unsigned int& windowSize, unsigned int& pixelPosition) {

    return ((pixelPosition * 1.0f / windowSize) * 2.0f - 1.0f);
}

const unsigned int xWindowSize = 640; //has to be multiple of 10
const unsigned int yWindowSize = 480;

const unsigned int gridSpacing = 20; //has to divide windowsize without remainder

std::vector<float> GenerateGridVertices(const unsigned int& xWindowSize, const unsigned int& yWindowSize, const unsigned int& gridSpacing) {

    std::vector<float> vertices;

    // x axis
    for (unsigned int i = gridSpacing; i < xWindowSize; i += gridSpacing) {

        vertices.push_back(NDC(xWindowSize, i));
        vertices.push_back(-1.0f);

        vertices.push_back(NDC(xWindowSize,i));
        vertices.push_back(1.0f);
    }

    //y axis
    for (unsigned int i = gridSpacing; i < yWindowSize; i += gridSpacing) {

        vertices.push_back(-1.0f);
        vertices.push_back(NDC(yWindowSize, i));

        vertices.push_back(1.0f);
        vertices.push_back(NDC(yWindowSize, i));
    }
    return vertices;
}

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

        std::vector<float> positions = GenerateGridVertices(xWindowSize, yWindowSize, gridSpacing);

        /*vertex array*/
        VertexArray va;

        /*vertex buffer*/
        VertexBuffer vb(positions.data(), positions.size() * sizeof(float));

        /*vertex buffer layout*/
        VertexBufferLayout layout;
        layout.Push<float>(2);
        va.AddBuffer(vb, layout);

        /*index buffer*/
        //IndexBuffer ib(indicies, 6);

        /*projection matrix*/
        glm::mat4 proj = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);

        /*shader*/
        Shader shader("res/shaders/Basic.shader");
        shader.Bind();
        shader.SetUniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);

        /*unbind*/
        va.Unbind();
        shader.Unbind();
        vb.Unbind();
        //ib.Unbind();

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
            shader.SetUniform4f("u_Color", red, 0.3f, 0.8f, 1.0f);
            shader.SetUniformMat4f("u_MVP", proj);

            //renderer.DrawTriangles(va, ib, shader);

            unsigned int count = ((xWindowSize + yWindowSize) / gridSpacing * 2) - 2 * 2;
            renderer.DrawLines(va, shader, count);

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