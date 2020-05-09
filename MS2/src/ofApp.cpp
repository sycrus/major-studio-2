#include "ofApp.h"
#include "ofxCv.h"

using namespace ofxCv;
using namespace cv;

ofColor navy, salmon, rubberDuck, babyBlue; //palette 1

int minStartArea = 31;      int maxStartArea = 36;
int minEndArea = 43;        int maxEndArea = 50;
int minWheelArea = 17;      int maxWheelArea = 21;
int minSeesawArea = 22;     int maxSeesawArea = 27;

bool isGui = false;

void ofApp::setup() {
    ofSetBackgroundColor(0,0,0); //sets background to black
    ofSetFrameRate(60);
    // set colors
    navy.setHex(0x21243d);
    salmon.setHex(0xff7c7c);
    rubberDuck.setHex(0xffd082);
    babyBlue.setHex(0x88e1f2);
    ////https://colorhunt.co/palette/170829
    
    initializeSvgs();
    initializeKinect();
    initializeCV();
  
    // Box2d
    box2d.init();
    box2d.enableEvents();
    box2d.setGravity(0, 30);
    box2d.setFPS(40.0);
    
    //Arduino
//    ard.connect("/dev/tty.usbmodem146101", 57600);
//    ofAddListener(ard.EInitialized, this, &ofApp::setupArduino);
//    bSetupArduino    = false;
    
    //Box2D: register the listener so that we get the events
    ofAddListener(box2d.contactStartEvents, this, &ofApp::contactStart);
    ofAddListener(box2d.contactEndEvents, this, &ofApp::contactEnd);
    
//    ofPoint middle;
//    middle.set(ofGetWidth()/2, ofGetHeight()/2);
    
    //Sounds: load the 8 sfx soundfile
    for (int i=0; i<N_SOUNDS; i++) {
        sound[i].load("sfx/"+ofToString(i)+".mp3");
        sound[i].setMultiPlay(true);
        sound[i].setLoop(false);
    }
    // Reference for sounds
//    auto c = std::make_shared<ofxBox2dCircle>();
//    c->setPhysics(0.0, 0.8, 0.9);
//    c->setup(box2d.getWorld(), ofGetWidth()/2, ofGetHeight()/2, 40);
//    c->setData(new SoundData());
//    auto * sd = (SoundData*)c->getData();
//    sd->soundID = ofRandom(0, N_SOUNDS);
//    sd->bHit    = false;
//    bumpers.push_back(c);

}

void ofApp::update() {
    
    //remove off-screen balls
    ofRemove(balls, [](shared_ptr<ofxBox2dCircle> balls) -> bool {
        return !ofRectangle(0, -400, ofGetWidth(), ofGetHeight()+400).inside(balls->getPosition());
    });
    
    //if (!bSetupArduino) ofLog() << "Arduino not ready..." << endl;;
    //updateArduino();
    updateKinect();
    
    box2d.update();
    
}
void ofApp::draw() {
    ofSetColor(255);
    ofFill();
    
    if (showContours) { contourFinder.draw(); }
    if (showShapes) { drawContourAreas();};
    
    //for detected objects
    for(auto & bEdge : backgroundEdges) {
        ofFill();
        ofSetColor(salmon);
        bEdge->draw();
    }
    for(auto & edge : edges) {
        ofFill();
        ofSetColor(salmon);
        edge->draw();
    }
    
    for(auto & obstaclesWheel : obstaclesWheels) {
        ofPushStyle();
            ofFill();
            ofSetColor(salmon);
            obstaclesWheel->draw();
        ofPopStyle();
    }
    for(auto & obstaclesSeesaw : obstaclesSeesaws) {
        ofPushStyle();
            ofFill();
            ofSetColor(salmon);
            obstaclesSeesaw->draw();
        ofPopStyle();
    }
    for(auto & obstaclesHammer : obstaclesHammers) {
        ofPushStyle();
            ofNoFill();
            ofSetColor(salmon);
            obstaclesHammer->draw();
        ofPopStyle();
    }
    for(auto & axle : axles) { ofNoFill(); axle->draw();}
    for(auto & axis : axes) { ofNoFill(); axis->draw();}
    for(auto & ssAxle : ssAxles) { ofNoFill(); ssAxle->draw(); }
    for(auto & ssAxis : ssAxes) { ofNoFill(); ssAxis->draw(); }
    for(auto & ssPin : ssPins) { ofNoFill(); ssPin->draw(); }

    drawEnd();
    //for ball
    for (auto &ball : balls) {
        ofPushStyle();
            ofFill();
            ofSetColor(babyBlue);
            ball->draw();
        ofPopStyle();
    }

    if (isGui) {
        gui.draw();
        stringstream reportStream;
        
        ofSetColor(ofColor::white);
        reportStream << "FPS: " << ofGetFrameRate() << endl;
        reportStream << "Threshold: " << threshold << endl;
        reportStream << "Angle: " << angle << endl;
        reportStream << "Min Area: " << minAreaRadius << endl;
        reportStream << "Max Area: " << maxAreaRadius << endl;
        reportStream << "Wheel: " << obstaclesWheels.size()  << endl;
        reportStream << "Seesaw: " << obstaclesSeesaws.size()  << endl;
        ofDrawBitmapString(reportStream.str(), 20, 500);
    }
   
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
    if(key == 'f') { ofToggleFullscreen();}
    if(key == 'h') { isGui = !isGui; showContours =  !showContours;}
}

void ofApp::keyReleased(int key) {
    // only one key, but check
    if(key == 'd') { showContours = !showContours; }
    if(key == 'b') { createEdgesContourFinder();}
    
    if (key == OF_KEY_UP) { angle++; if(angle>30) angle=30; kinect.setCameraTiltAngle(angle);}
    if (key == OF_KEY_DOWN) {angle--; if(angle<-30) angle=-30;kinect.setCameraTiltAngle(angle);}
    // Kinect : Change Threshold
    if (key == '=') {if (threshold > 255) {threshold = 255;}else {threshold++;}}
    if (key == '-') {if (threshold < 0)  {threshold = 0;} else {threshold--;}}
    // Kinect : Change min Blob Size
    if(key == '.') { minAreaRadius ++; contourFinder.setMinAreaRadius(minAreaRadius);}
    if(key == ',') { minAreaRadius --;contourFinder.setMinAreaRadius(minAreaRadius);}
    // Kinect : Change max Blob Size
    if(key == ';') { maxAreaRadius ++;contourFinder.setMaxAreaRadius(maxAreaRadius);}
    if(key == 'l') { maxAreaRadius --;contourFinder.setMaxAreaRadius(maxAreaRadius);}
    
    //set obstacles
    if(key == 'n') {
        getMarkerLocations();
        for(auto & wheel : wheels)      { createWheel(wheel);}
        for(auto & seesaw : seesaws)    { createSeesaw(seesaw);}
    }
    //send to one button
    if(key == 'c') {
        outlines.clear();           wheels.clear();     seesaws.clear();
        edges.clear();              balls.clear();    obstaclesWheels.clear();
        obstaclesSeesaws.clear();   axes.clear();       axles.clear();
        ssAxes.clear();             ssAxles.clear();    ssPins.clear();
        for(auto & axle : axles) { ofNoFill(); axle->destroy();}
        for(auto & axis : axes) { ofNoFill(); axis->destroy();}
        for(auto & ssAxle : ssAxles) { ofNoFill(); ssAxle->destroy(); }
        for(auto & ssAxis : ssAxes) { ofNoFill(); ssAxis->destroy(); }
        for(auto & ssPin : ssPins) { ofNoFill(); ssPin->destroy(); }
        endJoint.destroy();
        endPolygon.destroy();
        endAnchor.destroy();
    }
}

void ofApp::initializeSvgs() {
    wheelSvg.load("svg/wheel.svg");
    seesawSvg.load("svg/seesaw.svg");
    hammerSvg.load("svg/hammer.svg");
    endSvg.load("svg/end.svg");
    startSvg.load("svg/start.svg");
}

void ofApp::initializeKinect() {
    kinect.setRegistration(true);
    kinect.init();
    kinect.init(true); // shows infrared instead of RGB video image
    kinect.open();        // opens first available kinect
    angle = -8;
    kinect.setCameraTiltAngle(angle);
}

void ofApp::initializeCV() {
    threshold = 200;
    minAreaRadius = 23;
    maxAreaRadius = 80;
    bThreshWithOpenCV = true;

    contourFinder.setMinAreaRadius(minAreaRadius);
    contourFinder.setMaxAreaRadius(maxAreaRadius);
    
    grayImage.allocate(kinect.width, kinect.height);
    grayThresh.allocate(kinect.width, kinect.height);
    grayImage.resize(ofGetWidth(), ofGetHeight());
    grayImage.scale(2.8, 2.6);
    grayThresh = grayImage;
    grayThresh.resize(ofGetWidth(), ofGetHeight());
    grayThresh.scale(2.8, 2.6);
}

void ofApp::setupArduino(const int & version) {
    
    ofRemoveListener(ard.EInitialized, this, &ofApp::setupArduino);
    bSetupArduino = true;
    
    //ofLogNotice() << ard.getFirmwareName();
    //ofLogNotice() << "firmata v" << ard.getMajorFirmwareVersion() << "." << ard.getMinorFirmwareVersion();
    
    ard.sendDigitalPinMode(2, ARD_INPUT_PULLUP); //player 1
    ard.sendDigitalPinMode(3, ARD_INPUT_PULLUP); //player 2
   
    ofAddListener(ard.EDigitalPinChanged, this, &ofApp::digitalPinChanged);
}
void ofApp::updateArduino(){
    
    ard.update();
    
    if (bSetupArduino) { //only send to Arduino when ready
        cout << ard.getDigital(2) << "," <<ard.getDigital(3) << endl;
    }

}
void ofApp::digitalPinChanged(const int & pinNum) {

//    if (!ard.getDigital(2)) {
//        isLeftFlipperActive = true;
//    } else {
//        isLeftFlipperActive = false;
//    }
    
}

void ofApp::updateKinect(){
    kinect.update();
    if(kinect.isFrameNew()) {
        grayImage.setFromPixels(kinect.getPixels());
        //grayImage.setAnchorPoint(ofGetWidth()/2, ofGetHeight()/2);
        grayImage.resize(ofGetWidth(), ofGetHeight());
        grayImage.scale(1.3, 1.1);
        grayThresh = grayImage;
        
        grayThresh.resize(ofGetWidth(), ofGetHeight());
        grayThresh.scale(1.3, 1.1);
        grayThresh.blurGaussian(1.0f);
        grayThresh.threshold(threshold);
        
        grayImage.flagImageChanged();
        contourFinder.findContours(grayThresh);
    }
}

void ofApp::createStart(ofPoint position) {
    auto ball = std::make_shared<ofxBox2dCircle>();
    ball->setPhysics(1.0, 0.2, 0.1); //density bounce friction
    ball->setup(box2d.getWorld(), position.x, position.y, 12); //with the start module
    balls.push_back(ball);
}

void ofApp::drawEnd()  {
    ofPushStyle();
        ofFill();
        ofSetColor(babyBlue);
        endPolygon.draw(); endAnchor.draw(); endJoint.draw();
    ofPopStyle();
}

void ofApp::drawContourAreas() {
    //contourFinder.draw();
    int blobArea[contourFinder.size()];
    for (int i=0; i < contourFinder.size(); i++) {
        ofPolyline contourPolyline = contourFinder.getPolyline(i);
        contourPolyline.simplify(2.0f);
        blobArea[i] = abs(contourPolyline.getArea());
        int roundedBlobArea = blobArea[i]/100;
        if (showContours) {
            ofDrawBitmapString(100 * roundedBlobArea, contourPolyline.getCentroid2D().x, contourPolyline.getCentroid2D().y);
        }
         if (roundedBlobArea > minStartArea && roundedBlobArea < maxStartArea) {
            //ofDrawBitmapString("Start", contourPolyline.getCentroid2D().x, contourPolyline.getCentroid2D().y);
             ofPushMatrix();
                ofTranslate(contourPolyline.getCentroid2D().x - startSvg.getWidth()/2 , contourPolyline.getCentroid2D().y - startSvg.getHeight()/2);
                startSvg.draw();
             ofPopMatrix();
         }
        if (roundedBlobArea > minEndArea && roundedBlobArea < maxEndArea) {
           //ofDrawBitmapString("End", contourPolyline.getCentroid2D().x, contourPolyline.getCentroid2D().y);
            ofPushMatrix();
               ofTranslate(contourPolyline.getCentroid2D().x - endSvg.getWidth()/2 , contourPolyline.getCentroid2D().y - 10);
               endSvg.draw();
            ofPopMatrix();
        }
        if (roundedBlobArea > minWheelArea && roundedBlobArea < maxWheelArea) {
           //ofDrawBitmapString("Wheel", contourPolyline.getCentroid2D().x, contourPolyline.getCentroid2D().y);
            ofPushMatrix();
               ofTranslate(contourPolyline.getCentroid2D().x - wheelSvg.getWidth()/2 , contourPolyline.getCentroid2D().y - wheelSvg.getHeight()/2);
               wheelSvg.draw();
            ofPopMatrix();
        }
        if (roundedBlobArea > minSeesawArea && roundedBlobArea < maxSeesawArea) {
           //ofDrawBitmapString("Seesaw", contourPolyline.getCentroid2D().x, contourPolyline.getCentroid2D().y);
            ofPushMatrix();
               ofTranslate(contourPolyline.getCentroid2D().x - seesawSvg.getWidth()/2 , contourPolyline.getCentroid2D().y - seesawSvg.getHeight()/2);
               seesawSvg.draw();
            ofPopMatrix();
        }
    }
}

//marker recognition and draw here =======================
void ofApp::getMarkerLocations() {
    int blobArea[contourFinder.size()];\
    for (int i=0; i < contourFinder.size(); i++) {
        ofPolyline contourPolyline = contourFinder.getPolyline(i);
        contourPolyline.simplify(2.0f);
        blobArea[i] = abs(contourPolyline.getArea());
        int roundedBlobArea = blobArea[i]/100;
        if (roundedBlobArea > minStartArea && roundedBlobArea < maxStartArea) {//circle
            ofPoint centroid;
            centroid.set(contourPolyline.getCentroid2D().x, contourPolyline.getCentroid2D().y);
            createStart(centroid);
        }
        if (roundedBlobArea > minEndArea && roundedBlobArea < maxEndArea) { //square
            ofPoint centroid;
            centroid.set(contourPolyline.getCentroid2D().x, contourPolyline.getCentroid2D().y);
            createEnd(centroid);
        }
        if (roundedBlobArea > minWheelArea && roundedBlobArea < maxWheelArea) { //square
            ofPoint centroid;
            centroid.set(contourPolyline.getCentroid2D().x, contourPolyline.getCentroid2D().y);
            wheels.push_back(centroid);
        }
        if (roundedBlobArea > minSeesawArea && roundedBlobArea < maxSeesawArea) { //square
            ofPoint centroid;
            centroid.set(contourPolyline.getCentroid2D().x, contourPolyline.getCentroid2D().y);
            seesaws.push_back(centroid);
        }
        
    }
}

void ofApp::createEnd(ofPoint position) { //update
    endPolygon.addVertices(svgToPolyline(endSvg).getVertices());
    
    endPolygon.setPhysics(0.2, 0.5, 0.5);
    endPolygon.triangulate();
    endPolygon.create(box2d.getWorld(), position.x - 78, position.y - 10 );
    endAnchorPos.set(position.x, position.y);
    endAnchor.setup(box2d.getWorld(), endAnchorPos, 8);
    
    endJoint.setWorld(box2d.getWorld());
    endJoint.setup(endAnchor.body, endPolygon.body, 0, 0);
    endJoint.setLimits(ofDegToRad(-360), ofDegToRad(360));
    
    //create pins that restrict
    auto ssPin1 = make_shared<ofxBox2dCircle>();
    ssPin1->setup(box2d.getWorld(), position.x + 40, position.y + 10, 2);
    ssPins.push_back(ssPin1);

    auto ssPin2 = make_shared<ofxBox2dCircle>();
    ssPin2->setup(box2d.getWorld(), position.x - 10, position.y - 30, 2);
    ssPins.push_back(ssPin2);
}

void ofApp::createWheel(ofPoint position) { //update
    polylineToWheels(svgToPolyline(wheelSvg), position);
    
    auto axle = make_shared<ofxBox2dCircle>();
    axle->setup(box2d.getWorld(), position.x, position.y, 10);
    axles.push_back(axle);
    
    auto axis = make_shared<ofxBox2dRevoluteJoint>();
    axis->setup(axles.back()->body, obstaclesWheels.back()->body, 0, 0);
    axes.push_back(axis);
}

void ofApp::createSeesaw(ofPoint position) { //update
    polylineToSeesaws(svgToPolyline(seesawSvg), position);
    
    auto ssAxle = make_shared<ofxBox2dCircle>();
    ssAxle->setPhysics(0.0, 0.1, 0.0);
    ssAxle->setup(box2d.getWorld(), position.x , position.y, 10);
    ssAxles.push_back(ssAxle);
    
    auto ssAxis = make_shared<ofxBox2dRevoluteJoint>();
    ssAxis->setup(ssAxles.back()->body, obstaclesSeesaws.back()->body,position.x, position.y);
    ssAxes.push_back(ssAxis);
    
    //create pins that restrict
    auto ssPin1 = make_shared<ofxBox2dCircle>();
    ssPin1->setup(box2d.getWorld(), position.x - 40, position.y + 40, 2);
    ssPins.push_back(ssPin1);

    auto ssPin2 = make_shared<ofxBox2dCircle>();
    ssPin2->setup(box2d.getWorld(), position.x + 40, position.y + 40, 2);
    ssPins.push_back(ssPin2);
}

void ofApp::createEdgesContourFinder() {
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

void ofApp::polylineToWheels(ofPolyline polyline, ofPoint position) {
    auto center = polyline.getCentroid2D();
    auto poly = std::make_shared<ofxBox2dPolygon>();
    poly->addVertices(polyline.getVertices());
    poly->triangulate();
    poly->setPhysics(1., 0.2, 0.4);
    poly->create(box2d.getWorld(), position.x - center.x, position.y - center.y);
    obstaclesWheels.push_back(poly);
}

void ofApp::polylineToSeesaws(ofPolyline polyline, ofPoint position) {
    auto center = polyline.getCentroid2D();
    auto poly = std::make_shared<ofxBox2dPolygon>();
    poly->addVertices(polyline.getVertices());
    poly->triangulate();
    poly->setPhysics(1., 0.2, 0.4);
    poly->create(box2d.getWorld(), position.x - center.x, position.y - center.y);
    obstaclesSeesaws.push_back(poly);
}

void ofApp::polylineToHammers(ofPolyline polyline, ofPoint position) {
    auto center = polyline.getCentroid2D();
    auto poly = std::make_shared<ofxBox2dPolygon>();
    poly->addVertices(polyline.getVertices());
    poly->triangulate();
    poly->setPhysics(1., 0.2, 0.4);
    poly->create(box2d.getWorld(), position.x - center.x, position.y - center.y);
    obstaclesHammers.push_back(poly);
}

//returns a polyline, only works for one path (outline)
ofPolyline ofApp::svgToPolyline(ofxSVG svg) {
    ofPolyline  contour;
    ofPath p = svg.getPathAt(0);
    p.setPolyWindingMode(OF_POLY_WINDING_ODD);
    contour.addVertices(p.getOutline().at(0).getVertices());
    contour.addVertex(p.getOutline().at(0).getVertices().at(0));
    contour.close();
    return contour;
}

void ofApp::contactStart(ofxBox2dContactArgs &e) {
    if(e.a != NULL && e.b != NULL) {
        
        if(e.a->GetType() == b2Shape::e_circle && e.b->GetType() == b2Shape::e_circle) {
            drawnImg = face2;
            SoundData * aData = (SoundData*)e.a->GetBody()->GetUserData();
            SoundData * bData = (SoundData*)e.b->GetBody()->GetUserData();
            
            if(aData) {
                aData->bHit = true;
                sound[aData->soundID].play();
            }
            
            if(bData) {
                bData->bHit = true;
                sound[bData->soundID].play();
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::contactEnd(ofxBox2dContactArgs &e) {
    if(e.a != NULL && e.b != NULL) {
        drawnImg = face1;
        SoundData * aData = (SoundData*)e.a->GetBody()->GetUserData();
        SoundData * bData = (SoundData*)e.b->GetBody()->GetUserData();
        
        if(aData) {
            aData->bHit = false;
        }
        
        if(bData) {
            bData->bHit = false;
        }
    }
}
