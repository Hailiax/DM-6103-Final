#version 150

uniform sampler2DRect prevPosData;
uniform sampler2DRect origPosData;
uniform sampler2DRect velData;
uniform sampler2DRect ageData;
uniform sampler2DRect fractalData;

uniform int phase;
uniform int phase1Fractal;
uniform vec2 screen;
uniform float timestep;
uniform float velocityScale;

in vec2 vTexCoord;

out vec4 vFragColor;

void main(void){
    // Get the position and velocity from the pixel color.
    vec2 pos = texture( prevPosData, vTexCoord ).rg;
    vec2 origPos = texture( origPosData, vTexCoord ).rg;
    vec2 vel = texture( velData, vTexCoord ).rg;
    float age = texture( ageData, vTexCoord ).r;
    
    
    // Update the position.
    if (age < timestep){
        // Reset position if particle just died
        pos = origPos;
    } else{
        // Default fractal value, no influence
        vec2 fractal = vec2(1.0,1.0);
        
        // If particle is within window bounds => fractal bounds
        if (pos.x > 0.0 && pos.x < 1.0 && pos.y > 0.0 && pos.y < 1.0){
            // Get fractal value [-1,1] for X on r and Y on g.
            fractal = texture( fractalData, vec2(pos.x*screen.x,pos.y*screen.y) ).rg;
            
            // Dampen effect of fractal on parts of phase 1
            if (phase1Fractal == 1 && phase == 1){
                fractal += 1;
                fractal *= 0.5;
                fractal += 0.5;
            }
            
        }
        
        // Update positions with velocities
        if (phase == 1 && phase1Fractal == 0) {
            // No fractal effect
            pos += vel * timestep * velocityScale;
        } else{
            pos += vel * fractal * timestep * velocityScale;
        }
        
    }
    
    // And finally store it on the position FBO.
    vFragColor = vec4(pos.x,pos.y,1.0,1.0);
}
