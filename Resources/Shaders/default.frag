#version 330 core
out vec4 FragColor;
uniform vec4 ourColor;
uniform int useGrid;
in vec3 worldPos;

float gridLine(float coord) {
    float line = abs(fract(coord * 0.5 - 0.5) - 0.5);
    return 1.0 - smoothstep(0.0, 0.02, min(line, 0.5));
}

void main()
{
    if (useGrid == 1) {
        float lineX = gridLine(worldPos.x);
        float lineZ = gridLine(worldPos.z);
        float grid = max(lineX, lineZ);
        vec3 baseColor = vec3(0.10, 0.12, 0.15);
        vec3 lineColor = vec3(0.45, 0.45, 0.50);
        vec3 color = mix(baseColor, lineColor, grid);
        FragColor = vec4(color, 1.0);
    } else {
        FragColor = ourColor;
    }
}