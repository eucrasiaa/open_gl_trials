#version 460 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D postProcessColorTexture;

void main()
{
    vec3 originalColor = texture(postProcessColorTexture, TexCoords).rgb;
    
    vec3 targetTint = vec3(1.0, 1.0, 0.0); 
    float tintStrength = 1.0; 
    
    vec3 finalColor = mix(originalColor, originalColor * targetTint, tintStrength);
    
    FragColor = vec4(finalColor, 1.0);
}
