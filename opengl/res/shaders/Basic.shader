#shader vertex
#version 330 core
        
layout(location = 0) in vec2 quadPosition;
layout(location = 1) in vec2 iCenter;
layout(location = 2) in float iRadius;

out vec2 fragPos;
flat out vec2 center;
flat out float radius;

//uniform mat4 u_MVP;
        
void main()
{
    gl_Position = vec4(position, 0.0, 1.0);
    fragPos = quadPosition;
    center = iCenter;
    radius = iRadius;
};

#shader fragment
#version 330 core
        
layout(location = 0) out vec4 color;

in vec2 fragPos;
flat in vec2 center;
flat in float radius;

uniform vec2 u_Center;
uniform float u_Radius;
uniform vec4 u_Color;

void main()
{
    color = u_Color;

    float distance = length(fragPos - u_Center);
    if(distance > u_Radius)
        discard;


};