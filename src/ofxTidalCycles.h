#pragma once
#include "ofMain.h"
#include "ofxOsc.h"
//#include "iterator"
//#include "string"
//#include "ofxHPVPlayer.h"

// max number of strings to display
#define NUM_MSG_STRINGS 8

class TidalEvent {
public:
    string s = "";
    int n = 0;
    // <s, n, orbit>:
    tuple< string, int, int > sound;
    // <num, index, size, min, max>:
    tuple<int, int, int, int, int> orbit;
    ushort index = 10;
//    float length = 1.0;
    float cycle = .0f;
    float cps = .0f;
//    uint8_t startBar = 0;
    int bar = 0;
    float fract = .0f;
    float latency = .0f;
    float delta = .0f;
//    float timeStamp = .0;
//    ulong counter = 0;
};

class ofxTidalCycles {
public:
    ofxTidalCycles(int port/*, uint8_t barBuffer*/);
    void update();
    void drawOscMsg();
//    void drawBg(float left, float top, float width, float height);
//    void drawGraph(float top);
//    void beatShift();
//    void beatMonitor();
//    void calcStat();

    ofxOscReceiver receiver;
    ofxOscMessage m;
    int currentMsgString;
    string msgStrings[NUM_MSG_STRINGS];
    float timers[NUM_MSG_STRINGS];

    uint counter = 0;
    vector<TidalEvent> events;
    vector< tuple<string, int, int> > eventBuffer; // s, n, orbit
    map< int, set< pair<string, int> > > orbSounds; // orbit, <s, n>
    map< int, vector<int> > minmax;

    vector<int> activeOrbs;
//    float startBar;
    // delta, n
    tuple< float, vector<int> > msgBuffer;

    static const int maxBar = 4;

//    int bar;
//    uint8_t lastBar;
//    uint8_t barBuffer;
    static const ushort noteMax = 256; // 1024 * 8;
//    size_t startTime = ofGetElapsedTimeMillis();

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

