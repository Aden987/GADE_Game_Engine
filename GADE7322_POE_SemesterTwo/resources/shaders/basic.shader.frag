#version 330 core
out vec4 FragColor;

in vec3 outColour;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;

uniform int texType;

void main()
{
    //FragColor = vec4(outColour, 1.0f);
    FragColor = texture(texture1, TexCoord);
//    if (texType == 1)
//    {
//        FragColor = texture(texture2, TexCoord);
//    }
//    else if (texType == 2)
//    {
//        FragColor = texture(texture3, TexCoord);
//    }
//    
    
};