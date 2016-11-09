#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxKinect.h"
#include "ofxKinectProjectorToolkit.h"
#include "ofxOsc.h"
#include "Grid.h"

#define PROJECTOR_RESOLUTION_X 1024
#define PROJECTOR_RESOLUTION_Y 768
#define PORT 8001
#define IP_ADDRESS "127.0.0.1"
#define NGRIDS 9
#define NTRACKS 5
#define TIME_DELAY 1

using namespace ofxCv;
using namespace cv;

class ofApp : public ofBaseApp {
public:
    
    void setup();
    void update();
    void draw();
    void drawGroundWindow(ofEventArgs& args);
    void exit();
    void exitGroundWindow(ofEventArgs& args);
    
    void drawPointCloud();
    
    void keyPressed(int key);
    void mouseMoved(int x, int y);
    void mouseMovedGroundWindow(ofMouseEventArgs& args);
    void windowResized(int w, int h);
    
    void sendMessage(string m);
    
    ofxKinect kinect;
    ofxKinectProjectorToolkit kpt;
    ofxCv::ContourFinder contourFinder;
    
    ofxCvColorImage colorImg;
    
    ofxCvGrayscaleImage grayImage; // grayscale depth image
    ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
    ofxCvGrayscaleImage grayThreshFar; // the far thresholded image
    
    bool bThreshWithOpenCV;
    
    int nearThreshold;
    int farThreshold;
    
    int angle;
    
    ofPoint kinectPoint;
    ofPoint projectorPoint;
    
    //sounds
    //ofSoundPlayer planet0;
    //ofSoundPlayer planet1;
    //ofSoundPlayer planet2;
    ofSoundPlayer sounds [3][5];
    //vector<ofSoundPlayer> planet0;
    
    //osc
    ofxOscSender sender;
    ofxOscReceiver receiver;
    
    //grid
    Grid grids[NGRIDS];
    
    ofPath cursor;
    int currentPosition;
    int originalPosition;
    float startTime;
    
    void checkPoint(ofVec2f point);
};
