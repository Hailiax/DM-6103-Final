#pragma once

#include "ofApp.h"

class ofSecond : public ofBaseApp{
public:
    void setup();
    void update();
    void draw();
    
    // Pointer to main window
    shared_ptr<ofApp> main;
};
