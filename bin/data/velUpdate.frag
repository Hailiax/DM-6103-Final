#version 150

uniform sampler2DRect backbuffer;
uniform sampler2DRect origVelData;
uniform sampler2DRect posData;
uniform sampler2DRect ageData;

uniform int phase;
uniform int attractToggle;
uniform float timestep;
uniform float dancerRadiusSquared;
uniform float opposingVelocity;
uniform vec2 screen;
uniform vec2 mouse;

in vec2 vTexCoord;

out vec4 vFragColor;

const float PI = 3.1415926535897932384626433832795;
    
void main(void){
    
    // Get the position and velocity from the pixel color.
    vec2 pos = texture( posData, vTexCoord).rg;
    vec2 origVel = texture( origVelData, vTexCoord).rg;
    vec2 vel = texture( backbuffer, vTexCoord ).rg;
    float age = texture( ageData, vTexCoord ).r;
    
    // Decrease radius if on 3rd phase
    float dancerRad = dancerRadiusSquared;
    if (phase == 3) dancerRad /= 9;
        
    // Update the velocity.
    if (age < timestep){
        vel = origVel; // Reset Velocity if particle just died
    } else{
        
        // Get XY distance of particle to mouse
        float posX = pos.x*screen.x;
        float posY = pos.y*screen.y;
        float distX = mouse.x - posX;
        float distY = mouse.y - posY;
        
        // Get angle of particle to mouse, correct for limited range of atan
        float angle = atan( distY/distX );
        if (mouse.x < posX) angle += PI;
        
        // Calculate force based off of distance using dancer radius. Will return negative values if particle is in dancerRad of dancer position
        float force = 50000/(distX*distX + distY*distY - dancerRad);
        
        if (phase == 3){
            // Turn off attraction unless within dancer radius, then repel significantly
            if (force > 0) force = 0;
            if (force < 0) {
                vel.x = 0;
                vel.y = 0;
//                angle += PI/2; // Spin particles
                force = -opposingVelocity*5;
            }
        } else{
            // Increase force for phase 2
            if (phase == 2) force *= 2;
            
            // Max out force at 1
            if (force > 1) force = 1;
            
            // Make force inside radius constant
            if (force < 0) {
                vel.x = 0;
                vel.y = 0;
//                angle += PI/2; // Spin particles
                force = opposingVelocity;
            }
        }
        
        // Dont update velocities if attract toggle is off
        if (attractToggle == 1){
            vel.x += cos(angle)*force*screen.y/screen.x;
            vel.y += sin(angle)*force;
        }
        
        // Air resistance
        vel -= vel * 0.01;
        
    }
    
    // Then save the vel data into the velocity FBO.
    vFragColor = vec4(vel.x,vel.y,0.0,1.0);
}
