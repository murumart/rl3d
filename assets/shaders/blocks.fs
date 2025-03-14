#version 330

// Input vertex attributes (from vertex shader)
in vec3 fragPosition;
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;
in float vertexBrightness;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

// NOTE: Add your custom variables here

void main()
{
	vec4 brightness = vec4(vertexBrightness, vertexBrightness, vertexBrightness, 1.0);
	vec4 col = texture(texture0, fragTexCoord);
	finalColor = col * brightness * fragColor;
	//finalColor = brightness * fragColor;
}