#pragma once

#include "ofMain.h"
#include "FastNoiseSIMD.h"
#include "ofEvents.h"
#include "ofxSocketIO.h"
#include "ofxSocketIOData.h"
#include "ofxSyphon.h"

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
        float* noiseSet1 = fractalNoise1->GetValueFractalSet(0, 0, frame/2, fractalRes, fractalRes, 1);
        float* noiseSet2 = fractalNoise2->GetValueFractalSet(0, 0, frame/2, fractalRes, fractalRes, 1);
        
        // Load noise data from noise sets onto vector
        for (int i = 0; i < fractalRes*fractalRes; i++){
            (*fractalVec)[i*3 + 0] = noiseSet1[i];
            (*fractalVec)[i*3 + 1] = noiseSet2[i];
            (*fractalVec)[i*3 + 2] = 0.0;
        }
        
        // Free noise sets from memory
        FastNoiseSIMD::FreeNoiseSet(noiseSet1);
        FastNoiseSIMD::FreeNoiseSet(noiseSet2);
    }
    
    // How dense the noise is
    float fractalScale = 0.01;
    // Frame for z value for noise evolution
    uint64 frame = 0;
    // Data in vector for external access
    vector<float>* fractalVec;
    
private:
    // Internal noise settings
    FastNoiseSIMD* fractalNoise1;
    FastNoiseSIMD* fractalNoise2;
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
    int FBOwidth, FBOheight;
    int windowWidth, windowHeight;
    int posX, posY, posZ;
    int numParticles;
    int textureRes;
    int fractalRes;
    int frameWidth;
    int frameWidthScaled;
    float beat;
    float dancerRadius;
    float dancerRadiusSquared;
    float timeStep;
    float velocityScale;
    float particleLife;
    float particleSize;
    float opposingVelocity;
    float windX;
    float windY;
    int phase;
    bool useServerPosition;
    bool attractToggle;
    bool phase1Fractal;
    bool notUsingGUI = false;
    
    float velocityScaleConst;
    float opposingVelocityConst;
    
    ofTrueTypeFont font;
    
    // Colors and thresholds for the densityFilter shader
    std::array<float,4> currentThresh;
    std::array<float,4> lastThresh;
    std::array<float,4> nextThresh;
    std::array<ofFloatColor,5> currentColor;
    std::array<ofFloatColor,5> lastColor;
    std::array<ofFloatColor,5> nextColor;
    
    // Timed effects, stores frame numbers. 0 means effect is not active.
    uint64 effectWindExplode = 0;
    uint64 effectQuickExplode = 0;
    uint64 effectQuickExplodeFractal = 0;
    uint64 colorChange = 0;
    
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
    
    // Music
    ofSoundPlayer music;
    
    static constexpr size_t nBandsToGet = 128;
    std::array<float, nBandsToGet> fftSmoothed{{0}};
    
    float low;
    float mid;
    float high;
    float amp;
};
