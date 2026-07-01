#version 460 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D worldColorTexture;
// uniform vec4 u_InversionBounds; 
uniform float u_InversionShift;

void main()
{
    float lineWidth = 200.0; 
    float speed = -50.0; 
    // float speed=0.0;
    float movingY = gl_FragCoord.y - (u_InversionShift * speed);

    vec4 bufferAData = texture(worldColorTexture, TexCoords);
    vec3 invertedColor = vec3(1.0) - bufferAData.rgb;

    float pattern = mod(floor(movingY / lineWidth), 2.0);
    

    if (pattern < 0.5) {
        FragColor = vec4(invertedColor, bufferAData.a); 
    } else {
        FragColor = bufferAData;
    }



    // if (TexCoords.x >= u_InversionBounds.x && TexCoords.x <= u_InversionBounds.z && 
    //     TexCoords.y >= u_InversionBounds.y && TexCoords.y <= u_InversionBounds.w) 
    // {
    //     vec3 invertedColor = vec3(1.0) - sceneColor;
    //     FragColor = vec4(invertedColor, 1.0);
    // } 
    // else 
    // {
    //     FragColor = vec4(sceneColor, 1.0);
    // }

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
