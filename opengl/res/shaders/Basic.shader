#shader vertex
#version 330 core
        
layout(location = 0) in vec2 quadPosition;

out vec2 fragPos;

//uniform mat4 u_MVP;
        
void main()
{
    gl_Position = vec4(quadPosition, 0.0, 1.0);
    fragPos = quadPosition;
};

#shader fragment
#version 330 core
        
layout(location = 0) out vec4 color;

in vec2 fragPos;

uniform vec2 u_Center;
uniform float u_Radius;
uniform vec3 u_Color;
uniform float u_EdgeW;

void main()
{
    float distance = length(fragPos - u_Center);
    float alpha = smoothstep(u_Radius, u_Radius - u_EdgeW, distance);

    color = vec4(u_Color, alpha);

};