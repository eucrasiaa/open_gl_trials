#version 460 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D postProcessColorTexture;

void main()
{
    vec3 finalColor = texture(postProcessColorTexture, TexCoords).rgb;
    
    // just red tint it all
    vec3 redTint = finalColor * vec3(1.0, 0.2, 0.2);
    
    FragColor = vec4(redTint, 1.0);
}
