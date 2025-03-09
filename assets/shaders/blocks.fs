#version 330

// Input vertex attributes (from vertex shader)
in vec3 fragPosition;
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

// NOTE: Add your custom variables here

void main() {
    float brightness = 1.0;
    brightness -= normal.x * 0.1;
    finalColor = vec4(normal.x, normal.g, normal.b, 1.0);
}