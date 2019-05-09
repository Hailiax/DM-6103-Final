#version 150

uniform sampler2DRect tex0;
uniform sampler2DRect sharpTex;

in vec2 vTexCoord;
out vec4 outputColor;

void main()
{
    vec4 colorBlur = texture(tex0, vTexCoord);
    vec4 color = texture(sharpTex, vTexCoord);
    
    if (color.g < 0.00001)
        color = colorBlur;
    
    outputColor = color;
}
