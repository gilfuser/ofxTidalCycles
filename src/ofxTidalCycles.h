#pragma once
#include "ofMain.h"
#include "ofxOsc.h"
//#include "ofxHPVPlayer.h"

class TidalEvent {
public:
    string s = "";
    int n = 0;
    // <s, n, orbit>:
    tuple< string, int, int > sound;

    // <orb-num, orb-index, size, min, max>:
    tuple<int, int, int, int, int> orbit;

    ushort index = 10;
    float cycle = .0f;
    float cps = .0f;
    int bar = 0;
    float fract = .0f;
    float latency = .0f;
    float delta = .0f;
    bool haveLegato = false;
    float legato = 1.0f;
    float length = .5f;
};

class ofxTidalCycles {
public:
    ofxTidalCycles(int port/*, uint8_t barBuffer*/);
    void update();
    void drawOscMsg( bool argtype );

    ofxOscReceiver receiver;
    ofxOscMessage m;
    int currentMsgString;

//    uint counter = 0;
    vector< TidalEvent > events;
    vector< tuple< string, int, int > > eventBuffer; // s, n, orbit
    // orbit, <s, n>
    map< int, set< pair<string, int> > > orbSounds;
    map< int, vector<string> > orbUniqueS;
    map< int, vector<int> > minmax;

    vector<int> activeOrbs;

    ushort maxBar = 4;

    static const ushort noteMax = 256; // 1024 * 8;

    uint8_t resolution;
};

