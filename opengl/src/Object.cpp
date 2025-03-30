#include "Object.h"


Object::Object(std::string name, glm::vec2 pos, glm::vec2 v, float r, float m)
    : name(name), position(pos), velocity(v), radius(r), mass(m)
{

}

Object::~Object()
{

}

glm::vec2 Object::CheckCollision(std::vector<Object>& others) const {

    for (auto& other : others) {

        if (other.name != this->name) {

            names.begin();

            float dx = other.position.x - this->position.x;
            float dy = other.position.y - this->position.y;
            float distance = (dx * dx + dy * dy);

            if ((other.radius + this->radius) * (other.radius + this->radius) > distance) {

                glm::vec2 velocityOfThis = this->velocity - (2.0f * other.mass *
                    glm::dot((this->velocity - other.velocity), (this->position - other.position)) *
                    (this->position - other.position)) /
                    ((this->mass + other.mass) * distance);

                glm::vec2 velocityOfOther = other.velocity - (2.0f * this->mass *
                    glm::dot((other.velocity - this->velocity), (other.position - this->position)) *
                    (other.position - this->position)) / ((other.mass + this->mass) * distance);

                other.velocity = velocityOfOther;
                return velocityOfThis;
            }
        }
    }
    return glm::vec2(this->velocity);
}
