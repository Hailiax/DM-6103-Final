#include "ofMain.h"
#include "ofAppGLFWWindow.h"
#include "ofApp.h"
#include "ofDisplay.h"

// DM 6013 Project "Lost" Graphics Program created by Alexander Wing
// Copyright 2019

// Note: This program requires the addon ofxSocketIO
//
// This program uses FastNoiseSIMD which requires you to set specific compiler settings
// Find these settings at https://github.com/Auburns/FastNoiseSIMD/wiki/Compiler-Settings (In my experience, this doc contains incomplete compiler settings)
//
// For Xcode, I use: compiler setting -std=c++11 and -mfma, and the following for individual files:
// FastNoiseSIMD_sse2.cpp: -msse2
// FastNoiseSIMD_sse41.cpp: -msse4.1
// FastNoiseSIMD_avx2.cpp: -mavx2
// FastNoiseSIMD_avx512.cpp: -mavx512f
//
// Your compiler/chip may not support some SIMD features and may cause this program to fail
// To fix, go to src > FastNoiseSIMD > FastNoiseSIMD.h and comment out any offending lines

// This program does all the graphics calculation on ofApp.
// ofApp can display the graphics as well or...
// ofApp can display a GUI and one or more ofDisplay can display the graphics (good for multi screen setups)
// This program can also receive mocap positions. It is currently configured for kinect and receives the data over sockets
// To add your own mocap functionality, make sure "useServerPosition" is set to true and edit "onServerEvent()" at the bottom of ofApp.cpp

int main(){
    
    // Build settings (more graphics related settings can be found at the top of ofApp.cpp)
    bool fullscreen = false;
    int windows = 2; // Number of auxillary drawing windows. If this is set to 0, a GUI will not be drawn and graphics will be drawn on one window.
    int width = 800; // Height/width for non fullscreen. If fullscreen, determines GUI height/width
    int height = 450;
    
    float beat = 5.0; // How many "bangs" per minute. Bpm divided by measures/min divided by beats/measure?
    int particleCount = 300000; // Turn up as much as possible. Harder on the GPU
    float attractorRadius = 35.0;
    int frameWidth = 20;
    float velocityScale = 0.5f; // Adjust per computer for speed
    float opposingVelocity = -30.0f; // Adjust per computer so effects work properly
    if (fullscreen){ // Delete this outside of use for my college class
        velocityScale = 0.45f;
        opposingVelocity = -24.0f;
    }
    
    bool useServerPosition = false; // Use XY data from IP address. (Needs to be generalized outside of Jiwon's Kinect)
    std::string IPAddress = "http://10.18.248.66:3000";
    
    
    // Create window settings
    ofGLFWWindowSettings settings;
    settings.setGLVersion(3, 2);
    
    
    ///////
    // Create main/gui window
    ///////
    
    
    // Settings
    if (windows == 0) settings.setSize(width, height);
    else settings.setSize(width, height*windows);
    settings.setPosition(ofVec2f(0,0));
    settings.monitor = 0;
    
    // Create pointers to window and app for reference later and to change in-app settings
    shared_ptr<ofAppBaseWindow> mainWindow = ofCreateWindow(settings);
    shared_ptr<ofApp> mainApp(new ofApp);
    mainApp->useServerPosition = useServerPosition;
    
    
    ///////
    // Create display windows
    ///////
    
    
    // Shared settings between all windows
    settings.resizable = false;
    settings.decorated = false;
    settings.shareContextWith = mainWindow;
    
    if (fullscreen) settings.windowMode = OF_FULLSCREEN;
    else settings.setSize(width, height);
    
    // Loop to create windows, save last window reference for later
    shared_ptr<ofAppBaseWindow> lastDisplayWindow;
    
    for (int i = 0; i < windows; i++){
        if (fullscreen) settings.monitor = i+1;
        else settings.setPosition(ofVec2f(width,height*(windows-i-1)));
        
        shared_ptr<ofAppBaseWindow> displayWindow = ofCreateWindow(settings);
        shared_ptr<ofDisplay> displayApp(new ofDisplay);
        
        displayApp->main = mainApp;
        displayApp->display = i+1;
        lastDisplayWindow = displayWindow;
        
        ofRunApp(displayWindow, displayApp);
    }
    
    
    ///////
    // Share information with main window
    ///////
    
    
    if (windows == 0){
        mainApp->windowWidth = mainApp->windowHeight = mainApp->FBOwidth = mainApp->FBOheight = 0; // Detect dimensions in app
    } else{
        mainApp->windowWidth = lastDisplayWindow->getWidth();
        mainApp->windowHeight = lastDisplayWindow->getHeight();
        mainApp->FBOwidth = mainApp->windowWidth;
        mainApp->FBOheight = mainApp->windowHeight*windows;
    }
    mainApp->beat = beat;
    mainApp->velocityScaleConst = velocityScale;
    mainApp->opposingVelocityConst = opposingVelocity;
    mainApp->numParticles = particleCount;
    mainApp->address = IPAddress;
    mainApp->dancerRadius = attractorRadius;
    mainApp->frameWidth = frameWidth;
    
    
    ///////
    // Run everything!
    ///////
    
    
    ofRunApp(mainWindow, mainApp);
    
    ofSetWindowTitle("DM 6013 Final");
    ofSetEscapeQuitsApp(false);
    
    ofRunMainLoop();
    
}
