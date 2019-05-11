#pragma once

#include "ofApp.h"

class ofDisplay : public ofBaseApp{
public:
    void setup();
    void update();
    void draw();
    
    // Pointer to main window
    shared_ptr<ofApp> main;
    
    // Which display am I?
    int display;
};
