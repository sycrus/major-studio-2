#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"
#include "ofxBox2d.h"
#include "ofxSvg.h"
#include "ofxKinect.h"
#include "ofxWarp.h"

#define N_SOUNDS 5

class SoundData {
public:
    int     soundID;
    bool bHit;
};

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
    
    void initializeSvgs();
    void initializeKinect();
    void initializeCV();
    
    void updateKinect();
    void getMarkerLocations();
    
    void drawEnd();
    void drawContourAreas();
    
    ofPolyline svgToPolyline(ofxSVG svg);
    void polylineToWheels(ofPolyline polyline, ofPoint position);
    void polylineToSeesaws(ofPolyline polyline, ofPoint position);
    void polylineToAlternators(ofPolyline polyline, ofPoint position);
    void polylineToHammers(ofPolyline polyline, ofPoint position);
    void polylineToEnd(ofPolyline polyline, ofPoint position);
    
    void createStart(ofPoint position);
    void createEdgesContourFinder();
    void createWheel(ofPoint position);
    void createSeesaw(ofPoint position);
    void createAlternator(ofPoint position);
    void createHammer(ofPoint position);
    void createAxis(ofPoint position);
    void createEnd(ofPoint position);
    //void createStart(ofPoint position);
    
    void setupArduino(const int & version);
    void digitalPinChanged(const int & pinNum);
    void updateArduino();
    
    void contactStart(ofxBox2dContactArgs &e);
    void contactEnd(ofxBox2dContactArgs &e);
    
    ofArduino    ard;
    bool         bSetupArduino;
    
    ofxKinect               kinect;
    ofxCvGrayscaleImage     grayImage; // grayscale depth image
    ofxCvGrayscaleImage     grayThresh; // the thresholded image
    ofxCv::ContourFinder    contourFinder;
    bool                    bThreshWithOpenCV;
    bool                    bDrawPointCloud;
    int                     minBlobSize;
    int                     maxBlobSize;
    int                     minAreaRadius;
    int                     maxAreaRadius;
    int                     angle; //kinect angle
    //int                     blobArea[4];
    int                     threshold;
    bool                    showContours = false;
    bool                    showShapes = true;
    
    ofImage     drawnImg, face1, face2;
    ofxSVG      wheelSvg, seesawSvg, alternatorSvg, hammerSvg, startSvg, endSvg;
    int         wheelWidth, wheelHeight;
    
    ofxBox2d                            box2d; //world
    ofxBox2dCircle                      endAnchor;
    ofxBox2dPolygon                     endPolygon;
    ofxBox2dRevoluteJoint               endJoint;
    
    ofVec2f endPolygonPos, endAnchorPos, endJointPos;
    ofVec2f startPos;
    
    vector<ofPolyline>                          outlines;
    vector <ofPoint>                            wheels, seesaws, hammers, alternators; //locations of components
    vector <shared_ptr<ofxBox2dCircle>>         balls, axles, ssAxles, ssPins, startAxles; //for balls, axles
    vector <shared_ptr<ofxBox2dPolygon>>        obstaclesWheels, obstaclesSeesaws, obstaclesHammers; //for polyline polygons
    vector <shared_ptr<ofxBox2dEdge>>           edges, backgroundEdges; //for detected contours
    vector <shared_ptr<ofxBox2dRevoluteJoint>>  axes, ssAxes, startAxis;
    
    bool    isStart = false;
    bool    isEnd = false;
    bool    isWheel =false;
    bool    isSeesaw = false;
    bool    isClear = false;
    
    // when the ball hits we play this sound
    ofSoundPlayer           sound[N_SOUNDS];
    
    ofxPanel                gui;
    ofParameter<int>        thresh;
    ofParameter<bool>       trackHs;
    ofParameter<bool>       holes;
    ofxLabel                fps;
        
};



  

