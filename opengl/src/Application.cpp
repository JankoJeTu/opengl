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
#include "Camera.h"

//#include <Windows.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

int xWindowSize = 800;  //must be sq
int yWindowSize = 800;

const float G = 0.005f;
const float dt = 0.016f; //60 fps
const float gravity = -0.5f; // g acceleration
float bounce = 0.9f; // how much energy to keep

double accumulator = 0.0;
double lastTime = glfwGetTime();

bool paused = 0;
bool gravityEnabled = 0;
bool doLerp = 1;

/*void CalculateFrameRate()
{
    static float framesPerSecond = 0.0f;       // This will store our fps
    static float lastTime = 0.0f;       // This will hold the time from the last frame
    float currentTime = GetTickCount() * 0.001f;
    ++framesPerSecond;
    if (currentTime - lastTime > 1.0f)
    {
        lastTime = currentTime;
        fprintf(stderr, "\nCurrent Frames Per Second: %d\n\n", (int)framesPerSecond);
        framesPerSecond = 0;
    }
}*/

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

    if (key == GLFW_KEY_P && action == GLFW_PRESS) {

        paused = !paused;
    }
}

struct RigidBody {

    std::string name;
    glm::vec2 position;
    glm::vec2 prevPos;
    glm::vec2 velocity;
    float radius;
    float mass;
    glm::vec3 color;

    glm::vec2 ElasticCollision(std::vector<RigidBody>& others, std::vector<std::string>& names) const {

        for (auto& other : others) {

            if (!std::count(names.begin(), names.end(), other.name)) {
                float dx = other.position.x - this->position.x;
                float dy = other.position.y - this->position.y;

                float distanceSq = (dx * dx + dy * dy);

                if ((other.radius + this->radius)*(other.radius + this->radius) > distanceSq) {

                    glm::vec2 velocityOfThis = this->velocity - (2.0f * other.mass *
                        glm::dot((this->velocity - other.velocity), (this->position - other.position)) *
                        (this->position - other.position)) / 
                        ((this->mass + other.mass) * distanceSq);

                    glm::vec2 velocityOfOther = other.velocity - (2.0f * this->mass *
                        glm::dot((other.velocity - this->velocity), (other.position - this->position)) *
                        (other.position - this->position)) / ((other.mass + this->mass) * distanceSq);

                    other.velocity = velocityOfOther;
                    return velocityOfThis;
                }
            }
        }
        return glm::vec2(this->velocity);
    }

    glm::vec2 CheckForce(std::vector<RigidBody>& others, std::vector<std::string>& names) const {

        for (auto& other : others) {

            if (!std::count(names.begin(), names.end(), other.name)) {

                float dx = other.position.x - this->position.x;
                float dy = other.position.y - this->position.y;
                float distanceSq = (dx * dx + dy * dy);
                float sumRadii = (this->radius + other.radius);

                float force = (G * this->mass * other.mass) / distanceSq;

                float dirX = dx / std::pow(distanceSq, 1.0f / 2.0f);
                float dirY = dy / std::pow(distanceSq, 1.0f / 2.0f);
                
                float ax1 = (force / this->mass) * dirX;
                float ay1 = (force / this->mass) * dirY;
                float ax2 = (force / other.mass) * -dirX;
                float ay2 = (force / other.mass) * -dirY;
                
                other.velocity.x += ax2 * dt;
                other.velocity.y += ay2 * dt;

                /*lose mass*/

                glm::vec2 velOfThis(this->velocity.x + ax1 * dt, this->velocity.y + ay1 * dt);

                /*returns velocity*/
                return velOfThis;
            }
        }
        return glm::vec2(this->velocity);
    }

    RigidBody(std::string name, glm::vec2 pos, glm::vec2 v, float r, float m, glm::vec3 color)
        : name(name), position(pos), prevPos(1.0f), velocity(v), radius(r), mass(m), color(color) { }

};

void Lerp(std::vector<RigidBody>& circles, float& lerpFactor) {

    for (auto& circle : circles) {

        glm::vec2 interp = circle.prevPos * (1.0f - lerpFactor) + circle.position * lerpFactor;
    }
}

void Orbit(std::vector<RigidBody>& circles, const float& dt) {

    std::vector<std::string> names;

    for (auto& circle : circles) {

        names.push_back(circle.name);
        circle.prevPos = circle.position;

        circle.velocity = circle.CheckForce(circles, names);

        circle.position.y += circle.velocity.y * dt; //update pos y
        circle.position.x += circle.velocity.x * dt; //update pos x

    }
    names.clear();
}

void UpdatePhysics(std::vector<RigidBody>& circles, const float& dt, float& energyCoef) {

    std::vector<std::string> names;

    for (auto& circle : circles) {

        names.push_back(circle.name);
        circle.prevPos = circle.position;

        /*collisions*/
        circle.velocity = circle.ElasticCollision(circles, names);

        if (gravityEnabled) {

            circle.velocity.y += gravity * dt; //gravity
        }

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
    names.clear();
}

void UpdateBufferData(std::vector<RigidBody>& circles, float (&quadVertices)[], VertexArray& va, VertexBuffer& vb) {

    std::vector<float> bufferData;

    for (const auto& circle : circles) {

        float centerCordX = circle.position.x;
        float centerCordY = circle.position.y;
        float radius = circle.radius;
        float colorR = circle.color.x;
        float colorG = circle.color.y;
        float colorB = circle.color.z;

        for (int j = 0; j < 6; j++) {

            bufferData.push_back(quadVertices[j * 2]);
            bufferData.push_back(quadVertices[j * 2 + 1]);
            bufferData.push_back(centerCordX);
            bufferData.push_back(centerCordY);
            bufferData.push_back(radius);
            bufferData.push_back(colorR);
            bufferData.push_back(colorG);
            bufferData.push_back(colorB);
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

                        //name         center                  velocity      radius  mass            color
        RigidBody circleA("A", glm::vec2(0.0f, 0.3f), glm::vec2(0.1f, 0.0f), 0.1f, 4.0f, glm::vec3(0.6f, 0.8f, 1.0f));
        circles.push_back(circleA);

        RigidBody circleB("B", glm::vec2(0.0f, -0.3f), glm::vec2(-0.1f, 0.0f), 0.1f, 4.0f, glm::vec3(0.8f, 0.6f, 1.0f));
        circles.push_back(circleB);
        
        RigidBody circleC("C", glm::vec2(0.6f, 0.6f), glm::vec2(-0.3f, 0.0f), 0.2f, 6.0f, glm::vec3(0.2f, 0.6f, 0.8f));
        circles.push_back(circleC);
        /*
        RigidBody circleD("D", glm::vec2(0.6f, 0.0f), glm::vec2(-0.1f, 0.0f), 0.1f, 1.0f, glm::vec3(0.2f, 0.6f, 1.0f));
        circles.push_back(circleD);

        RigidBody circleE("E", glm::vec2(0.0f, 0.6f), glm::vec2(-0.1f, -0.1f), 0.1f, 1.0f, glm::vec3(0.2f, 0.6f, 1.0f));
        circles.push_back(circleE);

        RigidBody circleF("F", glm::vec2(-0.3f, 0.3f), glm::vec2(-0.1f, 0.2f), 0.1f, 1.0f, glm::vec3(0.2f, 0.6f, 1.0f));
        circles.push_back(circleF);

        RigidBody circleG("G", glm::vec2(-0.6f, 0.6f), glm::vec2(-0.4f, 0.0f), 0.1f, 1.0f, glm::vec3(0.2f, 0.6f, 1.0f));
        circles.push_back(circleG);*/

        /*buffer data*/
        std::vector<float> bufferData;

        for (const auto& circle : circles) {

            float centerCordX = circle.position.x;
            float centerCordY = circle.position.y;
            float radius = circle.radius;
            float colorR = circle.color.x;
            float colorG = circle.color.y;
            float colorB = circle.color.z;

            for (int j = 0; j < 6; j++) {

                bufferData.push_back(quadVertices[j * 2]);
                bufferData.push_back(quadVertices[j * 2 + 1]);
                bufferData.push_back(centerCordX);
                bufferData.push_back(centerCordY);
                bufferData.push_back(radius);
                bufferData.push_back(colorR);
                bufferData.push_back(colorG);
                bufferData.push_back(colorB);
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
        layout.Push<float>(3);  //3
        va.AddBuffer(vb, layout);


        /*shader*/
        Shader shader("res/shaders/Basic.shader");
        shader.Bind();

        shader.SetUniform1f("u_Edge", 0.005f);

        /*unbind*/
        va.Unbind();
        shader.Unbind();
        vb.Unbind();

        /*renderer*/
        Renderer renderer;

        /*camera*/
        Camera2D camera;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* pause */
            glfwSetKeyCallback(window, KeyCallback);

            /*camera movement*/
            camera.ProcessInput(window);
            glm::mat4 cameraMat = camera.GetCameraMatrix();
            shader.Bind();
            shader.SetUniformMat4f("u_Cam", cameraMat);

            if (doLerp) {

                double currentTime = glfwGetTime();
                double frameTime = currentTime - lastTime;
                lastTime = currentTime;
                accumulator += frameTime;

                while (accumulator >= dt) {

                    if (!paused) {

                        //UpdatePhysics(circles, dt, bounce);
                        Orbit(circles, dt);
                    }
                    accumulator -= dt;
                }

                // Calculate interpolation factor (how far between physics updates)
                float lerpFactor = accumulator / dt;
                Lerp(circles, lerpFactor);
            }
            else if(!paused) {
                //UpdatePhysics(circles, dt, bounce);
                Orbit(circles, dt);
            }

            /*update buffer*/
            UpdateBufferData(circles, quadVertices, va, vb);

            /* Render here */
            renderer.Clear();

            //CalculateFrameRate();

            renderer.Draw(va, shader, circles.size() * 6);

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
        
    }

    glfwTerminate();
    return 0;
}