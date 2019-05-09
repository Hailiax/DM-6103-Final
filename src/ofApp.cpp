#include "ofApp.h"

//--------------------------------------------------------------

//
// Note: there are computer specific settings for: OpenGL 3, SIMD 4.1, and Apple Clang
// XY positions [0,1] are received from kinect/software via websockets on a local server
//

void ofApp::setup(){
    // Width and Heigth of the windows
    width = ofGetWindowWidth();
    height = ofGetWindowHeight()*2;
    frameMesh.setUsage(GL_DYNAMIC_DRAW);
    
    // Set variables
    beat = 5; // How many "bangs" per minute. Bpm divided by measures/min divided by beats/measure?
    particleSize = 1.5f;
    particleLife = 15.0f;
    velocityScaleConst = 0.5f; // Adjust for speed ( 0.45f on live )
    opposingVelocityConst = -30.0f; // Adjust so effects work properly ( 25.0f on live )
    timeStep = 0.005f;
    numParticles = 200000; // Turn up as much as possible, 500000 decent, comment in AVX on FastNoiseSIMD.h if possible
    dancerRadiusSquared = 100*100; // Controlled somewhere else
    frameWidth = 25;
    phase = 1;
    attractToggle = true;
    phase1Fractal = false;
    
    beat = beat*M_PI/360; // Convert for input to SIN()
    velocityScale = velocityScaleConst;
    opposingVelocity = opposingVelocityConst/60.0;
    
    // Set default colors/thresh for densityFilter shader
    sThresh[0] = 0.9625;
    sThresh[1] = 0.5;
    sThresh[2] = 0.1;
    sThresh[3] = 0.01875;
    
    sColor[0] = 0xFFFFFF;
    sColor[1] = 0xFCECA3;
    sColor[2] = 0xA13B4F;
    sColor[3] = 0x181F54;
    sColor[4] = 0x000000;
    
    // Seting the textures where the information ( position and velocity ) will be
    textureRes = (int)sqrt((float)numParticles);
    numParticles = textureRes * textureRes;
    
    // Loading the Shaders
    updatePos.load("passthru.vert", "posUpdate.frag"); // shader for updating the texture that store the particles position on RG channels
    updateVel.load("passthru.vert", "velUpdate.frag"); // shader for updating the texture that store the particles velocity on RG channels
    updateAge.load("passthru.vert", "ageUpdate.frag"); // shader for updating the texture that store the particles effective age (for alternative lifespans) on R, true age on G, and original age on B.
    densityFilter.load("passthru.vert", "densityFilter.frag");// Shader that converts particles into flatenned image based on desnity of rendered particles
    blurX.load("passthru.vert", "blurX.frag"); // Shaders for glow
    blurY.load("passthru.vert", "blurY.frag");
    glowAdd.load("passthru.vert", "glowAdd.frag");
    
    // Frag, Vert and Geo shaders for the rendering process of the particles
    updateRender.setGeometryInputType(GL_POINTS);
    updateRender.setGeometryOutputType(GL_TRIANGLE_STRIP);
    updateRender.setGeometryOutputCount(6);
    updateRender.load("render.vert","render.frag","render.geom");
    
    // 1. Making arrays of float pixels with position information
    vector<float> pos(numParticles*3);
    for (int x = 0; x < textureRes; x++){
        for (int y = 0; y < textureRes; y++){
            int i = textureRes * y + x;
            
            pos[i*3 + 0] = ofRandom(1.0); //x*offset;
            pos[i*3 + 1] = ofRandom(1.0); //y*offset;
            pos[i*3 + 2] = 0.0f;
        }
    }
    // Load this information in to the FBO's texture
    posPingPong.allocate(textureRes, textureRes, GL_RGB32F);
    origPos.allocate(textureRes, textureRes, GL_RGB32F);
    origPos.getTexture().loadData(pos.data(), textureRes, textureRes, GL_RGB);
    posPingPong.src->getTexture().loadData(pos.data(), textureRes, textureRes, GL_RGB);
    
    // 2. Making arrays of float pixels with velocity information and the load it to a texture
    vector<float> vel(numParticles*3);
    for (int i = 0; i < numParticles; i++){
        vel[i*3 + 0] = ofRandom(-1.0,1.0);
        vel[i*3 + 1] = ofRandom(-1.0,1.0);
        vel[i*3 + 2] = 1.0f;
    }
    // Load this information in to the FBO's texture
    velPingPong.allocate(textureRes, textureRes, GL_RGB32F);
    origVel.allocate(textureRes, textureRes, GL_RGB32F);
    origVel.getTexture().loadData(vel.data(), textureRes, textureRes, GL_RGB);
    velPingPong.src->getTexture().loadData(vel.data(), textureRes, textureRes, GL_RGB);
    
    // 3. Making arrays of float pixels with age information and the load it to a texture
    vector<float> age(numParticles*3);
    for (int i = 0; i < numParticles; i++){
        float a = 1.0*i/numParticles*(particleLife+timeStep) - timeStep;
        age[i*3 + 0] = a;
        age[i*3 + 1] = a;
        age[i*3 + 2] = a;
    }
    // Load this information in to the FBO's texture
    agePingPong.allocate(textureRes, textureRes, GL_RGB32F);
    agePingPong.src->getTexture().loadData(age.data(), textureRes, textureRes, GL_RGB);
    
    // Loading and setings of the variables of the textures of the particles
    particleImg.load("dot.png");
    
    // Allocate the particle renderer
    renderFBO.allocate(width, height, GL_RGB32F);
    renderFBO.begin();
    ofClear(0, 0, 0, 255);
    renderFBO.end();
    
    mesh.setMode(OF_PRIMITIVE_POINTS);
    for(int x = 0; x < textureRes; x++){
        for(int y = 0; y < textureRes; y++){
            mesh.addVertex(ofVec3f(x,y));
            mesh.addTexCoord(ofVec2f(x, y));
        }
    }
    
    // Allocate some effects
    effectsPingPong.allocate(width, height, GL_RGB32F);
    glowAddFBO.allocate(width, height, GL_RGB32F);
    
    // Create fractal noise map array, values are [-1, 1]
    fractalRes = width;
    if (height > width) fractalRes = height;
    fractal.allocate(fractalRes, fractalRes, GL_RGB32F);
    fractalGenerator.setup(fractalRes);
    fractalGenerator.startThread();
    
    // Setup sockets
    isConnected = false;
    address = "http://192.168.1.20:3000";
    status = "not connected";
    
    socketIO.setup(address);
    
    ofAddListener(socketIO.notifyEvent, this, &ofApp::gotEvent);
    
    ofAddListener(socketIO.connectionEvent, this, &ofApp::onConnection);
}

//--------------------------------------------------------------
void ofApp::update(){
    // Use mouse coordinates
    posX = mouseX;
    posY = mouseY + height/2;
    
    // Create new fractal on different thread
    if (!fractalGenerator.isThreadRunning()){
        fractal.getTexture().loadData(fractalGenerator.fractalVec->data(), fractalRes, fractalRes, GL_RGB);
        fractalGenerator.startThread();
    }
    
    // Update ages
    agePingPong.dst->begin();
    ofClear(0);
    updateAge.begin();
    updateAge.setUniformTexture("prevAgeData", agePingPong.src->getTexture(), 0);   // passing the previus age information
    updateAge.setUniform1f("timestep", (float)timeStep);
    updateAge.setUniform1f("life", particleLife);
    updateAge.setUniform1i("phase", (int)phase);
    
    agePingPong.src->draw(0, 0);
    
    updateAge.end();
    agePingPong.dst->end();
    
    agePingPong.swap();
    
    
    // Update velocities
    velPingPong.dst->begin();
    ofClear(0);
    updateVel.begin();
    updateVel.setUniformTexture("backbuffer", velPingPong.src->getTexture(), 0);   // passing the previus velocity information
    updateVel.setUniformTexture("origVelData", origVel.getTexture(), 1);  // passing the position information
    updateVel.setUniformTexture("posData", posPingPong.src->getTexture(), 2);  // passing the position information
    updateVel.setUniformTexture("ageData", agePingPong.src->getTexture(), 3);  // passing the position information
    updateVel.setUniform2f("screen", (float)width, (float)height);
    updateVel.setUniform2f("mouse", (float)posX, (float)posY);
    updateVel.setUniform1f("dancerRadiusSquared", (float)dancerRadiusSquared);
    updateVel.setUniform1f("timestep", (float)timeStep);
    updateVel.setUniform1f("opposingVelocity", (float)opposingVelocity);
    updateVel.setUniform1i("phase", (int)phase);
    updateVel.setUniform1i("attractToggle", (int)attractToggle);
    
    velPingPong.src->draw(0, 0);
    
    updateVel.end();
    velPingPong.dst->end();
    
    velPingPong.swap();
    
    
    // Update positions
    posPingPong.dst->begin();
    ofClear(0);
    updatePos.begin();
    updatePos.setUniformTexture("prevPosData", posPingPong.src->getTexture(), 0); // Previus position
    updatePos.setUniformTexture("origPosData", origPos.getTexture(), 1);  // passing the position information
    updatePos.setUniformTexture("velData", velPingPong.src->getTexture(), 2);  // Velocity
    updatePos.setUniformTexture("ageData", agePingPong.src->getTexture(), 3);  // Age
    updatePos.setUniformTexture("fractalData", fractal.getTexture(), 4);  // Fractal
    updatePos.setUniform2f("screen", (float)width, (float)height);
    updatePos.setUniform1f("timestep", (float) timeStep);
    updatePos.setUniform1f("velocityScale", (float)velocityScale);
    updatePos.setUniform1i("phase", (int)phase);
    updatePos.setUniform1i("phase1Fractal", (int)phase1Fractal);
    
    posPingPong.src->draw(0, 0);
    
    updatePos.end();
    posPingPong.dst->end();
    
    posPingPong.swap();
    
    
    // Rendering
    //
    // 1.   Sending this vertex to the Vertex Shader.
    //      Each one it's draw exactly on the position that match where it's stored on both vel and pos textures
    //      So on the Vertex Shader (that's is first at the pipeline) can search for it information and move it
    //      to it right position.
    // 2.   Once it's in the right place the Geometry Shader make 6 more vertex in order to make a billboard
    // 3.   that then on the Fragment Shader is going to be filled with the pixels of sparkImg texture
    //
    renderFBO.begin();
    ofClear(0,0,0,0);
    updateRender.begin();
    updateRender.setUniformTexture("posTex", posPingPong.dst->getTexture(), 0);
    updateRender.setUniformTexture("imgTex", particleImg.getTexture() , 1);
    updateRender.setUniform1i("resolution", (float)textureRes);
    updateRender.setUniform2f("screen", (float)width, (float)height);
    updateRender.setUniform1f("particleSize", (float)particleSize);
    updateRender.setUniform1f("imgWidth", (float)particleImg.getWidth());
    updateRender.setUniform1f("imgHeight", (float)particleImg.getHeight());
    
    ofPushStyle();
    ofEnableBlendMode( OF_BLENDMODE_ADD );
    ofSetColor(255);
    
    mesh.draw();
    
    ofDisableBlendMode();
    glEnd();
    
    updateRender.end();
    renderFBO.end();
    ofPopStyle();
    
    // Transfer rendered particles on renderFBO to effectsPingPong src FBO
    effectsPingPong.src->begin();
    ofClear(0);
    renderFBO.draw(0,0);
    effectsPingPong.src->end();
    
    
    ///////
    // Apply effects
    ///////
    
    // DensityFilter effect
    effectsPingPong.dst->begin();
    ofClear(0);
    densityFilter.begin();
    densityFilter.setUniformTexture("fractalData", fractal.getTexture(), 1);
    densityFilter.setUniform1f("frameNum", (float)sin(ofGetFrameNum()));
    
    densityFilter.setUniform1f("thresh1", (float)sThresh[0]);
    densityFilter.setUniform1f("thresh2", (float)sThresh[1]);
    densityFilter.setUniform1f("thresh3", (float)sThresh[2]);
    densityFilter.setUniform1f("thresh4", (float)sThresh[3]);
    
    densityFilter.setUniform4f("color1", ofFloatColor::fromHex(sColor[0]));
    densityFilter.setUniform4f("color2", ofFloatColor::fromHex(sColor[1]));
    densityFilter.setUniform4f("color3", ofFloatColor::fromHex(sColor[2]));
    densityFilter.setUniform4f("color4", ofFloatColor::fromHex(sColor[3]));
    densityFilter.setUniform4f("color5", ofFloatColor::fromHex(sColor[4]));
    
    effectsPingPong.src->draw(0,0);
    densityFilter.end();
    effectsPingPong.dst->end();
    effectsPingPong.swap();

//    glowAddFBO.begin(); // Hold current frame in FBO for glow effect
//    ofClear(0);
//    effectsPingPong.src->draw(0,0);
//    glowAddFBO.end();
//
//    effectsPingPong.dst->begin(); // Blur X
//    ofClear(0);
//    blurX.begin();
//    blurX.setUniform1f("blurAmount", 8.0);
//    effectsPingPong.src->draw(0,0);
//    blurX.end();
//    effectsPingPong.dst->end();
//    effectsPingPong.swap();
//
//    effectsPingPong.dst->begin(); // Blur Y
//    ofClear(0);
//    blurY.begin();
//    blurY.setUniform1f("blurAmount", 8.0);
//    effectsPingPong.src->draw(0,0);
//    blurY.end();
//    effectsPingPong.dst->end();
//    effectsPingPong.swap();
//
//    effectsPingPong.dst->begin(); // Combine blur and stored renderFBO for glow
//    ofClear(0);
//    glowAdd.begin();
//    glowAdd.setUniformTexture("sharpTex", glowAddFBO.getTexture() , 1);
//    effectsPingPong.src->draw(0,0);
//    glowAdd.end();
//    effectsPingPong.dst->end();
//    effectsPingPong.swap();
    
    
    
    ///////
    // Live effects
    ///////
    
    // Oscillate frame
    float beatMult = pow( sin(ofGetFrameNum()*beat), 2);
    dancerRadiusSquared = beatMult*40 + 20;
    dancerRadiusSquared *= dancerRadiusSquared;
    frameWidth = beatMult*10 + 20;
    
    // Timed effects
    /*if (effectWindExplode){
        unsigned int f = ofGetFrameNum() - effectWindExplode;
        if (f == 0){
            phase = 2;
            opposingVelocity = opposingVelocityConst;
        } else if (f == 45){
            phase = 1;
            opposingVelocity = opposingVelocityConst/60;
            
            effectWindExplode = 0;
        }
    }*/
    if (effectQuickExplode){
        // get frames since effect called
        unsigned int f = ofGetFrameNum() - effectQuickExplode;
        if (f == 0){
            // Eject particles from center by increasing opposing velocity
            opposingVelocity = opposingVelocityConst/2.0;
        } else if (f == 5){
            // Reset opposing velocity
            opposingVelocity = opposingVelocityConst/60.0;
            // Reset effect timer and turn off this effect
            effectQuickExplode = 0;
        }
    }
    if (effectQuickExplodeFractal){
        unsigned int f = ofGetFrameNum() - effectQuickExplodeFractal;
        if (f == 0){
            phase1Fractal = true;
            opposingVelocity = opposingVelocityConst/2.0;
        } else if (f == 5){
            opposingVelocity = opposingVelocityConst/60.0;
        } else if (f == 10){
            phase1Fractal = false;
            
            effectQuickExplodeFractal = 0;
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    
    // Draw 1st half of image
    effectsPingPong.src->getTexture().drawSubsection(0,0,width,height/2,0,height/2);
    
    // Draw frame
    int fWidth = ofGetWindowWidth();
    int fHeight = ofGetWindowHeight();
    frameMesh.clear();
    frameMesh.setMode(OF_PRIMITIVE_TRIANGLES);
    
    frameMesh.addVertex(ofVec3f(0, 0, 0));
    frameMesh.addVertex(ofVec3f(fWidth, 0, 0));
    frameMesh.addVertex(ofVec3f(fWidth, fHeight, 0));
    frameMesh.addVertex(ofVec3f(0, fHeight, 0));
    
    frameMesh.addVertex(ofVec3f(frameWidth, frameWidth, 0));
    frameMesh.addVertex(ofVec3f(fWidth-frameWidth, frameWidth, 0));
    frameMesh.addVertex(ofVec3f(fWidth-frameWidth, fHeight-frameWidth, 0));
    frameMesh.addVertex(ofVec3f(frameWidth, fHeight-frameWidth, 0));
    
    frameMesh.addColor(ofColor::white);
    frameMesh.addColor(ofColor::white);
    frameMesh.addColor(ofColor::white);
    frameMesh.addColor(ofColor::white);
    frameMesh.addColor(ofColor::white);
    frameMesh.addColor(ofColor::white);
    frameMesh.addColor(ofColor::white);
    frameMesh.addColor(ofColor::white);
    
    frameMesh.addIndex(0);
    frameMesh.addIndex(1);
    frameMesh.addIndex(4);
    
    frameMesh.addIndex(4);
    frameMesh.addIndex(1);
    frameMesh.addIndex(5);
    
    frameMesh.addIndex(1);
    frameMesh.addIndex(2);
    frameMesh.addIndex(5);
    
    frameMesh.addIndex(5);
    frameMesh.addIndex(2);
    frameMesh.addIndex(6);
    
    frameMesh.addIndex(2);
    frameMesh.addIndex(3);
    frameMesh.addIndex(6);
    
    frameMesh.addIndex(6);
    frameMesh.addIndex(3);
    frameMesh.addIndex(7);
    
    frameMesh.addIndex(3);
    frameMesh.addIndex(0);
    frameMesh.addIndex(7);
    
    frameMesh.addIndex(7);
    frameMesh.addIndex(0);
    frameMesh.addIndex(4);
    
    frameMesh.draw();
    
    // log status of sockets
    cout << ofApp::status << endl;
    
    // Second half is drawn in ofSecond
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    // Phases
    if (key == '1')
        phase = 1;
    else if (key == '2')
        phase = 2;
    else if (key == '3')
        phase = 3;
    
    // Effects
    /*else if (key == 'q')
        effectWindExplode = ofGetFrameNum();*/
    else if (key == 'q')
        effectQuickExplode = ofGetFrameNum();
    else if (key == 'w')
        effectQuickExplodeFractal = ofGetFrameNum();
    
    // Modifiers
    else if (key == 'a'){ // Normal attraction
        phase1Fractal = false;
        velocityScale = velocityScaleConst;
    } else if (key == 's'){ // Anti attraction
        phase1Fractal = true;
        velocityScale = -velocityScaleConst;
    } else if (key == 'd'){ // Normal attraction
        attractToggle = true;
    } else if (key == 'f'){ // Paused attraction
        attractToggle = false;
    } // phase 3 play with effects try adding random side velocities or mod y position to multiply x vel
    
    // Change colors/thresholds in densityFilter
    else if (key == 'z'){
        sThresh[0] = 0.9625;
        sThresh[1] = 0.5;
        sThresh[2] = 0.1;
        sThresh[3] = 0.01875;
        sColor[0] = 0xFFFFFF;
        sColor[1] = 0xFCECA3;
        sColor[2] = 0xA13B4F;
        sColor[3] = 0x181F54;
        sColor[4] = 0x000000;
    } else if (key == 'x'){
        sThresh[0] = 0.9625;
        sThresh[1] = 0.5;
        sThresh[2] = 0.8;
        sThresh[3] = 0.01;
        sColor[0] = 0xFFFFFF;
        sColor[1] = 0x37FFF6;
        sColor[2] = 0x3AFF37;
        sColor[3] = 0xFCFF37;
        sColor[4] = 0xFCFF00;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}

// Socket stuff
void ofApp::onConnection () {
    isConnected = true;
    bindEvents();
}

void ofApp::bindEvents () {
    std::string serverEventName = "kinectData";
    socketIO.bindEvent(serverEvent, serverEventName);
    ofAddListener(serverEvent, this, &ofApp::onServerEvent);
}

//--------------------------------------------------------------
void ofApp::gotEvent(string& name) {
    status = name;
}

//--------------------------------------------------------------
void ofApp::onServerEvent (ofxSocketIOData& data) {
    posX = data.getFloatValue("x")*width;
    posY = (data.getFloatValue("y") + 1)*height/2;
}
