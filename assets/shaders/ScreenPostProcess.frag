#version 460 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D postProcessColorTexture;

void main()
{
    vec2 direction = TexCoords - vec2(0.5);
    
    float aberrationStrength = 0.025; 
    
    float r = texture(postProcessColorTexture, TexCoords + direction * aberrationStrength).r;
    float g = texture(postProcessColorTexture, TexCoords).g;
    float b = texture(postProcessColorTexture, TexCoords - direction * aberrationStrength).b;
    
    vec3 finalColor = vec3(r, g, b);
    
    FragColor = vec4(finalColor, 1.0);
  // vec3 originalColor = texture(postProcessColorTexture, TexCoords).rgb;
    //
    // vec3 targetTint = vec3(0.0, 0.0, 0.0); 
    // float tintStrength = 0.15; 
    //
    // vec3 finalColor = mix(originalColor, originalColor * targetTint, tintStrength);
    //
    // FragColor = vec4(finalColor, 1.0);
}
