#shader vertex
#version 330 core
        
layout(location = 0) in vec2 quadPosition;
layout(location = 1) in vec2 centerPosition;
layout(location = 2) in float circleRadius;

out vec2 fragPos;
out vec2 center;
out float radius;

void main()
{
    vec2 worldPosition = centerPosition + quadPosition * circleRadius;

    gl_Position = vec4(worldPosition, 0.0, 1.0);

    fragPos = quadPosition * circleRadius;
    radius = circleRadius;
};

#shader fragment
#version 330 core
        
layout(location = 0) out vec4 color;

in vec2 fragPos;
in float radius;

uniform vec3 u_Color;
uniform float u_Edge;

void main()
{
    float distance = length(fragPos);

    if (distance > radius) 
        discard;

    float alpha = smoothstep(radius, radius - u_Edge, distance);

    color = vec4(u_Color, alpha);

};