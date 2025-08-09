//
#version 460 core

layout (location=0) in vec2 uv;
layout (location=0) out vec4 fragColor;


layout(binding = 0) uniform sampler2D texScene;
layout(binding = 1) uniform sampler2D texDepth;

void main()
{
	// Depth en [0,1]
    float depthValue = texture(texDepth, uv).r;

    vec3 color;

    // Umbrales de distancia (ajústalos según tu escena)
    float nearThreshold = 0.7; // 40% del rango depth = muy cerca
    float farThreshold  = 0.99; // 78% del rango depth = muy lejos

    if (depthValue <= nearThreshold)
    {
        // Muy cerca → rojo
        color = vec3(1.0, 0.0, 0.0);
    }
    else if (depthValue >= farThreshold)
    {
        // Muy lejos → verde
        color = vec3(0.0, 1.0, 0.0);
    }
    else
    {
        // Entre medio → usar la textura de escena
        color = texture(texScene, uv).rgb;
    }

    fragColor = vec4(color, 1.0);
}
