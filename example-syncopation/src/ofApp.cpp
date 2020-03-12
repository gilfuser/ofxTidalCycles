#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    ofBackground(0);

    //args: port number, buffering bar num
    tidal = new ofxTidalCycles(3333, 3);
}

//--------------------------------------------------------------
void ofApp::update(){
    tidal->update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    float marginTop = ofGetWidth() / 32.0;
    float marginLeft = ofGetWidth() / 32.0;
    float w = ofGetWidth() - marginLeft * 2;
    float h = ofGetHeight() / 1.5 - marginTop * 2.0;
//    tidal->drawBg(marginLeft, marginTop, w, h);
    tidal->drawGrid(marginLeft, marginTop, w, h);
    tidal->drawNotes(marginLeft, marginTop, w, h);
    tidal->drawInstNames(marginLeft, marginTop, w);
    tidal->drawGraph(marginTop + h + 40);
}

void ofxTidalCycles::drawGrid(float left, float top, float width, float height) {
    float orbCellY;
    ofNoFill();
    ofSetColor(127);
    ofDrawRectangle(left, top, width, height);
    for (size_t i = 0; i < activeOrbs.size(); i++) {
        orbCellHeight = height / activeOrbs.size();
        orbCellY = orbCellHeight * i;
//        ofSetColor(i * 255, 255, 255 - i * 255);
        ofDrawRectangle(left, orbCellY + top, width, orbCellHeight);
    }
    ofFill();
}

void ofxTidalCycles::drawOrbNumbers(float left, float top, float width, float height) {
    ofSetColor(255);
    for ( size_t i = 0; i < activeOrbs.size(); i++) {
        float y = /* height - */ ( orbCellHeight * i ) + top + 13 - orbCellHeight;
        ofDrawBitmapStringHighlight(ofToString(activeOrbs[i]), width + left - 10, y);
    }
}


void ofxTidalCycles::drawNotes(float left, float top, float width, float height) {
    if (soundBuffer.size() > 0) {
        float h, y, w = width / 128.0;
            for ( auto event : events ) {
                int bar = events[events.size() - 1].bar - event.bar;
                float x = ofMap(bar - event.fract, -1, maxBar, width+left, left);

                h = orbCellHeight / get<2>(event.orbit);
                y = /*height -*/ ofMap(event.n, get<3>(event.orbit), get<4>(event.orbit),
                                           orbCellHeight * get<1>(event.orbit),  orbCellHeight *
                                           get<1>(event.orbit) + orbCellHeight - h ) + top;

                if (x > left) {
                    ofSetColor(255);
                    ofDrawRectangle(x, y , w, h);
                    ofDrawBitmapStringHighlight(  ofToString(event.n), x, y);
                };
//            ofDrawBitmapStringHighlight(  ofToString(get<0>(event.orbit)), x + 30, y);
//            ofDrawBitmapStringHighlight( eventsBuffer[i], left + 5, y);
            }
//                for( auto i = soundBuffer.begin(); i != soundBuffer.end(); ++i) {
//                    cout << "orb0Buffer: " << (*i) << '\n';
//                }
//        }
    }
}

void ofxTidalCycles::drawInstNames(float left, float top, float height) {
    ofSetColor(255);
    float  h, y;
    for ( auto event : events ) {
        h = orbCellHeight / get<2>(event.orbit);
        y = /*height -*/ ofMap(event.n, get<3>(event.orbit), get<4>(event.orbit),
                               orbCellHeight * get<1>(event.orbit),  orbCellHeight *
                               get<1>(event.orbit) + orbCellHeight - h ) + top;
            ofDrawBitmapStringHighlight( event.s, left + 5, y);
        };
//    }
}

void ofxTidalCycles::drawGraph(float top) {
    //draw graph
    float x, y, gwidth, gheight, graphX;
    float graphWidth;
    int instNumMax =soundBuffer.size();

    x = 20;
    y = top;
    graphX = 70;
    gwidth = ofGetWidth() - 40 - graphX;
    gheight = 10;

    ofTranslate(x, y);
    ofPushMatrix();
    ofSetColor(255);
    ofDrawBitmapString("Syncopation Degree", 0, 0);
    ofTranslate(0, -8);
    for (int i = 0; i < instNumMax; i++) {
        ofTranslate(0, 14);
        graphWidth = ofMap(syncopation[i], 0, 20, 0, gwidth);
        ofSetColor(63);
        ofDrawRectangle(graphX, 0, gwidth, gheight);
        ofSetColor(63, 127, 255);
        ofDrawRectangle(graphX, 0, graphWidth, gheight);
        ofSetColor(255);
        ofDrawBitmapString(get<0>(soundBuffer[i]), 0, 10);
        ofDrawBitmapString(ofToString(syncopation[i]), graphX + 5, 10);
    }
    ofTranslate(0, 40);
    ofDrawBitmapString("Note Number", 0, 0);
    ofTranslate(0, -8);
    for (int i = 0; i < instNumMax; i++) {
        ofTranslate(0, 14);
        graphWidth = ofMap(eventsNum[i], 0, 32, 0, gwidth);
        ofSetColor(63);
        ofDrawRectangle(graphX, 0, gwidth, gheight);
        ofSetColor(63, 127, 255);
        ofDrawRectangle(graphX, 0, graphWidth, gheight);
        ofSetColor(255);
        //ofDrawBitmapString("E"
        //                   + ofToString(i) + ":"
        //                   + ofToString(app->tidal->entropy[i], 2), 0, 10);
        ofDrawBitmapString(get<0>(soundBuffer[i]), 0, 10);
        ofDrawBitmapString(ofToString(eventsNum[i]), graphX + 5, 10);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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
