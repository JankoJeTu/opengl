#shader vertex
#version 330 core
        
layout(location = 0) in vec2 position;
out vec2 fragPos;

uniform mat4 u_MVP;
        
void main()
{
    gl_Position = vec4(position, 0.0, 1.0);
    fragPos = position;
};

#shader fragment
#version 330 core
        
layout(location = 0) out vec4 color;
in vec2 fragPos;

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