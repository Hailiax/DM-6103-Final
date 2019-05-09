#pragma once

#include "ofMain.h"
#include "FastNoiseSIMD.h"
#include "ofEvents.h"
#include "ofxSocketIO.h"
#include "ofxSocketIOData.h"

// Run fractal generation on separate thread
class fractalGenerationThread : public ofThread{
public:
    void setup(int res){
        // Setup vector using resolution
        fractalRes = res;
        fractalVec = new vector<float>(fractalRes*fractalRes*3);
        
        // Settings for noisemap 1
        fractalNoise1 = FastNoiseSIMD::NewFastNoiseSIMD();
        fractalNoise1->SetFrequency(fractalScale);
        fractalNoise1->SetSeed(1000);
        
        // Settings for noisemap 2
        fractalNoise2 = FastNoiseSIMD::NewFastNoiseSIMD();
        fractalNoise2->SetFrequency(fractalScale);
        fractalNoise2->SetSeed(2000);
    }
    
    void threadedFunction(){
        // Generate noise sets from settings
        float* noiseSet1 = fractalNoise1->GetValueFractalSet(0, 0, z, fractalRes, fractalRes, 1);
        float* noiseSet2 = fractalNoise2->GetValueFractalSet(0, 0, z, fractalRes, fractalRes, 1);
        
        // Load noise data from noise sets onto vector
        for (int i = 0; i < fractalRes*fractalRes; i++){
            (*fractalVec)[i*3 + 0] = noiseSet1[i];
            (*fractalVec)[i*3 + 1] = noiseSet2[i];
            (*fractalVec)[i*3 + 2] = 0.0;
        }
        
        // Free noise sets from memory
        FastNoiseSIMD::FreeNoiseSet(noiseSet1);
        FastNoiseSIMD::FreeNoiseSet(noiseSet2);
        
        // Increment on Z space to evolve fractal
        z += 10;
    }
    
    // How dense the noise is
    float fractalScale = 0.01;
    // Data in vector for external access
    vector<float>* fractalVec;
    
private:
    // Internal noise settings
    FastNoiseSIMD* fractalNoise1;
    FastNoiseSIMD* fractalNoise2;
    
    // Internal noise setting data
    uint64 z = 0;
    int fractalRes;
};

struct pingPongBuffer {
public:
    void allocate( int _width, int _height, int _internalformat = GL_RGBA){
        // Allocate
        for(int i = 0; i < 2; i++){
            FBOs[i].allocate(_width,_height, _internalformat );
            FBOs[i].getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
        }
        
        //Assign
        src = &FBOs[0];
        dst = &FBOs[1];
        
        // Clean
        clear();
    }
    
    void swap(){
        std::swap(src,dst);
    }
    
    void clear(){
        for(int i = 0; i < 2; i++){
            FBOs[i].begin();
            ofClear(0,255);
            FBOs[i].end();
        }
    }
    
    ofFbo& operator[]( int n ){ return FBOs[n];}
    ofFbo   *src;       // Source       ->  Ping
    ofFbo   *dst;       // Destination  ->  Pong
    
private:
    ofFbo   FBOs[2];    // Real addresses of ping/pong FBO«s
};


class ofApp : public ofBaseApp{
public:
    // Basic of functions
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    // Create fractal generator
    fractalGenerationThread fractalGenerator;
    
    // Basic variables
    int width, height;
    int posX, posY;
    int numParticles;
    int textureRes;
    int fractalRes;
    int frameWidth;
    float beat;
    float dancerRadiusSquared;
    float timeStep;
    float velocityScale;
    float particleLife;
    float particleSize;
    float opposingVelocity;
    int phase;
    bool attractToggle;
    bool phase1Fractal;
    
    float velocityScaleConst;
    float opposingVelocityConst;
    
    // Colors and thresholds for the densityFilter shader
    float sThresh[4];
    uint32 sColor[5];
    
    // Timed effects, stores frame numbers. 0 means effect is not active.
    uint64 effectWindExplode = 0;
    uint64 effectQuickExplode = 0;
    uint64 effectQuickExplodeFractal = 0;
    uint64 xDistort = 0;
    
    // Shaders
    ofShader updatePos;
    ofShader updateVel;
    ofShader updateAge;
    ofShader densityFilter;
    ofShader blurX;
    ofShader blurY;
    ofShader glowAdd;
    
    // FBO Ping Pongs
    pingPongBuffer posPingPong;
    pingPongBuffer velPingPong;
    pingPongBuffer agePingPong;
    pingPongBuffer effectsPingPong;
    
    // Stores initial pos and vel
    ofFbo origPos;
    ofFbo origVel;
    
    // Other FBOs
    ofFbo fractal;
    ofFbo glowAddFBO;
    
    // Rendering stuff
    ofImage particleImg;
    ofShader updateRender;
    ofFbo renderFBO;
    ofVboMesh mesh;
    
    ofVboMesh frameMesh;
    
    // Socket stuff
    void gotEvent(std::string& name);
    void onServerEvent(ofxSocketIOData& data);
    
    ofxSocketIO socketIO;
    
    bool isConnected;
    void onConnection();
    void bindEvents();
    ofEvent<ofxSocketIOData&> serverEvent;
    
    std::string address;
    std::string status;
};
