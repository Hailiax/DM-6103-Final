#version 150

uniform sampler2DRect tex0;
uniform float blurAmount;

in vec2 vTexCoord;
out vec4 outputColor;

// Gaussian weights from http://dev.theomader.com/gaussian-kernel-calculator/

void main()
{

    vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
	
	color += 0.000229 * texture(tex0, vTexCoord + vec2(0.0, blurAmount * -4.0));
	color += 0.005977 * texture(tex0, vTexCoord + vec2(0.0, blurAmount * -3.0));
	color += 0.060598 * texture(tex0, vTexCoord + vec2(0.0, blurAmount * -2.0));
	color += 0.241732 * texture(tex0, vTexCoord + vec2(0.0, blurAmount * -1.0));
    
	color += 0.382928 * texture(tex0, vTexCoord + vec2(0.0, 0));
	
	color += 0.241732 * texture(tex0, vTexCoord + vec2(0.0, blurAmount * 1.0));
	color += 0.060598 * texture(tex0, vTexCoord + vec2(0.0, blurAmount * 2.0));
	color += 0.005977 * texture(tex0, vTexCoord + vec2(0.0, blurAmount * 3.0));
	color += 0.000229 * texture(tex0, vTexCoord + vec2(0.0, blurAmount * 4.0));
    
    outputColor = color;
}
