#include "ofApp.h"
#include "ofxCv.h"
#include "CVShape.h" 

using namespace ofxCv;
using namespace cv;
int triCounter = 0;
int rectCounter = 0;

void ofApp::setup() {
    // cam.listDevices();
    cam.setDeviceID(1); //using USB webcam
    cam.setup(1280, 720);
   // ofSetVerticalSync(true);
    ofDisableAntiAliasing();
    contourFinder.setMinAreaRadius(10);
    contourFinder.setMaxAreaRadius(150);
    //contourFinder.setInvert(true); // find black instead of white
    
    // Box2d
    box2d.init();
    box2d.setGravity(0, 30);
    box2d.createGround();
    box2d.setFPS(40.0);

    //from contourFinder advanced example
    gui.setup();
    gui.add(threshold.set("Threshold", 128, 0, 255));
    gui.add(trackHs.set("Track Hue/Saturation", false));
    gui.add(holes.set("Holes", false));
}

void ofApp::update() {

    ofRemove(polyShapes, [](shared_ptr<ofxBox2dPolygon> shape) -> bool {
        return !ofRectangle(0, -400, ofGetWidth(), ofGetHeight()+400).inside(shape->getPosition());
    });
    ofRemove(circles, [](shared_ptr<ofxBox2dCircle> shape) -> bool {
        return !ofRectangle(0, -400, ofGetWidth(), ofGetHeight()+400).inside(shape->getPosition());
    });
    
    cam.update();
    if(cam.isFrameNew()) {
        contourFinder.setTargetColor(targetColor, trackHs ? TRACK_COLOR_HS : TRACK_COLOR_RGB);
        contourFinder.setThreshold(threshold);
        contourFinder.findContours(cam);
    }
    box2d.update();
}

void ofApp::draw() {
    ofSetColor(255);
    cam.draw(0, 0);
    
    for (auto &circle : circles) {
        ofFill();
        ofSetHexColor(0xc0dd3b);
        circle->draw();
    }
    
    ofFill();
    ofSetRectMode(OF_RECTMODE_CORNER);
    contourFinder.draw();
    
    ////Using <ofxBox2dPolygon>
//    for(auto & poly : polyShapes) {
//        poly->draw();
//    }
    
    ////Using <ofxBox2dEdge>
    ////Using <ofxBox2dPolygon>
        for(auto & edge : edges) {
            edge->draw();
        }
        
    
    gui.draw();
    
    ofTranslate(8, 90);
    ofFill();
    ofSetColor(0);
    ofDrawRectangle(-3, -3, 64+6, 64+6);
    ofSetColor(targetColor);
    ofDrawRectangle(0, 0, 64, 64);
}
//
void ofApp::mousePressed(int x, int y, int button) {
    targetColor = cam.getPixels().getColor(x, y);
}

void ofApp::mouseDragged(int x, int y, int button) {
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    if(key == '1') {
        auto circle = std::make_shared<ofxBox2dCircle>();
        circle->setPhysics(0.3, 0.5, 0.1);
        circle->setup(box2d.getWorld(), mouseX, mouseY, ofRandom(10, 20));
        circles.push_back(circle);
    }
    
    if(key == 'x') {
        ////Using <ofxBox2dPolygon>
//        for(int i = 0; i < contourFinder.size(); i++) {
//            ofPolyline convexHull = toOf(contourFinder.getConvexHull(i));
//            shape.addVertices(convexHull.getVertices());
//            shape.simplify();
//
//            auto poly = std::make_shared<ofxBox2dPolygon>();
//            //poly->addVertices(shape.getVertices());
//            poly->addVertices(convexHull.getVertices());
//            poly->create(box2d.getWorld());
//            polyShapes.push_back(poly);
//            shape.clear();
//        }
        ////Using <ofxBox2dEdges> WORKS
//            for (int i=0; i < contourFinder.size(); i++) {
//                auto edge = make_shared<ofxBox2dEdge>();
//                 ofPolyline convexHull = toOf(contourFinder.getConvexHull(i));
//                 //shape.addVertices(convexHull.getVertices());
//                for (int j = 0; j < convexHull.getVertices().size() ; j++) {
//                    auto pts = convexHull.getVertices();
//                    edge->addVertex(pts[j]);
//                }
//                edge->create(box2d.getWorld());
//                edges.push_back(edge);
//            }
        
            for (int i=0; i < contourFinder.size(); i++) {
                auto edge = make_shared<ofxBox2dEdge>();
                ofPolyline contourPolyline = contourFinder.getPolyline(i);
                for (int j = 0; j < contourPolyline.getVertices().size() ; j++) {
                    auto pts = contourPolyline.getVertices();
                    edge->addVertex(pts[j]);
                }
                edge->create(box2d.getWorld());
                edges.push_back(edge);
            }
    }
    
    if (key == OF_KEY_LEFT) {
        ofLog() <<"Left flipper";
    }
    if (key == OF_KEY_RIGHT) {
        ofLog()<<"Right flipper";
    }
    
    if(key == 'c') {
        edges.clear();
        circles.clear();
    }
    
}
////Using ContourFinder.h -------------------------------
