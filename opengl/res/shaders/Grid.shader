#shader vertex
#version 330 core
        
layout(location = 0) in vec2 quadPosition;

out vec2 fragPos;

void main()
{
    gl_Position = vec4(quadPosition, 0.0, 1.0);

    fragPos = quadPosition;
};

#shader fragment
#version 330 core
        
layout(location = 0) out vec4 color;

in vec2 fragPos;

uniform float u_Zoom;
uniform vec3 gridColor = vec3(0.5, 0.5, 0.5); // Grid line color

void main()
{
    float lineThickness = max(1.0, u_Zoom * 0.5); // Thickness of the grid lines
    float gridSpacing = 50.0 * pow(2.0, floor(log2(u_Zoom))); // Grid spacing

    vec2 screenPos = (fragPos + 1.0) * 0.5;
    screenPos *= vec2(800.0, 800.0);

    vec2 gridPos = mod(screenPos, gridSpacing);

    float lineX = 1.0 - smoothstep(0.0, lineThickness, min(gridPos.x, gridSpacing - gridPos.x));
    float lineY = 1.0 - smoothstep(0.0, lineThickness, min(gridPos.y, gridSpacing - gridPos.y));

    float gridLine = max(lineX, lineY);
    vec3 lineColor = mix(vec3(0.0), gridColor, gridLine); // Blend grid lines with background
    
    color = vec4(lineColor, 0.8);
    
};