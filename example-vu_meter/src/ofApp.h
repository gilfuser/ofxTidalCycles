//#pragma once

#include "ofMain.h"
#include "ofxTidalCycles.h"

#include "ofxAudioFile.h"

class ofApp : public ofBaseApp {

public:
    void setup();
    void update();
    void draw();
    void exit();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    void drawNotes(float left, float top, float width );
    void drawWaveforms(float x, float y, float h);
    void drawGrid(float left, float top, float width, float height);
    void drawOrbNumbers(float left, float top, float width );
    void drawInstNames(float left, float top );

    void audioOut(ofSoundBuffer &buffer);
//    void drawNotes (ofxTidalCycles & drawNotes);

    ofxTidalCycles* tidal;
//    float lastTime = 0.0;
//    int lastN = 99;
    ulong lastCount = 0;
    float orbCellHeight;

    ofxAudioFile* audiofile;
    vector<ofxAudioFile*> audiofiles;
    float maxL;

    ofMutex waveformMutex;
    double playhead;
    vector<double> playheads;
    double step;
    vector<double> steps;
    std::atomic<double> playheadControl;
    vector<double> playheadControls;
    double sampleRate;

    double wavePhase;
    double pulsePhase;

//        std::mutex audioMutex;
    ofSoundStream soundStream;
    ofSoundBuffer lastBuffer;
    ofPolyline waveform;
    float rms;
//    float color = ofRandom(255);
};

