//#pragma once

#include "ofMain.h"
#include "ofxTidalCycles.h"
#include "ofxAudioFile.h"

class ofApp : public ofBaseApp {

public:
    void setup();
    void update();
    void draw();
//    void exit();

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
//    void drawWaveforms(float x, float y, float w, float h);
    void drawGrid(float left, float top, float width, float height);
    void drawOrbNumbers(float left, float top, float width );
    void drawInstNames(float left, float top, float h );

//    void audioOut(ofSoundBuffer &buffer);
    void drawNotes (ofxTidalCycles & drawNotes);

    ofxTidalCycles* tidal;
    ulong lastCount {};
    float orbCellHeight;

    ofxAudioFile* audiofile;
    vector<string> sounds;
    map< string, vector<ofxAudioFile*> > audiofiles;

    map< string, int> hist;

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

    const int colors[9][4][3] {
        { {110, 87, 115}, {212, 93, 121}, {234, 144, 133}, {233, 226, 208} },
        { {238, 249, 191}, {167, 233, 175}, {117, 183, 158}, {106, 140, 175} },
        { {248, 177, 149}, {246, 114, 128}, {192, 108, 132}, {108, 86, 123} },
        { {252, 248, 232}, {236, 223, 200}, {236, 179, 144}, {223, 120, 97} },
        { {110, 87, 115}, {212, 80, 121}, {234, 144, 133}, {233, 225, 204} },
        { {255, 178, 167}, {230, 115, 159}, {204, 14, 116}, {121, 12, 90} },
        { {32, 64, 81}, {59, 105, 120}, {132, 169, 172}, {202, 232, 213} },
        { {54, 48, 98}, {77, 76, 125}, {130, 115, 151}, {216, 185, 195} },
        { {133, 163, 146}, {245, 185, 113}, {253, 217, 152}, {255, 236, 199} }
    };

//        std::mutex audioMutex;
//    ofSoundStream soundStream;
//    ofSoundBuffer lastBuffer;
//    ofPolyline waveform;
    float rms;

uint startTime;
uint timer {};
ushort counter {0};
};

