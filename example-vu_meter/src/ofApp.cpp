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
    startTime = ofGetElapsedTimeMillis();
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
    if ( tidal->cycleBuffer.size() > 0 )
    {
        float h, y, w;
        for ( auto msg : tidal->tidalmsgs )
        {
            auto instrs = tidal->orbSounds[ msg.orbnum ];

            auto s_index = ofFind( tidal->orbUniqueS[ msg.orbnum ], msg.s );

            for ( auto const & x : instrs )
                ++hist[ x.first ];

            // timer = ofGetElapsedTimeMillis() - startTime;
            // for (auto const & p : hist)
            // {
                // if ( timer >= 500 ) {
                //    ofLog() << "total " << instrs.size() << " " << p.first << " occurs " << " index " << s_index << " | " << p.second << " times";
                // }
            // }

            auto lastmsg = tidal->tidalmsgs[ tidal->tidalmsgs.size() - 1 ];

            if ( audiofiles.find(msg.s) == audiofiles.end() )
                w = width * msg.cps * msg.delta / tidal->maxBar -1;
            else
                w = width * msg.cps * audiofiles[msg.s][msg.n % audiofiles[msg.s].size()]->length() / sampleRate / tidal->maxBar -1;
            if (msg.haveLegato == true)
                w = ofClamp(w, 0, width * msg.cps * msg.delta / tidal->maxBar * msg.legato );

            int bar = lastmsg.bar - msg.bar;
            float x = ofMap(bar - msg.fract, 0, tidal->maxBar, width, 0) + left;

            h = orbCellHeight / msg.orbsize - 1;

            y = ofGetHeight() - ( 2 * top ) - ofMap(
                msg.n,
                msg.orb_minnum,
                msg.orb_maxnum,
                orbCellHeight * msg.orbindex + ( orbCellHeight / hist.size() * s_index ),
                ( orbCellHeight * msg.orbindex + ( orbCellHeight / hist.size() * ( s_index + 1 ) ) ) - h
            ) - h + top;

//            y = ofMap( y, orbCellHeight * msg.orbindex, orbCellHeight * msg.orbindex + ( orbCellHeight ) - h, 0, 0);

            if ( x >= left and x < ofGetWidth() - left )
            {
                ofFill();
                ofSetColor( 255 );
                ofDrawRectangle( x, y , w, h );
                ofDrawBitmapStringHighlight( ofToString( msg.n ), x, y );
//                drawWaveforms(x, y, w, h);
//             ofDrawBitmapStringHighlight( tidalmsgsBuffer[i], left + 5, y);
            /*
            cout << endl << ".............................." << endl;
            cout << "bar " << bar << endl;
            cout << "msg bar " << msg.bar << endl;
            cout << "fract " << msg.fract << endl;
            cout << "x " << x << endl;
            cout << "delta " << get<0>(tidal->msgBuffer) << endl;
            cout << "cps " << msg.cps << endl;
            cout << "width " << w << endl;
            */
            /*
            cout << "last msg index " << lastmsg.index << endl;
            */
            };
            hist.clear();
            // timer = ofGetElapsedTimeMillis() - startTime;
            // if ( timer >= 500 ) {
            //     ofLog() << "s_index " << s_index;
                // startTime = ofGetElapsedTimeMillis();
                // ofLog() << "msg.s " << ofToString( msg.s );
                // ofLog() << "msg.n" << ofToString( msg.n );
                // ofLog() << "msg.orbnum " + ofToString( msg.orbnum );
                // ofLog() << "msg.orb_minnum " + ofToString( msg.orb_minnum );
                // ofLog() << "msg.orb_maxnum " + ofToString( msg.orb_maxnum );
                // ofLog() << "msg.orbindex " + ofToString( msg.orbindex );
                // ofLog() << "msg.orbsize " << ofToString(msg.orbsize);
                // cout << "----------------------------------------" << endl;
                // cout << "orb sounds" << endl;
                // for ( auto osound : tidal->orbSounds[ msg.orbnum ] ) {
                //     cout << "size " << tidal->orbSounds[msg.orbnum].size() << endl;
                //     cout << "s " << osound.first << " n " << osound.second << endl;
                // }
                // }
        }
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
//    sort(orb.begin(), orb.end());
    for ( auto msg : tidal->tidalmsgs ) {
        auto instrs = tidal->orbUniqueS[msg.orbnum].size();
        h = orbCellHeight / msg.orbsize;
        y = ofGetHeight() - (2 * top) - ofMap( msg.n, msg.orb_minnum, msg.orb_maxnum, orbCellHeight * msg.orbindex / instrs, orbCellHeight * msg.orbindex + orbCellHeight - h ) + top;
        ofDrawBitmapStringHighlight( msg.s, left + 5, y);
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
