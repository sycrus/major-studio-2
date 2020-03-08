#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxGui.h"
#include "ofxBox2d.h"

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void mousePressed(int x, int y, int button);
    void mouseDragged(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void keyPressed(int key);
    
    ofVideoGrabber cam;
    ofxCv::ContourFinder contourFinder;
    ofColor targetColor;
    
    ofxPanel gui;
    ofParameter<float> threshold;
    ofParameter<bool> trackHs;
    ofParameter<bool> holes;
    
    vector <ofPolyline>                  lines;
    ofPolyline                           shape;
    ofxBox2d                             box2d;
    vector <shared_ptr<ofxBox2dCircle>>  circles;
    //vector <shared_ptr<ofxBox2dEdge>>    edges;
    vector <shared_ptr<ofxBox2dPolygon>> polyShapes;
};

