#pragma once
#include "ofMain.h"
#include "ofxOsc.h"
//#include "ofxHPVPlayer.h"

class TidalMsg {
public:
    string s {""};
    int n {};
    // <s, n, orbit>:
    // tuple< string, int, int > sound;
    // <orb-num, orb-index, size, min, max>:
    // tuple<int, int, int, int, int> orbit;
    int orbnum {};
    int orbindex {};
    int orbsize {};
    int orb_minnum {};
    int orb_maxnum {};

    ushort index {10};
    float cycle {.0f};
    float cps {.0f};
    int bar {0};
    float fract {.0f};
    float latency {.0f};
    float delta {.0f};
    bool haveLegato {false};
    float legato {1.0f};
    float length {.5f};
    float time {};
};

class ofxTidalCycles {
public:
    ofxTidalCycles( int port );
    void update();
    void drawOscMsg( bool argtype );

    ofxOscReceiver receiver;
    ofxOscMessage m;
    int currentMsgString;

    vector< TidalMsg > tidalmsgs;
    // s, n, orbit
    vector< tuple< string, int, int > > cycleBuffer;
    // orbit, <s, n>
    map< int, set< pair< string, int > > > orbSounds;
    map< int, vector< string > > orbUniqueS;
    map<int, vector< int > > minmax;

    vector<int> activeOrbs;

    ushort maxBar {2};

    static const ushort noteMax {164}; // 1024 * 8;

    uint8_t resolution;
    uint startTime;
    uint timer {};
    ushort counter {};
    int lastBar {};
};

