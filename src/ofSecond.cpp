#include "ofSecond.h"

//--------------------------------------------------------------

void ofSecond::setup(){
    
}

//--------------------------------------------------------------
void ofSecond::update(){
    
}

//--------------------------------------------------------------
void ofSecond::draw(){
    ofBackground(0);
    
    // Draw second half of image
    main->effectsPingPong.src->getTexture().drawSubsection(0,0,main->width,main->height/2,0,0);
    
    // Draw frame
    main->frameMesh.draw();
}
