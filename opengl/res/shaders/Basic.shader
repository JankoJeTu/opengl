#shader vertex
#version 330 core
        
layout(location = 0) in vec2 quadPosition;
layout(location = 1) in vec2 centerPosition;
layout(location = 2) in float circleRadius;
layout(location = 3) in vec3 circleColor;

uniform mat4 u_Cam;

out vec2 fragPos;
out vec2 center;
out float radius;
out vec3 fragColor;

void main()
{
    vec2 worldPosition = centerPosition + quadPosition * circleRadius;

    gl_Position = u_Cam * vec4(worldPosition, 0.0, 1.0);

    fragPos = quadPosition * circleRadius;
    radius = circleRadius;
    fragColor = circleColor;
};

#shader fragment
#version 330 core
        
layout(location = 0) out vec4 color;

in vec2 fragPos;
in float radius;
in vec3 fragColor;

uniform float u_Edge;

void main()
{
    float distance = length(fragPos);

    if (distance > radius) 
        discard;

    float alpha = smoothstep(radius, radius - u_Edge, distance);

    color = vec4(fragColor, alpha);

};