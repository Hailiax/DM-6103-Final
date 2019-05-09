#version 150

uniform sampler2DRect tex0;
uniform sampler2DRect fractalData;

uniform float blurAmount;
uniform float frameNum;

uniform float thresh1;
uniform float thresh2;
uniform float thresh3;
uniform float thresh4;
uniform vec4 color1;
uniform vec4 color2;
uniform vec4 color3;
uniform vec4 color4;
uniform vec4 color5;

in vec2 vTexCoord;
out vec4 outputColor;

void main()
{

    // Default to last color
    vec4 color = color5;
    
    // Get density (lightness) of surrounding area with gaussian weights
    float surrounding =
        texture(tex0, vTexCoord + vec2( 0.0,  0.0)).r * 0.150342
        +
        texture(tex0, vTexCoord + vec2( 1.0,  1.0)).r * 0.059912 +
        texture(tex0, vTexCoord + vec2( 1.0,  0.0)).r * 0.094907 +
        texture(tex0, vTexCoord + vec2( 1.0, -1.0)).r * 0.059912 +
        texture(tex0, vTexCoord + vec2( 0.0, -1.0)).r * 0.094907 +
        texture(tex0, vTexCoord + vec2(-1.0, -1.0)).r * 0.059912 +
        texture(tex0, vTexCoord + vec2(-1.0,  0.0)).r * 0.094907 +
        texture(tex0, vTexCoord + vec2(-1.0,  1.0)).r * 0.059912 +
        texture(tex0, vTexCoord + vec2( 0.0,  1.0)).r * 0.094907
        +
        texture(tex0, vTexCoord + vec2( 2.0,  2.0)).r * 0.003765 +
        texture(tex0, vTexCoord + vec2( 2.0,  1.0)).r * 0.015019 +
        texture(tex0, vTexCoord + vec2( 2.0,  0.0)).r * 0.023792 +
        texture(tex0, vTexCoord + vec2( 2.0, -1.0)).r * 0.015019 +
        texture(tex0, vTexCoord + vec2( 2.0, -2.0)).r * 0.003765 +
        texture(tex0, vTexCoord + vec2( 1.0, -2.0)).r * 0.015019 +
        texture(tex0, vTexCoord + vec2( 0.0, -2.0)).r * 0.023792 +
        texture(tex0, vTexCoord + vec2(-1.0, -2.0)).r * 0.015019 +
        texture(tex0, vTexCoord + vec2(-2.0, -2.0)).r * 0.003765 +
        texture(tex0, vTexCoord + vec2(-2.0, -1.0)).r * 0.015019 +
        texture(tex0, vTexCoord + vec2(-2.0,  0.0)).r * 0.023792 +
        texture(tex0, vTexCoord + vec2(-2.0,  1.0)).r * 0.015019 +
        texture(tex0, vTexCoord + vec2(-2.0,  2.0)).r * 0.003765 +
        texture(tex0, vTexCoord + vec2(-1.0,  2.0)).r * 0.015019 +
        texture(tex0, vTexCoord + vec2( 0.0,  2.0)).r * 0.023792 +
        texture(tex0, vTexCoord + vec2( 1.0,  2.0)).r * 0.015019
        ;
    
    // Map densities to colors using thresholds
    if (surrounding > thresh1)
        color = color1;
    else if (surrounding > thresh2)
        color = color2;
    else if (surrounding > thresh3)
        color = color3;
    else if (surrounding > thresh4)
        color = color4;
    
//    color.a = (texture(fractalData, vTexCoord).r + 1.0)* 3.0 * (frameNum+1);
    
    outputColor = color;
}
