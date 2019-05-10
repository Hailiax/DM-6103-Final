#version 150

uniform sampler2DRect tex0;
//uniform sampler2DRect fractalData;

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
//    float surrounding =
//        texture(tex0, vTexCoord + vec2( 0.0,  0.0)).r * 0.150342
//        +
//        texture(tex0, vTexCoord + vec2( 1.0,  1.0)).r * 0.059912 +
//        texture(tex0, vTexCoord + vec2( 1.0,  0.0)).r * 0.094907 +
//        texture(tex0, vTexCoord + vec2( 1.0, -1.0)).r * 0.059912 +
//        texture(tex0, vTexCoord + vec2( 0.0, -1.0)).r * 0.094907 +
//        texture(tex0, vTexCoord + vec2(-1.0, -1.0)).r * 0.059912 +
//        texture(tex0, vTexCoord + vec2(-1.0,  0.0)).r * 0.094907 +
//        texture(tex0, vTexCoord + vec2(-1.0,  1.0)).r * 0.059912 +
//        texture(tex0, vTexCoord + vec2( 0.0,  1.0)).r * 0.094907
//        +
//        texture(tex0, vTexCoord + vec2( 2.0,  2.0)).r * 0.003765 +
//        texture(tex0, vTexCoord + vec2( 2.0,  1.0)).r * 0.015019 +
//        texture(tex0, vTexCoord + vec2( 2.0,  0.0)).r * 0.023792 +
//        texture(tex0, vTexCoord + vec2( 2.0, -1.0)).r * 0.015019 +
//        texture(tex0, vTexCoord + vec2( 2.0, -2.0)).r * 0.003765 +
//        texture(tex0, vTexCoord + vec2( 1.0, -2.0)).r * 0.015019 +
//        texture(tex0, vTexCoord + vec2( 0.0, -2.0)).r * 0.023792 +
//        texture(tex0, vTexCoord + vec2(-1.0, -2.0)).r * 0.015019 +
//        texture(tex0, vTexCoord + vec2(-2.0, -2.0)).r * 0.003765 +
//        texture(tex0, vTexCoord + vec2(-2.0, -1.0)).r * 0.015019 +
//        texture(tex0, vTexCoord + vec2(-2.0,  0.0)).r * 0.023792 +
//        texture(tex0, vTexCoord + vec2(-2.0,  1.0)).r * 0.015019 +
//        texture(tex0, vTexCoord + vec2(-2.0,  2.0)).r * 0.003765 +
//        texture(tex0, vTexCoord + vec2(-1.0,  2.0)).r * 0.015019 +
//        texture(tex0, vTexCoord + vec2( 0.0,  2.0)).r * 0.023792 +
//        texture(tex0, vTexCoord + vec2( 1.0,  2.0)).r * 0.015019
//        ;
//    float gaussianVals[49] = float[49](0.0000000000, 0.0000000000, 0.0000000000, 0.0006225496, 0.0000000000, 0.0000000000, 0.0000000000,0.0000000000, 0.0000000000, 0.0000000000, 0.0075376138, 0.0000000000, 0.0000000000, 0.0000000000,0.0000000000, 0.0000000000, 0.0129615686, 0.0854328997, 0.0129615686, 0.0000000000, 0.0000000000,0.0006225496, 0.0075376138, 0.0854328997, 0.5631093421, 0.0854328997, 0.0075376138, 0.0006225496,0.0000000000, 0.0000000000, 0.0129615686, 0.0854328997, 0.0129615686, 0.0000000000, 0.0000000000,0.0000000000, 0.0000000000, 0.0000000000, 0.0075376138, 0.0000000000, 0.0000000000, 0.0000000000,0.0000000000, 0.0000000000, 0.0000000000, 0.0006225496, 0.0000000000, 0.0000000000, 0.0000000000);
//    float surrounding = 0;
//
//    for (int y = 0; y < 7; y++){
//        for (int x = 0; x < 7; x++){
//            float m = gaussianVals[x*y];
//            if (m > 0.00000001)
//                surrounding += texture(tex0, vTexCoord + vec2(x-3,y-3)).r * m;
//        }
//    }
    // texture(tex0, vTexCoord + vec2(-3.0,  0.0)).r
    float surrounding =
         0.0006225496 * (
                         texture(tex0, vTexCoord + vec2(-3.0,  0.0)).r +
                         texture(tex0, vTexCoord + vec2( 3.0,  0.0)).r +
                         texture(tex0, vTexCoord + vec2( 0.0,  3.0)).r +
                         texture(tex0, vTexCoord + vec2( 0.0, -3.0)).r
                         ) +
        0.0075376138 * (
                        texture(tex0, vTexCoord + vec2(-2.0,  0.0)).r +
                        texture(tex0, vTexCoord + vec2( 2.0,  0.0)).r +
                        texture(tex0, vTexCoord + vec2( 0.0,  2.0)).r +
                        texture(tex0, vTexCoord + vec2( 0.0, -2.0)).r
                        ) +
        0.0129615686 * (
                        texture(tex0, vTexCoord + vec2(-1.0,  1.0)).r +
                        texture(tex0, vTexCoord + vec2(-1.0, -1.0)).r +
                        texture(tex0, vTexCoord + vec2( 1.0, -1.0)).r +
                        texture(tex0, vTexCoord + vec2( 1.0,  1.0)).r
                        ) +
        0.0854328997 * (
                        texture(tex0, vTexCoord + vec2(-1.0,  0.0)).r +
                        texture(tex0, vTexCoord + vec2( 1.0,  0.0)).r +
                        texture(tex0, vTexCoord + vec2( 0.0,  1.0)).r +
                        texture(tex0, vTexCoord + vec2( 0.0, -1.0)).r
                        ) +
        0.5631093421 *  texture(tex0, vTexCoord + vec2( 0.0,  0.0)).r
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
