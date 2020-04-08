#pragma once
#include "ofMain.h"
#include "ofxOsc.h"
//#include "iterator"
//#include "string"
//#include "ofxHPVPlayer.h"

class TidalEvent {
public:
    string s = "";
    int n = 0;
    tuple< string, int, int > sound; // <s, n, orbit>
    tuple<int, int, int, int, int> orbit; // <num, index, size, min, max
    ushort index = 10;
//    float length = 1.0;
    float cycle = 0;
    float cps = .0;
    uint8_t startBar = 0;
    uint8_t bar = 0;
    float fract = .0;
    float latency = .0;
    float timeStamp = .0;
//    ulong counter = 0;
};

class ofxTidalCycles {
public:
    ofxTidalCycles(int port/*, uint8_t barBuffer*/);
    void update();
    void drawBg(float left, float top, float width, float height);
    void drawGraph(float top);
    void beatShift();
    void beatMonitor();
    void calcStat();

    ofxOscReceiver receiver;

    uint counter = 0;
    vector<TidalEvent> events;
    vector< tuple<string, int, int> > eventBuffer; // s, n, orbit
    map< int, set< pair<string, int> > > orbSounds; // orbit, <s, n>
    map< int, vector<int> > minmax;

    vector<int> activeOrbs;
//    float startBar;
    vector< vector<int> > msgBuffer = {{0}}; // n

    static const int maxBar = 4;

//    int bar;
//    uint8_t lastBar;
//    uint8_t barBuffer;
    static const ushort noteMax = 256; // 1024 * 8;
    size_t startTime = ofGetElapsedTimeMillis();

    uint8_t resolution;
//    static const ushort max1 = 256;
    static const ushort max2 = 128;
//    int eventMatrix[max1][max2];
//    float syncopation[max1];
    //double entropy[max1];
    //double jointEntropy[max1];
    //double mutualInformation[max1];
//    ushort eventsNum[max1];
//    ushort bgAlpha[max1];

};

