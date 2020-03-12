#pragma once
#include "ofMain.h"
#include "ofxOsc.h"
#include "iterator"
#include "string"
//#include "ofxHPVPlayer.h"

class TidalEvent {
public:
    string s = "";
    int n = 0;
    tuple< string, int, int > sound; // <s, n, orbit>
    tuple<int, int, int, int, int> orbit; // <num, index, size, min, max
    ushort index = 10;
    float length = 1;
    float cycle = 0;
    float cps = .0;
    int startBar = 0;
    int bar = 0;
    float fract = .0;
    float latency = .0;
    float timeStamp = .0;
};

class ofxTidalCycles {
public:
    ofxTidalCycles(int port, int barBuffer);
    void update();

    void drawGrid(float left, float top, float width, float height);
    void drawBg(float left, float top, float width, float height);
    void drawNotes(float left, float top, float width, float height);
    void drawInstNames(float left, float top, float height);
    void drawOrbNumbers(float left, float top, float width, float height);
    void drawOrbGrid(float left, float top, float width, float height);
    void drawGraph(float top);

    void beatShift();
    void beatMonitor();
    void calcStat();

    ofxOscReceiver receiver;
    vector<TidalEvent> events;
    vector< tuple<string, int, int> > soundBuffer; // s, n, orbit
    map< int, set< pair<string, int> > > orbSounds; // orbit, <s, n>
    map< int, vector<int> > minmax;

    vector<int> activeOrbs;
    float orbCellHeight;
    float startBar;
    static const int maxBar = 4;

//    int bar;
    int lastBar;
    int barBuffer;
    static const int noteMax = 1024 * 8;
    size_t startTime = ofGetElapsedTimeMillis();

    int resolution;
    static const int max1 = 128;
    static const int max2 = 64;
    int eventMatrix[max1][max2];
    float syncopation[max1];
    //double entropy[max1];
    //double jointEntropy[max1];
    //double mutualInformation[max1];
    int eventsNum[max1];
    int bgAlpha[max1];
};

