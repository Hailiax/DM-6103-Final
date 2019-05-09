#version 150

uniform sampler2DRect prevAgeData;

uniform float timestep;
uniform float life;
uniform int phase;

in vec2 vTexCoord;

out vec4 vFragColor;

void main(void){
    // Get the age from the pixel color.
    float trueAge = texture( prevAgeData, vTexCoord ).g;
    float originalAge = texture( prevAgeData, vTexCoord ).b;
    
    // Update the age
    trueAge += timestep;
    if (trueAge > life)
        trueAge = 0.0;
    
    float age = trueAge;
    
    // Calculate alternative ages
    if (phase == 2){
        age = int(trueAge/timestep)%30*timestep;
    }
    
    // And finally store it on the position FBO.
    vFragColor = vec4(age,trueAge,originalAge,1.0);
}

