#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    ofBackground(0);

    //args: port number, buffering bar num
    tidal = new ofxTidalCycles(3333, 4);
}

//--------------------------------------------------------------
void ofApp::update(){
    tidal->update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    float margin = ofGetWidth() / 16.0;
    tidal->drawGrid(margin, margin, ofGetWidth() - margin * 2, ofGetHeight() - margin * 2);
    tidal->drawNotes(margin, margin, ofGetWidth() - margin * 2, ofGetHeight() - margin * 2);
    tidal->drawInstNames(margin, margin,  ofGetHeight() - margin * 2);

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
