#version 460 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D worldColorTexture;
uniform vec4 u_InversionBounds; 

void main()
{
    vec3 sceneColor = texture(worldColorTexture, TexCoords).rgb;
    
    if (TexCoords.x >= u_InversionBounds.x && TexCoords.x <= u_InversionBounds.z && 
        TexCoords.y >= u_InversionBounds.y && TexCoords.y <= u_InversionBounds.w) 
    {
        vec3 invertedColor = vec3(1.0) - sceneColor;
        FragColor = vec4(invertedColor, 1.0);
    } 
    else 
    {
        FragColor = vec4(sceneColor, 1.0);
    }
    // vec3 sceneColor = texture(worldColorTexture, TexCoords).rgb; 
    // if (TexCoords.x >= 0.4 && TexCoords.x <= 0.6 && 
    //     TexCoords.y >= 0.4 && TexCoords.y <= 0.6) 
    // {
    //     vec3 invertedColor = vec3(1.0) - sceneColor;
    //     FragColor = vec4(invertedColor, 1.0);
    // } 
    // else 
    // {
    //     FragColor = vec4(sceneColor, 1.0);
    // }
}
