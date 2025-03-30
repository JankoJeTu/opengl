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


const float gravity = -0.5f; // g acceleration
const float dt = 0.016f; //frame
float bounce = 0.9f; // how much energy to keep




struct RigidBody {

    std::string name;
    glm::vec2 position;
    glm::vec2 velocity;
    float radius;
    float mass;

    RigidBody(std::string name, glm::vec2 pos, glm::vec2 v, float r)
        : name(name), position(pos), velocity(v), radius(r), mass(1.0f) { }

};

void UpdatePhysics(std::vector<RigidBody>& circles, const float& dt, float& energyCoef) {

    for (auto& circle : circles) {

        //circle.previousPosition.x = circle.position.x;
        //circle.previousPosition.y = circle.position.y;

        circle.velocity.y += gravity * dt; //gravity
        circle.position.y += circle.velocity.y * dt; //update pos y
        circle.position.x += circle.velocity.x * dt; //update pos x

        /*assuming screen height - 1 to 1*/
    // y
        if (circle.position.y - circle.radius < -1.0f) {
            circle.position.y = -1.0f + circle.radius; //stop at ground
            circle.velocity.y *= -bounce; //energy loss
        }
        else if (circle.position.y + circle.radius > 1.0f) {
            circle.position.y = 1.0f - circle.radius; //stop at ceiling
            circle.velocity.y *= -bounce; //energy loss
        }

        // x
        if (circle.position.x - circle.radius < -1.0f) {
            circle.position.x = -1.0f + circle.radius; //stop at left
            circle.velocity.x *= -bounce; //energy loss

        }
        else if (circle.position.x + circle.radius > 1.0f) {
            circle.position.x = 1.0f - circle.radius; //stop at right
            circle.velocity.x *= -bounce; //energy loss
        }
    }
}

void UpdateBufferData(std::vector<RigidBody>& circles, float (&quadVertices)[], VertexArray& va, VertexBuffer& vb) {

    std::vector<float> bufferData;

    for (const auto& circle : circles) {

        float centerCordX = circle.position.x;
        float centerCordY = circle.position.y;
        float radius = circle.radius;

        for (int j = 0; j < 6; j++) {

            bufferData.push_back(quadVertices[j * 2]);
            bufferData.push_back(quadVertices[j * 2 + 1]);
            bufferData.push_back(centerCordX);
            bufferData.push_back(centerCordY);
            bufferData.push_back(radius);
        }
    }

    va.UpdateBuffer(vb, bufferData.size() * sizeof(float), bufferData.data());
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

    /*opengl version*/
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /*check if GLEW is init*/
    if (glewInit() != GLEW_OK)
        std::cout << "err" << std::endl;

    /*scope*/
    {
        /*canvas*/
        float quadVertices[12] = {

            -1.0f, -1.0f,  // Bottom-left
             1.0f, -1.0f,  // Bottom-right
             1.0f,  1.0f,  // Top-right

            -1.0f, -1.0f,  // Bottom-left
             1.0f,  1.0f,  // Top-right
            -1.0f,  1.0f   // Top-left
        };

        /*rb circle*/
        std::vector<RigidBody> circles;

        RigidBody circleA("A", glm::vec2(0.5f, 0.5f), glm::vec2(0.0f, 0.0f), 0.3f);
        circles.push_back(circleA);

        RigidBody circleB("B", glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), 0.2f);
        circles.push_back(circleB);

        RigidBody circleC("C", glm::vec2(-0.5f, -0.5f), glm::vec2(0.0f, 0.0f), 0.1f);
        circles.push_back(circleC);


        /*buffer data*/
        std::vector<float> bufferData;

        for (const auto& circle : circles) {

            float centerCordX = circle.position.x;
            float centerCordY = circle.position.y;
            float radius = circle.radius;

            for (int j = 0; j < 6; j++) {

                bufferData.push_back(quadVertices[j * 2]);
                bufferData.push_back(quadVertices[j * 2 + 1]);
                bufferData.push_back(centerCordX);
                bufferData.push_back(centerCordY);
                bufferData.push_back(radius);
            }
        }
        
        /*blending*/
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        /*vertex array*/
        VertexArray va;

        /*vertex buffer*/
        VertexBuffer vb(bufferData.data(), bufferData.size() * sizeof(float));

        /*vertex buffer layout*/
        VertexBufferLayout layout;
        layout.Push<float>(2);  //0
        layout.Push<float>(2);  //1
        layout.Push<float>(1);  //2
        va.AddBuffer(vb, layout);


        /*shader*/
        Shader shader("res/shaders/Basic.shader");
        shader.Bind();
        shader.SetUniform3f("u_Color", 0.2f, 0.3f, 0.8f);
        shader.SetUniform1f("u_Edge", 0.005f);

        /*unbind*/
        va.Unbind();
        shader.Unbind();
        vb.Unbind();
        //ib.Unbind();

        /*renderer*/
        Renderer renderer;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {

            /* Render here */
            renderer.Clear();

            UpdatePhysics(circles, dt, bounce);
            UpdateBufferData(circles, quadVertices, va, vb);

            renderer.Draw(va, shader, 18);

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }

    }

    glfwTerminate();
    return 0;
}