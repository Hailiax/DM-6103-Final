#include "ofDisplay.h"

//--------------------------------------------------------------

void ofDisplay::setup(){
//    displayOneServer.setName("display " + ofToString(display));
    
//    mClient.setup();
//    mClient.set("", "Simple Server");
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
    
//    mClient.draw(50, 50);
//    displayOneServer.publishScreen();
//    main->frameMesh.draw();
}
