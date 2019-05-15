#include "ofDisplay.h"

//--------------------------------------------------------------

void ofDisplay::setup(){
    
}

//--------------------------------------------------------------
void ofDisplay::update(){
    
}

//--------------------------------------------------------------
void ofDisplay::draw(){
    ofBackground(0);
    
    // Draw image
    main->effectsPingPong.src->getTexture().drawSubsection(
                                                           0, 0,
                                                           main->windowWidth, main->windowHeight,
                                                           0, main->FBOheight - main->windowHeight*display
    );
    
    // Draw frame
    glPointSize(10.f);
//    main->frameMesh.draw();
}
