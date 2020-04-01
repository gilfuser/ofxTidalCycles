#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    ofBackground(0);
    //args: port number, buffering bar num
    tidal = new ofxTidalCycles(3333, 4);
    vector<string> filepaths;


    string path = "/home/skmecs/tidal-samples/808/bd8";
    ofDirectory dir(path);
    dir.allowExt("WAV");
    //populate the directory object
    dir.listDir();
    cout << "dir size: " << dir.size() << endl;

    //go through and print out all the paths
    for(size_t i = 0; i < dir.size(); i++){
        filepaths.push_back(
                    ofToDataPath( dir.getPath(i), true )
                    );
        ofSort(filepaths);
    }

    ofSetLogLevel(OF_LOG_VERBOSE);

//    for( auto file : filepaths ) {
    for ( auto file : filepaths) {
//        ofLog( OF_LOG_NOTICE, "sample " + file );
        if( ofFile::doesFileExist( file ) ) {
            audiofile = new ofxAudioFile;
            audiofile->load( file );
//            audiofile.load( filepaths[findex] );
            audiofile->setVerbose(true);
//            audiofiles.push_back(&audiofile);
            audiofiles.push_back(audiofile);
            if (!audiofile->loaded()){
                ofLogError()<<"error loading file, double check the file path";
            }
//            cout << "audio file length: " << audiofile->length() << endl;
//            cout << "audio file 0 length: " << audiofiles[0]->length() << endl;
        } else {
            ofLogError()<<"input file does not exists";
        }
    };

    maxL = 0;

    // audio setup for testing audio file stream
    ofSoundStreamSettings settings;
    sampleRate = 44100.0;
    settings.setOutListener(this);
    settings.sampleRate = sampleRate;
    settings.numOutputChannels = 2;
    settings.numInputChannels = 0;
    settings.bufferSize = 512;
    ofSoundStreamSetup(settings);

    playheadControl = -1.0;
    for( size_t f = 0; f != audiofiles.size(); f++ ) {
        if ( audiofiles[f]->length() > maxL )
            maxL = audiofiles[f]->length();
//        cout << "max length " << maxL << endl;
//        cout << ofGetWidth() << endl;
        playheads.push_back( std::numeric_limits<int>::max() ); // because it is converted to int for check
//        steps.push_back( audiofiles[f]->samplerate() / sampleRate );
    }

}

void ofApp::update(){
    tidal->update();
}

//--------------------------------------------------------------
void ofApp::draw(){
//    float margin = ofGetWidth() / 16.0;

//    tidal->drawGrid( margin, margin, ofGetWidth() - margin * 2, ofGetHeight() - margin * 2 );
//    tidal->drawNotes( margin, margin, ofGetWidth() - margin * 2, ofGetHeight() - margin * 2 );
//    tidal->drawInstNames( margin, margin,  ofGetHeight() - margin * 2 );

TODO: "make playheads independent";

    ofNoFill();
    ofPushMatrix();
//    float max = ofGetWidth();
    for( size_t f = 0; f != audiofiles.size(); f++ ) {
        int maxLength = ofMap( audiofiles[f]->length(), 0, maxL, 0, ofGetWidth() );
        if( playheadControl >= 0.0 ){
            playheads[f] = playheadControl;
        playheadControl = -1.0;
        }

        for( size_t chan = 0; chan < audiofiles[f]->channels(); ++chan ){
            ofBeginShape();
            for( size_t x = 0; x < maxLength; ++x ){
                int sampN = ofMap( x, 0, maxLength , 0, audiofiles[f]->length(), true );
                float val = audiofiles[f]->sample( sampN, chan );
                float y = ofMap( val, -1.0f, 1.0f, ofGetHeight() / audiofiles.size(), 0.0f );
                ofVertex( x, y );
            }
            ofEndShape();

            for (size_t i = 0; i < audiofiles[f]->length(); i++){

                size_t n = playheads[f];

                if( n < audiofiles[f]->length()-1 ){

                    playheads[f] += steps[f];

                }else{
                    playheads[f] = std::numeric_limits<int>::max();
                }
            }

            float phx = ofMap( playheads[f], 0, audiofiles[f]->length(), 0, maxLength );
            ofDrawLine( phx, 0, phx, ofGetHeight() / audiofiles.size() );

            ofTranslate( 0.0, ofGetHeight() / audiofiles.size() );
        }
//        ofDrawBitmapString( audiofiles[f]->path(), 10, 0 );
    }
    ofPopMatrix();

    ofDrawBitmapString ( "press SPACEBAR to play, press L to load a sample", 10,
                         ofGetHeight() - 20 );

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
    if( key == ' ') playheadControl = 0.0;

    if( key == 'l' || key=='L'){
       //Open the Open File Dialog
        ofFileDialogResult openFileResult= ofSystemLoadDialog("select an audio sample");
        //Check if the user opened a file
        if (openFileResult.bSuccess){
            string filepath = openFileResult.getPath();
//            audiofiles[0].load ( filepath );
//            step = audiofiles[0].samplerate() / sampleRate;
            ofLogVerbose("file loaded");
        }else {
            ofLogVerbose("User hit cancel");
        }
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
//    audiofiles[0].load( dragInfo.files[0] );
}


//--------------------------------------------------------------
void ofApp::exit(){
    ofSoundStreamClose();
}
