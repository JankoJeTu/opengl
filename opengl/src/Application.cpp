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

double accumulator = 0.0;
double lastTime = glfwGetTime();

const float gravity = -0.5f; // g acceleration
const float dt = 0.016f; //frame
float bounce = 1.0f; // how much energy to keep

float vx = 0.0f;
float vy = 0.4f;
float r = 0.3f;
float m = 1.0f;



struct RigidBody {

    glm::vec2 position;
    glm::vec2 previousPosition;
    glm::vec2 velocity;
    float radius;
    float mass;

    RigidBody(glm::vec2 pos, glm::vec2 v, float r, float m)
        : position(pos), velocity(v), previousPosition(0.0f), radius(r), mass(m) { }

};

void updatePhysics(RigidBody& rigidBody, const float& dt, float& bounce) {

    rigidBody.previousPosition.x = rigidBody.position.x;
    rigidBody.previousPosition.y = rigidBody.position.y;

    rigidBody.velocity.y += gravity * dt; //gravity
    rigidBody.position.y += rigidBody.velocity.y * dt; //update pos y
    rigidBody.position.x += rigidBody.velocity.x * dt; //update pos x

    /*assuming screen height - 1 to 1*/
    // y
    if (rigidBody.position.y - rigidBody.radius < -1.0f) {
        rigidBody.position.y = -1.0f + rigidBody.radius; //stop at ground
        rigidBody.velocity.y *= -bounce; //energy loss
    }
    else if (rigidBody.position.y + rigidBody.radius > 1.0f) {
        rigidBody.position.y = 1.0f - rigidBody.radius; //stop at ceiling
        rigidBody.velocity.y *= -bounce; //energy loss
    }

    // x
    if (rigidBody.position.x - rigidBody.radius < -1.0f) {
        rigidBody.position.x = -1.0f + rigidBody.radius; //stop at left
        rigidBody.velocity.x *= -bounce; //energy loss
    
    }
    else if (rigidBody.position.x + rigidBody.radius > 1.0f) {
        rigidBody.position.x = 1.0f - rigidBody.radius; //stop at right
        rigidBody.velocity.x *= -bounce; //energy loss
    }

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
        float quadVertices[] = {

            -1.0f, -1.0f,
             1.0f, -1.0f,
            -1.0f,  1.0f,
             1.0f,  1.0f
        };

        unsigned int indices[] = { 0, 1, 2, 1, 2, 3 };

        /*rb circle*/
        std::vector<RigidBody> circles;

        RigidBody circleA(glm::vec2(0.0f, 0.0f), glm::vec2(vx, vy), r, m);
        circles.push_back(circleA);

        RigidBody circleB(glm::vec2(0.5f, 0.5f), glm::vec2(0.0f, 0.0f), 0.2f, m);
        circles.push_back(circleB);


        std::vector<float> bufferData;

        for (const auto& circle : circles) {

            float centerCordX = circle.position.x;
            float centerCordY = circle.position.y;
            float radius = circle.radius;

            for (int j = 0; j < 4; j++) {

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

        /*index buffer*/
        IndexBuffer ib(indices, 6);

        /*projection matrix*/
        //glm::mat4 proj = glm::ortho(0.0f, xWindowSize * 1.0f, 0.0f, yWindowSize * 1.0f, -1.0f, 1.0f);

        /*shader*/
        Shader shader("res/shaders/Basic.shader");
        shader.Bind();
        shader.SetUniform3f("u_Color", 0.2f, 0.3f, 0.8f);
        shader.SetUniform1f("u_EdgeW", 0.005f);

        /*unbind*/
        va.Unbind();
        shader.Unbind();
        vb.Unbind();
        ib.Unbind();

        /*renderer*/
        Renderer renderer;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {

            /*
            double currentTime = glfwGetTime();
            double frameTime = currentTime - lastTime;
            lastTime = currentTime;
            accumulator += frameTime;

            while (accumulator >= dt) {

                update physics
                //updatePhysics(circle, dt, bounce);

                accumulator -= dt;
            }

            //Calculate interpolation factor (how far between physics updates)
            float alpha = accumulator / dt;

            render(alpha);  //Render with interpolated positions
            */

            /* Render here */
            renderer.Clear();
            shader.Bind();
            renderer.Draw(va, shader, circles.size());

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }

    }

    glfwTerminate();
    return 0;
}