#pragma once

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

    void audioOut(ofSoundBuffer & buffer);

    ofxTidalCycles *tidal;

    ofxAudioFile *audiofile;
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
};

