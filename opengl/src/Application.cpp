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

/*float NDC(int& windowSize, unsigned int& pixelPosition) {

    return ((pixelPosition * 1.0f / windowSize) * 2.0f - 1.0f);
}*/

int xWindowSize = 960;
int yWindowSize = 540;

//const unsigned int gridSpacing = 20;

/*std::vector<float> GenerateGridVertices(int& xWindowSize, int& yWindowSize, const unsigned int& gridSpacing) {

    std::vector<float> vertices;

    // x axis
    for (unsigned int i = gridSpacing; i < xWindowSize; i += gridSpacing) {

        vertices.push_back(i * 1.0f);
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);

        vertices.push_back(i * 1.0f);
        vertices.push_back(yWindowSize * 1.0f);
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);
    }

    //y axis
    for (unsigned int i = gridSpacing; i < yWindowSize; i += gridSpacing) {

        vertices.push_back(0.0f);
        vertices.push_back(i * 1.0f);
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);

        vertices.push_back(xWindowSize * 1.0f);
        vertices.push_back(i * 1.0f);
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);
    }
    return vertices;
}*/

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
        
        //std::vector<float> positions = GenerateGridVertices(xWindowSize, yWindowSize, gridSpacing);

        float positions[] = {
            100.0f,100.0f,0.0f,0.0f,
            200.0f,100.0f,0.0f,0.0f,
            200.0f,200.0f,0.0f,0.0f
        };

        unsigned int indicies[] = {
            0,1,2
        };

        /*vertex array*/
        VertexArray va;

        /*vertex buffer*/
        VertexBuffer vb(positions, 12 * sizeof(float));

        /*vertex buffer layout*/
        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);
        va.AddBuffer(vb, layout);

        /*index buffer*/
        IndexBuffer ib(indicies, 3);

        /*projection matrix*/
        glm::mat4 proj = glm::ortho(0.0f, xWindowSize * 1.0f, 0.0f, yWindowSize * 1.0f, -1.0f, 1.0f);

        /*shader*/
        Shader shader("res/shaders/Basic.shader");
        shader.Bind();
        shader.SetUniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);

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
            shader.SetUniform4f("u_Color", red, 0.3f, 0.8f, 1.0f);
            shader.SetUniformMat4f("u_MVP", proj);

            renderer.DrawTriangles(va, ib, shader);

            //unsigned int count = (positions.size()/4);
            //renderer.DrawLines(va, shader, count);

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