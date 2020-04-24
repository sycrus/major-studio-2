#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"
#include "ofxBox2d.h"
#include "ofxSvg.h"
#include "ofxKinect.h"
#include "ofxWarp.h"

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void exit();
    void mousePressed(int x, int y, int button);
    void mouseDragged(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void keyPressed(int key);
    void keyReleased(int key);
    
    
    void initializeKinect();
    void initializeCV();
    void initializeFlippers();
    void drawFlippers();
    ofPolyline svgToPolyline(ofxSVG svg);
    void polylineToPolygon(ofPolyline poly);
    //shared_ptr<ofxBox2dPolygon> loadSavedShape(const std::string& file);
    
    //ofVideoGrabber cam; //rgb webcam
    ofxCv::ContourFinder contourFinder;
    ofColor targetColor;
    
    ofxSVG wheelSvg;
    int wheelWidth, wheelHeight;
    ofPolyline wheelPolyline;
    
    ofxPanel gui;
    ofParameter<float> threshold;
    ofParameter<bool> trackHs;
    ofParameter<bool> holes;
    ofxLabel fps;
    
    ofxBox2d                             box2d; //world
    ofxBox2dCircle                       leftAnchor, rightAnchor;
    ofxBox2dRect                         leftFlipper, rightFlipper;
    ofxBox2dRevoluteJoint                leftFlipperJoint, rightFlipperJoint;
    
    ofVec2f leftFlipperPos, rightFlipperPos;
    ofVec2f leftAnchorPos, rightAnchorPos;
    

    vector <shared_ptr<ofxBox2dCircle>>  circles; //for balls
    vector <shared_ptr<ofxBox2dPolygon>>  obstacles;
    vector <shared_ptr<ofxBox2dEdge>>    edges; //for detected contours

    ofxKinect kinect;
    
    ofxCvGrayscaleImage grayImage; // grayscale depth image
    ofxCvGrayscaleImage grayThresh; // the thresholded image
    
    //ofxCv::ContourFinder contourFinder;
    //ofColor targetColor;
    //ofxCvContourFinder contourFinder;
    
    bool bThreshWithOpenCV;
    bool bDrawPointCloud;
    
    //int threshold;
    int minBlobSize;
    int maxBlobSize;
    int minAreaRadius;
    int maxAreaRadius;
        
    int angle;
    
    int blobArea[4];
        
};



  

