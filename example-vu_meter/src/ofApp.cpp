#include "ofApp.h"

//-------------------------------------------------------------
void ofApp::setup(){

    ofBackground(0);
    //args: port number, buffering bar num
    tidal = new ofxTidalCycles(3333/*, 4*/); // port, bar buffer
    wavePhase = 0;
    pulsePhase = 0;
    string path = "/home/skmecs/tidal-samples/bpm110/";
    ofDirectory tidalSamps(path);
    for( auto sampNames : tidalSamps ) {
        if( sampNames.isDirectory())
        {
            string path = sampNames.path();
            ofDirectory samps(path);
            string spath = samps.path();
            ofDirectory sdir(spath);
            sdir.allowExt("WAV");
            sdir.allowExt("wav");
            spath = spath.substr( 0, spath.size() - 1 );
            auto spathI = spath.find_last_of('/');
            spath = spath.substr( ++spathI );
            sounds.push_back(spath);
            sdir.listDir();
//            cout << spath <<" size: " << sdir.size() << endl;
            for ( auto samps : sdir ) {
                if ( ofFile::doesFileExist(samps.path()) )
                {
                    audiofile = new ofxAudioFile;
                    audiofile->setVerbose(true);
                    audiofile->load( samps.path() );
                    audiofiles[spath].push_back(audiofile);
                    if (!audiofile->loaded())
                        ofLogError()<<"error loading file, double check the file path";
                }
                else
                    ofLogError()<<"input file does not exists";
            }
        }
        else if ( sampNames.isFile() )
        {
            tidalSamps.allowExt("WAV");
            tidalSamps.allowExt("wav");
            string spath = tidalSamps.path();
            spath = spath.substr( 0, spath.size() - 1 );
            auto spathI = spath.find_last_of('/');
            spath = spath.substr( ++spathI );
            sounds.push_back(spath);
            tidalSamps.listDir();
//            cout << spath << " size: " << tidalSamps.size() << endl;
            for( auto samps : tidalSamps ) {
                if( ofFile::doesFileExist( samps.path() ) )
                {
                    audiofile = new ofxAudioFile;
                    audiofile->load( samps.path() );
                    audiofile->setVerbose(true);
//                    audiofiles.insert( make_pair(spath, audiofile) );
                    audiofiles[spath].push_back(audiofile);
                    if (!audiofile->loaded())
                        ofLogError()<<"error loading file, double check the file path";
                }
                else
                    ofLogError()<<"input file does not exists";
            }
        }
    }

    ofSetLogLevel(OF_LOG_VERBOSE);

    ofSoundStreamSettings settings;
    sampleRate = 44100.0;
    settings.setOutListener(this);
    settings.sampleRate = sampleRate;
    settings.numOutputChannels = 2;
    settings.numInputChannels = 0;
    settings.bufferSize = 512;
    ofSoundStreamSetup(settings);

    playheadControl = -1.0;
    for (const auto & [name, sdir] : audiofiles) {
        playheads.push_back( std::numeric_limits<int>::max() );
//        cout << aufile->first << " length " << aufile->second->length() << endl;
        for (size_t sind = 0; sind < sdir.size(); sind++) {
//            cout << name << " " << sind << endl;
            playheadControls.push_back(playheadControl);
        steps.push_back(  sdir[sind]->samplerate() / sampleRate );
        }
//        float length = file->length() / sampleRate;
//        cout << "sample " << ofToString(sname) /*<< ofToString(f)*/ << " length = " << length << " sec." << endl;
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
    drawInstNames( margin, margin,  ofGetHeight() - margin * 2 );
    tidal->drawOscMsg( false );
}

void ofApp::drawNotes( float left, float top, float width ) {
    if (tidal->eventBuffer.size() > 0)
    {
        float h, y, w;
        for ( auto event : tidal->events )
        {
            auto lastEvent = tidal->events[tidal->events.size() - 1];

            if ( audiofiles.find(event.s) == audiofiles.end() )
                w = width * event.cps * event.delta / tidal->maxBar -1;
            else
                w = width * event.cps * audiofiles[event.s][event.n % audiofiles[event.s].size()] ->length() / sampleRate / tidal->maxBar -1;
            if (event.haveLegato == true)
//                cout << "sdfsdadasdasdasdadsf" << endl;
                w = ofClamp(w, 0, width * event.cps * event.delta / tidal->maxBar * event.legato );

            int bar = lastEvent.bar - event.bar;
            float x = ofMap(bar - event.fract, 0, tidal->maxBar, width, 0) + left;

            h = orbCellHeight / get<2>(event.orbit);

            y = ofGetHeight() - (2 * top) - ofMap(
                        event.n, get<3>(event.orbit),
                        get<4>(event.orbit),
                        orbCellHeight * get<1>(event.orbit),
                        orbCellHeight * get<1>(event.orbit) + orbCellHeight - h ) - h + top;

            if ( x >= left and x < ofGetWidth() - left )
            {
                ofFill();
                ofSetColor(255);
                ofDrawRectangle(x, y , w, h);
                ofDrawBitmapStringHighlight(  ofToString(event.n), x, y);
//                ofDrawBitmapStringHighlight(  ofToString(get<0>(event.orbit)), x + 30, y);
//                drawWaveforms(x, y, w, h);
//             ofDrawBitmapStringHighlight( eventsBuffer[i], left + 5, y);
            /*
            cout << endl << ".............................." << endl;
            cout << "bar " << bar << endl;
            cout << "event bar " << event.bar << endl;
            cout << "fract " << event.fract << endl;
            cout << "x " << x << endl;
            cout << "delta " << get<0>(tidal->msgBuffer) << endl;
            cout << "cps " << event.cps << endl;
            cout << "width " << w << endl;
            */
            /*
            cout << "last event index " << lastEvent.index << endl;
            */
            };
        }
//        if( tidal->counter != lastCount )
//        {
////            cout << endl << "----------------------------------" << endl;
////            cout << "maxBar " << tidal->maxBar << endl;

////            cout << "counter " << tidal->counter << endl << endl;
//            for ( ulong i = 0; i < get<1>(tidal->msgBuffer).size(); i++ )
//            {
//                playheadControls[ get<1>(tidal->msgBuffer)[i] ] = 0.0;
////                cout << "buf_n " << get<1>(tidal->msgBuffer)[i] << endl;
//            }
//            get<1>(tidal->msgBuffer) = {};
//            lastCount = tidal->counter;
//            //    tidal->msgBuffer[0].erase(tidal->msgBuffer[0].begin());
//        }
    }
}

void ofApp::drawGrid( float left, float top, float width, float height ) {
    float orbCellY;
    ofNoFill();
    ofSetColor(127);
    ofDrawRectangle(left, top, width, height);
    for (size_t i = 0; i < tidal->activeOrbs.size(); i++)
    {
        orbCellHeight = height / tidal->activeOrbs.size();
        orbCellY = orbCellHeight * i;
        ofSetColor(i * 255, 255, 255 - i * 255);
        ofDrawRectangle(left, orbCellY + top, width, orbCellHeight);
    }
    ofFill();
}

void ofApp::drawOrbNumbers( float left, float top, float width ) {
    ofSetColor(255);
    for ( size_t i = 0; i < tidal->activeOrbs.size(); i++) {
        float y = ofGetHeight() - (2 * top) - ( orbCellHeight * i ) + top + 13 - orbCellHeight;
        ofDrawBitmapStringHighlight(
                    ofToString( tidal->activeOrbs[i] ),
                    width + left - 10, y
                    );
    }
}

void ofApp::drawInstNames( float left, float top, float h ) {
    ofSetColor(255);
    float  y;
    for ( auto event : tidal->events ) {
        h = orbCellHeight / get<2>(event.orbit);
        y = ofGetHeight() - (2 * top) - ofMap( event.n, get<3>(event.orbit), get<4>(event.orbit), orbCellHeight * get<1>(event.orbit), orbCellHeight * get<1>(event.orbit) + orbCellHeight - h ) + top;
        ofDrawBitmapStringHighlight( event.s, left + 5, y);
        };
}

//void ofApp::drawWaveforms(float xis, float ypslon, float w, float h){
//    ofNoFill();
//    ofPushMatrix();
//    for ( auto aufile = audiofiles.begin(); aufile != audiofiles.end(); ++ aufile )
//    {
//        uint maxLength = ofMap( aufile->second->length(), 0, maxL, 0, w );

//        for( size_t chan = 0; chan < aufile->second->channels(); ++chan )
//        {
//            ofBeginShape();
//            for( uint x = 0; x < maxLength; ++x )
//            {
//                int sampN = ofMap( x, 0, maxLength , 0, aufile->second->length(), true );
//                float val = aufile->second->sample( sampN, chan );
//                float y = ofMap( val, -1.0f, 1.0f, ofGetHeight() / audiofiles.size(), 0.0f );
//                ofVertex( x, y );
//            }
//            ofEndShape();

////            float phx = ofMap( playheads[f], 0, audiofiles[f]->length(), 0, maxLength );
//            ofSetColor(255, 0, 0);
////            ofDrawLine( phx, 0, phx, ofGetHeight() / audiofiles.size() );

//            ofTranslate( xis, ypslon /* / audiofiles[f]->channels() */ );
//        }
////        ofDrawBitmapString( audiofiles[f]->path(), 10, 0 );
//    }
//    ofPopMatrix();
//    ofDrawBitmapString ( "press SPACEBAR to play, press L to load a sample", 10,
//                         ofGetHeight() - 20 );
//};

//void ofApp::audioOut(ofSoundBuffer &buffer)
//{
//    for( auto aufile = audiofiles.begin(); aufile != audiofiles.end(); ++ aufile )
//    {
//        if( playheadControls[f] >= 0.0 )
//        {
//            playheads[f] = playheadControls[f];
//            playheadControls[f] = -1.0;
//        }

//        for (size_t i = 0; i < buffer.getNumFrames(); i++)
//        {
//            uint n = playheads[f];
//            if( n < audiofiles[f]->length() - 1 )
//            {
//                for( size_t k=0; k<buffer.getNumChannels(); ++k)
//                {
//                    if( k < audiofiles[f]->channels() )
//                    {
//                        float fract = playheads[f] - (double) n;
//                        float s0 = audiofiles[f]->sample( n, k );
//                        float s1 = audiofiles[f]->sample( n + 1, k );
//                        float isample = s0 * (1.0-fract) + s1 * fract; // linear interpolation
//                        buffer[ i * buffer.getNumChannels() + k] = isample;
//                    }
//                    else
//                        buffer[ i * buffer.getNumChannels() + k] = 0.0f;
//                }

//                playheads[f] += steps[f];
//            }
//            else
//            {
//                buffer[ i * buffer.getNumChannels() ] = 0.0f;
//                buffer[ i * buffer.getNumChannels() + 1] = 0.0f;
//                playheads[f] = std::numeric_limits<int>::max();
//            }
//        }
//    }
//}


////--------------------------------------------------------------
void ofApp::keyPressed(int key){

//    if ( key == '7' ) playheadControls[2] = 0.0;
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

////--------------------------------------------------------------
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
//    audiofiles[0]->load( dragInfo.files[0] );
}


//--------------------------------------------------------------
//void ofApp::exit(){
//    ofSoundStreamClose();
//}
