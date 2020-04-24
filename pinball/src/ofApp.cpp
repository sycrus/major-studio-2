#include "ofApp.h"
#include "ofxCv.h"


using namespace ofxCv;
using namespace cv;

ofColor white = (255, 255, 255);
ofColor navy, salmon, rubberDuck, babyBlue; //palette 1

bool isLeftFlipperActive = false;
bool isRightFlipperActive = false;
bool isGui = false;

void ofApp::setup() {
    ofSetBackgroundColor(0,0,0); //sets background to black
    ofSetFrameRate(60);
     
    initializeKinect();
    initializeCV();
    
    wheelSvg.load("wheel.svg");
    wheelPolyline = svgToPolyline(wheelSvg);
    ofRectangle rect = wheelPolyline.getBoundingBox();
    wheelWidth = rect.getWidth();
    wheelHeight = rect.getHeight();
    
    // set colors
    navy.setHex(0x21243d);
    salmon.setHex(0xff7c7c);
    rubberDuck.setHex(0xffd082);
    babyBlue.setHex(0x88e1f2);
    
    // Box2d
    box2d.init();
    box2d.setGravity(0, 30);
    box2d.createGround();
    box2d.setFPS(40.0);
    
    //pinball
    initializeFlippers();
    
//    gui.setup();
//    gui.add(threshold.set("Threshold", 225, 0, 255));
}

void ofApp::update() {
    
    //remove off-screen circles
    ofRemove(circles, [](shared_ptr<ofxBox2dCircle> shape) -> bool {
        return !ofRectangle(0, -400, ofGetWidth(), ofGetHeight()+400).inside(shape->getPosition());
    });
    
    ofFill();
    ofSetColor(navy);
    
    kinect.update();
    if(kinect.isFrameNew()) {
        
        grayImage.setFromPixels(kinect.getPixels());
        grayImage.resize(ofGetWidth(), ofGetHeight());
        grayImage.scale(1.4, 1.2);
        grayThresh = grayImage;
        
        grayThresh.resize(ofGetWidth(), ofGetHeight());
        grayThresh.scale(1.4, 1.2);
        grayThresh.blurGaussian(1.0f);
        grayThresh.threshold(threshold);
        
        // update the cv images
        grayImage.flagImageChanged();
        contourFinder.findContours(grayThresh);
    }
    
    box2d.update();
    
    if (isLeftFlipperActive) {
        leftFlipper.addRepulsionForce(leftFlipperPos.x , leftFlipperPos.y + 100 , 40.0);
    }
    if (isRightFlipperActive) {
        rightFlipper.addRepulsionForce(rightFlipperPos.x, rightFlipperPos.y + 100, 40.0);
    }
    
}

void ofApp::draw() {
    ofSetColor(255);
    ofFill();
    
    edges.clear();
    obstacles.clear();
    
    for (int i=0; i < contourFinder.size(); i++) {
        auto edge = make_shared<ofxBox2dEdge>();
        ofPolyline contourPolyline = contourFinder.getPolyline(i);
        contourPolyline.simplify(2);
        
        for (int j = 0; j < contourPolyline.getVertices().size() ; j++) {
            auto pts = contourPolyline.getVertices();
            blobArea[i] = abs(contourPolyline.getArea());
            ofSetColor(salmon);
            int roundedBlobArea = blobArea[i]/100;
            //ofDrawCircle(contourPolyline.getCentroid2D().x, contourPolyline.getCentroid2D().y, 10);
            ofDrawBitmapString(100 * roundedBlobArea, contourPolyline.getCentroid2D().x, contourPolyline.getCentroid2D().y);
            
            if (roundedBlobArea > 48 && roundedBlobArea < 52) { //square
                //ofDrawBitmapString("Square", contourPolyline.getCentroid2D().x, contourPolyline.getCentroid2D().y);
            }
            if (roundedBlobArea < 37 && roundedBlobArea > 30) { //triangle
                //ofDrawBitmapString("Triangle", contourPolyline.getCentroid2D().x, contourPolyline.getCentroid2D().y);
                
                //store coordinates
                //draw at those coordinates
                //check if the coordinates have changed much -> isframenew?
                //isframenew == true, redraw
                
                ofDrawCircle(contourPolyline.getCentroid2D().x, contourPolyline.getCentroid2D().y, 2);
                
                wheelPolyline.translate(ofVec2f(contourPolyline.getCentroid2D().x - wheelWidth, contourPolyline.getCentroid2D().y - wheelHeight));
                wheelPolyline.draw();
                polylineToPolygon(wheelPolyline);
            }
            if (roundedBlobArea > 43 && roundedBlobArea < 47) {//circle
                //ofDrawBitmapString("Circle", contourPolyline.getCentroid2D().x, contourPolyline.getCentroid2D().y);
            }
            edge->addVertex(pts[j]);
        }
        edge->create(box2d.getWorld());
        edges.push_back(edge);
    }
    
    drawFlippers();
   
    //just displays
//    ofPolyline wheel = svgToPolyline(wheelSvg);
//    int shapeX = wheel.getCentroid2D().x;
//    int shapeY = wheel.getCentroid2D().y;
//    ofDrawCircle(ofGetWidth()/2, ofGetHeight()/2, 2);
//    wheel.translate(ofVec2f(ofGetWidth()/2 - shapeX, ofGetHeight()/2 - shapeY));
//    wheel.draw();

    //for detected objects
    for(auto & edge : edges) {
        ofFill();
        ofSetColor(salmon);
        edge->draw();
    }
    
    for(auto & obstacle : obstacles) {
        ofNoFill();
        obstacle->draw();
    }

    //for ball
    for (auto &circle : circles) {
        ofFill();
        ofSetHexColor(0xc0dd3b);
        circle->draw();
    }
    if (isGui) {
        gui.draw();
    }
    stringstream reportStream;
       
   ofSetColor(white);
   reportStream << "FPS: " << ofGetFrameRate() << endl;
    reportStream << "Threshold: " << threshold << endl;
    reportStream << "Angle: " << angle << endl;
    reportStream << "Min Area: " << minAreaRadius << endl;
    reportStream << "Max Area: " << maxAreaRadius << endl;
    reportStream << "Wheel: " << wheelPolyline.isClosed()   << endl;
   ofDrawBitmapString(reportStream.str(), 20, 652);

}

void ofApp::exit() {
}

void ofApp::mousePressed(int x, int y, int button) {
}

void ofApp::mouseDragged(int x, int y, int button) {
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    if(key == 'f') {
        ofToggleFullscreen();
    }
    if(key == ' ') {
       //set all positions and create box2d objects
    }
    
    // drops a ball
    if(key == '0') {
        auto circle = std::make_shared<ofxBox2dCircle>();
        circle->setPhysics(1.0, 0.2, 0.1); //density bounce friction
        //circle->setup(box2d.getWorld(), ofGetWidth()/2, 10, 12);
        circle->setup(box2d.getWorld(), mouseX, 10, 12);
        circles.push_back(circle);
    }
    
    if (key == OF_KEY_LEFT) {
        isLeftFlipperActive = true;
    }
    if (key == OF_KEY_RIGHT) {
        isRightFlipperActive = true;
    }
    //KINECT ------------------------------------------------------------------
    // Kinect : Adjust Angle
    if (key == OF_KEY_UP) {
        angle++;
        if(angle>30) angle=30;
        kinect.setCameraTiltAngle(angle);
    }
    if (key == OF_KEY_DOWN) {
        angle--;
        if(angle<-30) angle=-30;
        kinect.setCameraTiltAngle(angle);
    }
    
    // Kinect : Change Threshold
    if (key == '=') {
        threshold++;
        if (threshold > 255) threshold = 255;
        cout<< "Threshold: " << threshold <<endl;
    }
    if (key == '-') {
       threshold--;
       if (threshold < 0) threshold = 0;
        cout<< "Threshold: " << threshold <<endl;
    }
    
    // Kinect : Change min Blob Size
    if(key == '.') {
        minAreaRadius ++;
        contourFinder.setMinAreaRadius(minAreaRadius);
    }
    if(key == ',') {
        minAreaRadius --;
        contourFinder.setMinAreaRadius(minAreaRadius);
    }
    // Kinect : Change max Blob Size
    if(key == ';') {
        maxAreaRadius ++;
        contourFinder.setMaxAreaRadius(maxAreaRadius);
    }
    if(key == 'l') {
        maxAreaRadius --;
        contourFinder.setMaxAreaRadius(maxAreaRadius);
    }
    //KINECT ------------------------------------------------------------------
    
    if(key == 'c') {
        edges.clear();
        circles.clear();
    }
    
    if(key == 'h') {
        isGui = !isGui;
    }
}

void ofApp::keyReleased(int key) {
    if (key == OF_KEY_LEFT) {
        isLeftFlipperActive = false;
    }
    if (key == OF_KEY_RIGHT) {
        isRightFlipperActive = false;
    }
}

void ofApp::initializeKinect() {
    // enable depth->video image calibration
    kinect.setRegistration(true);
    
    kinect.init();
    kinect.init(true); // shows infrared instead of RGB video image
    kinect.open();        // opens first available kinect
    // zero the tilt on startup
    angle = -9;
    kinect.setCameraTiltAngle(angle);
}

void ofApp::initializeCV() {
    minAreaRadius = 30;
    maxAreaRadius = 50;
    threshold = 225;
    bThreshWithOpenCV = true;

    contourFinder.setMinAreaRadius(minAreaRadius);
    contourFinder.setMaxAreaRadius(maxAreaRadius);
    
    grayImage.allocate(kinect.width, kinect.height);
    grayThresh.allocate(kinect.width, kinect.height);
    grayImage.resize(ofGetWidth(), ofGetHeight());
    grayImage.scale(1.4, 1.2);
    grayThresh = grayImage;
    grayThresh.resize(ofGetWidth(), ofGetHeight());
    grayThresh.scale(1.4, 1.2);
}

void ofApp::initializeFlippers() {
    leftFlipperPos.set(ofGetWidth()/2 - 80, ofGetHeight() - 150);
    leftFlipper.setPhysics(1.0, 0.5, 0.5);
    leftFlipper.setup(box2d.getWorld(), leftFlipperPos.x, leftFlipperPos.y, 150, 20);
    
    leftAnchorPos.set(leftFlipperPos.x - 75, leftFlipperPos.y);
    leftAnchor.setup(box2d.getWorld(), leftAnchorPos, 10);

    leftFlipperJoint.setWorld(box2d.getWorld());
    
    leftFlipperJoint.setup(leftAnchor.body, leftFlipper.body, 0, 0);
    leftFlipperJoint.setLimits(-ofDegToRad(35), ofDegToRad(35));
    
    rightFlipperPos.set(ofGetWidth()/2 + 80 , ofGetHeight() - 150);
    rightFlipper.setPhysics(1.0, 0.5, 0.5);
    rightFlipper.setup(box2d.getWorld(), rightFlipperPos.x, rightFlipperPos.y, 150, 20);
    
    rightAnchorPos.set(rightFlipperPos.x + 75, rightFlipperPos.y);
    rightAnchor.setup(box2d.getWorld(), rightAnchorPos, 10);
    
    rightFlipperJoint.setWorld(box2d.getWorld());
        
    rightFlipperJoint.setup(rightAnchor.body, rightFlipper.body, 0, 0);
    rightFlipperJoint.setLimits(-ofDegToRad(35), ofDegToRad(35));
}

void ofApp::drawFlippers() {
    ofSetColor(babyBlue);
   leftFlipper.draw();
   rightFlipper.draw();
   leftAnchor.draw();
   rightAnchor.draw();
   leftFlipperJoint.draw();
   rightFlipperJoint.draw();
}

//returns a polyline
ofPolyline ofApp::svgToPolyline(ofxSVG svg) {
    ofPolyline  contour;
    // grab a shape from SVG file
    ofPath p = svg.getPathAt(0);
    p.setPolyWindingMode(OF_POLY_WINDING_ODD);
    // convert shape path to polyline
    contour.addVertices(p.getOutline().at(0).getVertices());
    // close the shape by adding a last vertex which is the first vertex of the shape
    contour.addVertex(p.getOutline().at(0).getVertices().at(0));
    contour.close();
    
    return contour;
}

void ofApp::polylineToPolygon(ofPolyline polyline) {
    auto poly = std::make_shared<ofxBox2dPolygon>();
    poly->addVertices(polyline.getVertices());
    poly->triangulate();
    poly->create(box2d.getWorld());
    obstacles.push_back(poly);
}

//        // convert contour to box2d edge and add to the box2d World
//        auto poly = std::make_shared<ofxBox2dPolygon();
//         poly->addVertices(contour.getVertices());
//        poly->setPhysics(1.0, 0.3, 0.5);
//        poly->create(box2d.getWorld());
//        obstacles.push_back(poly);
//    }
//    ofxBox2dCircle jointAxis;
//    jointAxis.setup(box2d.getWorld(), shapeX, shapeY, 10);
//    jointAxis.draw();
//    ofxBox2dRevoluteJoint joint;
//    joint.setWorld(box2d.getWorld());
//    //joint.setup(jointAxis, <#b2Body *b#>)
//
//}
