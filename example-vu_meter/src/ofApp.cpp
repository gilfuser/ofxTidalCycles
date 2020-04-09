#include "ofApp.h"

//-------------------------------------------------------------
void ofApp::setup(){

    ofBackground(0);
    //args: port number, buffering bar num
    tidal = new ofxTidalCycles(3333/*, 4*/); // port, bar buffer
    vector<string> filepaths;

    wavePhase = 0;
    pulsePhase = 0;

    string path = "/home/skmecs/tidal-samples/skmecs/wahiao3_15";
    ofDirectory dir(path);
    dir.allowExt("WAV");
    dir.allowExt("wav");
    //populate the directory object
    dir.listDir();
    cout << "dir size: " << dir.size() << endl;

    for(size_t i = 0; i < dir.size(); i++){
        filepaths.push_back(
                    ofToDataPath( dir.getPath(i), true )
                    );
        ofSort(filepaths);
    }

    ofSetLogLevel(OF_LOG_VERBOSE);

    for ( auto file : filepaths) {
//        ofLog( OF_LOG_NOTICE, "sample " + file );
        if( ofFile::doesFileExist( file ) ) {
            audiofile = new ofxAudioFile;
            audiofile->load( file );
            audiofile->setVerbose(true);
            audiofiles.push_back(audiofile);
            if (!audiofile->loaded()){
                ofLogError()<<"error loading file, double check the file path";
            }
        } else {
            ofLogError()<<"input file does not exists";
        }
    }

    ofSoundStreamSettings settings;
    sampleRate = 44100.0;
    settings.setOutListener(this);
    settings.sampleRate = sampleRate;
    settings.numOutputChannels = 2;
    settings.numInputChannels = 0;
    settings.bufferSize = 512;
    ofSoundStreamSetup(settings);

    playheadControl = -1.0;
    maxL = 0;
    for( size_t f = 0; f != audiofiles.size(); f++ ) {
        if ( audiofiles[f]->length() > maxL )
            maxL = audiofiles[f]->length();
        playheads.push_back( std::numeric_limits<int>::max() );
        steps.push_back( audiofiles[f]->samplerate() / sampleRate );
        playheadControls.push_back(playheadControl);
    }
}

void ofApp::update(){
//    cout << "counter " << tidal->counter << endl;
    tidal->update();
}

//--------------------------------------------------------------

void ofApp::draw(){
    float margin = ofGetWidth() / 16.0;
    drawGrid( margin, margin, ofGetWidth() - margin * 2, ofGetHeight() - margin * 2 );
    drawNotes( margin, margin, ofGetWidth() - margin * 2 );
//    tidal->drawInstNames( margin, margin,  ofGetHeight() - margin * 2 );
    drawWaveforms(0, 0, 0);
//    tidal->drawOscMsg();
}

void ofApp::drawNotes( float left, float top, float width )
{
    if (tidal->eventBuffer.size() > 0)
    {
        float h, y;
        for ( auto event : tidal->events )
        {
            auto lastEvent = tidal->events[tidal->events.size() - 1];
            float w = width / event.cps / lastEvent.delta / tidal->maxBar;
            int bar = lastEvent.bar - event.bar;
            float x = ofMap(bar - event.fract, 0, tidal->maxBar, width, 0) - w + left;

            h = orbCellHeight / get<2>(event.orbit);
            y = ofGetHeight() - (2 * top) - ofMap(
                        event.n, get<3>(event.orbit), get<4>(event.orbit),
                        orbCellHeight * get<1>(event.orbit),
                        orbCellHeight * get<1>(event.orbit) + orbCellHeight - h) - h + top;
            if (x >= left )
            {
                ofSetColor(255);
                ofDrawRectangle(x, y , w, h);
                ofDrawBitmapStringHighlight(  ofToString(event.n), x, y);
            // ofDrawBitmapStringHighlight(  ofToString(get<0>(event.orbit)), x + 30, y);
            // ofDrawBitmapStringHighlight( eventsBuffer[i], left + 5, y);
            cout << endl << ".............................." << endl;
            cout << "bar " << bar << endl;
            cout << "event bar " << event.bar << endl;
            cout << "fract " << event.fract << endl;
            cout << "x " << x << endl;
            cout << "delta " << get<0>(tidal->msgBuffer) << endl;
            cout << "cps " << event.cps << endl;
            };
        }
        if( tidal->counter != lastCount ) {
            cout << endl << "----------------------------------" << endl;
            cout << "maxBar " << tidal->maxBar << endl;

            cout << "counter " << tidal->counter << endl << endl;
            for ( ulong i = 0; i < get<1>(tidal->msgBuffer).size(); i++ )
            {
//                playheadControls[ tidal->msgBuffer[1][i] ] = 0.0;
                cout << "buf_n " << get<1>(tidal->msgBuffer)[i] << endl;
            }
            get<1>(tidal->msgBuffer) = {};
            lastCount = tidal->counter;
            //    tidal->msgBuffer[0].erase(tidal->msgBuffer[0].begin());
        }
    }
}

void ofApp::drawWaveforms(float x, float y, float h){
    ofNoFill();
    ofPushMatrix();

    for( size_t f = 0; f != audiofiles.size(); f++ ) {
        uint maxLength = ofMap( audiofiles[f]->length(), 0, maxL, 0, ofGetWidth() );

        for( size_t chan = 0; chan < audiofiles[f]->channels(); ++chan ){
            ofBeginShape();
            for( uint x = 0; x < maxLength; ++x ){
                int sampN = ofMap( x, 0, maxLength , 0, audiofiles[f]->length(), true );
                float val = audiofiles[f]->sample( sampN, chan );
                float y = ofMap( val, -1.0f, 1.0f, ofGetHeight() / audiofiles.size(), 0.0f );
                ofVertex( x, y );
            }
            ofEndShape();

            float phx = ofMap( playheads[f], 0, audiofiles[f]->length(), 0, maxLength );
            ofDrawLine( phx, 0, phx, ofGetHeight() / audiofiles.size() );

            ofTranslate( 0.0, ofGetHeight() / audiofiles.size() );
        }
//        ofDrawBitmapString( audiofiles[f]->path(), 10, 0 );
    }
    ofPopMatrix();

    ofDrawBitmapString ( "press SPACEBAR to play, press L to load a sample", 10,
                         ofGetHeight() - 20 );
};

void ofApp::audioOut(ofSoundBuffer &buffer){

    // really spartan and not efficient sample playing, just for testing
    for( size_t f = 0; f != audiofiles.size(); f++ ) {

    if( playheadControls[f] >= 0.0 ){
        playheads[f] = playheadControls[f];
        playheadControls[f] = -1.0;
    }

    for (size_t i = 0; i < buffer.getNumFrames(); i++){

        uint n = playheads[f];
        if( n < audiofiles[f]->length() - 1 ){

            for( size_t k=0; k<buffer.getNumChannels(); ++k){
                if( k < audiofiles[f]->channels() ){
                    float fract = playheads[f] - (double) n;
                    float s0 = audiofiles[f]->sample( n, k );
                    float s1 = audiofiles[f]->sample( n + 1, k );
                    float isample = s0 * (1.0-fract) + s1 * fract; // linear interpolation
                    buffer[ i * buffer.getNumChannels() + k] = isample;
                }else{
                    buffer[ i * buffer.getNumChannels() + k] = 0.0f;
                }
            }

            playheads[f] += steps[f];

        }else{
            buffer[ i * buffer.getNumChannels() ] = 0.0f;
            buffer[ i * buffer.getNumChannels() + 1] = 0.0f;
            playheads[f] = std::numeric_limits<int>::max();
        }

    }
    }
}


void ofApp::drawGrid( float left, float top, float width, float height ) {
    float orbCellY;
    ofNoFill();
    ofSetColor(127);
    ofDrawRectangle(left, top, width, height);
    for (size_t i = 0; i < tidal->activeOrbs.size(); i++) {
        orbCellHeight = height / tidal->activeOrbs.size();
        orbCellY = orbCellHeight * i;
//        ofSetColor(i * 255, 255, 255 - i * 255);
        ofDrawRectangle(left, orbCellY + top, width, orbCellHeight);
    }
    ofFill();
}

void ofApp::drawOrbNumbers( float left, float top, float width ) {
    ofSetColor(255);
    for ( size_t i = 0; i < tidal->activeOrbs.size(); i++) {
        float y = /* height - */ ( orbCellHeight * i ) + top + 13 - orbCellHeight;
        ofDrawBitmapStringHighlight(
                    ofToString( tidal->activeOrbs[i] ),
                    width + left - 10, y
                    );
    }
}

void ofApp::drawInstNames( float left, float top ) {
    ofSetColor(255);
    float  h, y;
    for ( auto event : tidal->events ) {
        h = orbCellHeight / get<2>(event.orbit);
        y = ofMap( event.n, get<3>(event.orbit), get<4>(event.orbit), orbCellHeight * get<1>(event.orbit), orbCellHeight * get<1>(event.orbit) + orbCellHeight - h ) + top;
        ofDrawBitmapStringHighlight( event.s, left + 5, y);
        };
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    if ( key == '7' ) playheadControls[2] = 0.0;
//    if( key == 'l' || key=='L'){
//       //Open the Open File Dialog
//        ofFileDialogResult openFileResult= ofSystemLoadDialog("select an audio sample");
//        //Check if the user opened a file
//        if (openFileResult.bSuccess){
//            string filepath = openFileResult.getPath();
////            audiofiles[f].load ( filepath );
////            step = audiofiles[f].samplerate() / sampleRate;
//            ofLogVerbose("file loaded");
//        }else {
//            ofLogVerbose("User hit cancel");
//        }
//    }
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
    audiofiles[0]->load( dragInfo.files[0] );
}


//--------------------------------------------------------------
void ofApp::exit(){
    ofSoundStreamClose();
}
