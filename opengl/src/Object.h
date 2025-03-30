#pragma once

#include <string>
#include <vector>
#include "glm/glm.hpp"

class Object
{
public:
    std::string name;
    glm::vec2 position;
    glm::vec2 velocity;
    float radius;
    float mass;

	Object(std::string name, glm::vec2 pos, glm::vec2 v, float r, float m);
	~Object();

    glm::vec2 CheckCollision(std::vector<Object>& others) const;

private:

    std::vector<std::string> names;
};