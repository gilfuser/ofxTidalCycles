#include "ofxTidalCycles.h"

ofxTidalCycles::ofxTidalCycles(int port ) {
    receiver.setup(port);
    tidalmsgs.reserve(128);
    cycleBuffer.reserve(64);
}

void ofxTidalCycles::update() {
    ofSetColor(255);
//    uint8_t beatCount;
    while (receiver.hasWaitingMessages())
    {
    //    ofxOscMessage m;
        receiver.getNextMessage(m);
        if (m.getAddress() == "/play2")
        {
            TidalMsg msg;
            for ( uint8_t i = 0; i < m.getNumArgs(); i += 2)
            {
                if (m.getArgAsString(i) == "cycle")
                {
                    float bar;
                    float fract = modff(m.getArgAsFloat(i + 1), & bar );
                    msg.cycle = m.getArgAsFloat(i + 1);
                    msg.fract = fract;
                    msg.bar =int( bar ); // % 4;
                    // cout << "msg fract " << msg.fract << endl;
                    // cout << "msg bar " << msg.bar << endl;
                }
                else if ( m.getArgAsString(i) == "cps" )
                    msg.cps = m.getArgAsFloat(i + 1);

                else if ( m.getArgAsString(i) == "delta" )
                    msg.delta = m.getArgAsFloat(i + 1);

                else if ( m.getArgAsString(i) == "legato" )
                {
                    msg.haveLegato = true;
                    msg.legato = m.getArgAsFloat(i + 1);
                }
                else if (m.getArgAsString(i) == "n")
                    msg.n = m.getArgAsInt(i + 1);

                else if ( m.getArgAsString(i) == "orbit" )
                {
                    msg.orbnum = m.getArgAsInt(i + 1);
                    // get orbit index
                        msg.orbindex = ofFind( activeOrbs, msg.orbnum );
                    if ( not ofContains( activeOrbs, msg.orbnum ) )
                    {
                        activeOrbs.emplace_back( msg.orbnum);
                        ofSort( activeOrbs );
                        // update orbits indexes in all tidalmsgs
                        for ( auto msgs : tidalmsgs )
                        {
                            if ( msgs.orbnum == msg.orbnum )
                            {
                                msgs.orbsize = msg.orbsize;
                                msgs.orbindex = msg.orbindex;
                            }
                        }
                    }
                }
                else if (m.getArgAsString(i) == "s")
                {
                    msg.s = m.getArgAsString(i + 1);
                    // get<0>(msg.sound) = msg.s;

                    bool newInst = true;
                    for (size_t i = 0; i < cycleBuffer.size(); i++)
                    {
                        if ( msg.s == get<0>(cycleBuffer[i]) and msg.n == get<1>(cycleBuffer[i])
                        and msg.orbnum == get<2>(cycleBuffer[i]) )
                        {
                            newInst = false;
                            msg.index = i;
                        }
                    }
                    if ( not ofContains( minmax[ msg.orbnum], msg.n ) )
                    {
                        transform( begin( orbSounds[msg.orbnum] ),
                            end( orbSounds[ msg.orbnum ] ),
                            back_inserter( minmax[msg.orbnum] ),
                            [](auto const& pair) { return pair.second; } );
                    }

                    // orbSounds map get orbits sizes and min max
                    orbSounds[ msg.orbnum ].insert( make_pair( msg.s, msg.n) ) ;
                    msg.orbsize = orbSounds[ msg.orbnum ].size();

                    if (newInst)
                    {
                        cycleBuffer.emplace_back( make_tuple( msg.s, msg.n, msg.orbnum ) );
                        ofSort( cycleBuffer );
                        orbUniqueS[msg.orbnum].emplace_back(msg.s);
                    }
                }
                else if (m.getArgAsString(i) == "zz" && cycleBuffer.size() > 1 )
                {
                    //erace unused inst
                    for ( auto thisSound : cycleBuffer )
                    {
                        bool instExist = false;

                        for ( auto thismsg : tidalmsgs )
                        {
                            if ( thismsg.bar > tidalmsgs[ tidalmsgs.size() - 1 ].bar /* - maxBar * 2*/ )
                            {
                                if
                                ( 
                                    get<0>(thisSound) == thismsg.s
                                    and get<1>(thisSound) == thismsg.n
                                    and get<2>(thisSound) == thismsg.orbnum
                                )
                                    instExist = true;
                            }
                        }
                        if ( instExist == false )
                        {
                            minmax[msg.orbnum].erase(
                                remove( begin( minmax[msg.orbnum] ),
                                end( minmax[msg.orbnum] ), get<1>(thisSound) ),
                            end( minmax[msg.orbnum] ) );

                            orbSounds[get<2>(thisSound)].erase(
                            { get<0>(thisSound), get<1>(thisSound) } );

                            msg.orbsize = orbSounds[msg.orbnum].size();

                            cycleBuffer.erase(
                                remove( cycleBuffer.begin(), cycleBuffer.end(), thisSound ),
                            cycleBuffer.end() );
                            ofSort(cycleBuffer);

                            orbUniqueS[msg.orbnum].erase(
                                    remove( begin( orbUniqueS[msg.orbnum] ),
                                    end( orbUniqueS[msg.orbnum] ), get<0>(thisSound) ), end( orbUniqueS[msg.orbnum] ) );
                            if ( find( begin( minmax[msg.orbnum] ),
                                      end( minmax[msg.orbnum] ),
                                      msg.n ) != end( minmax[msg.orbnum] ) )
                            {
                                transform( begin( orbSounds[msg.orbnum] ),
                                    end( orbSounds[ msg.orbnum ] ),
                                    back_inserter( minmax[msg.orbnum] ),
                                    [](auto const& pair) { return pair.second; }
                                );
                            }
                        }
                    }
                    for ( auto thisOrb : activeOrbs )
                    {
                        bool orbExist = false;
                        for ( auto thismsg : tidalmsgs )
                        {
                            if ( thismsg.bar > tidalmsgs[tidalmsgs.size() - 1].bar - maxBar * 2 ) {
                                if ( thisOrb == thismsg.orbnum ) {
                                    orbExist = true;
                                }
                            }
                        }
                        if ( orbExist == false )
                        {
                            activeOrbs.erase(
                                remove(activeOrbs.begin(), activeOrbs.end(), thisOrb),
                                activeOrbs.end()
                            );
                            // ofRemove( activeOrbs, thisOrb );
                            ofSort(activeOrbs);

                            msg.orbindex = ofFind( activeOrbs, msg.orbnum );
                        }
                    }
                    const auto min_iter = min_element(begin( minmax[msg.orbnum] ),
                            end( minmax[msg.orbnum] ) );
                    const auto max_iter = max_element(begin( minmax[msg.orbnum] ),
                            end( minmax[msg.orbnum] ));
                    if ( min_iter == minmax[msg.orbnum].end()) {}
                    else
                    {
                        auto const min = *min_iter;
                        msg.orb_minnum = min;
                    }
                    if ( max_iter == minmax[msg.orbnum].end()) {}
                    else
                    {
                        auto const max = *max_iter;
                        msg.orb_maxnum = max;
                    }
                }
            // timer = ofGetElapsedTimeMillis() - startTime;
            }
                    tidalmsgs.emplace_back(msg);

                    // if ( tidalmsgs.size() > cycleBuffer.size()* 4 )
                    if ( tidalmsgs.size() >= noteMax )
                        tidalmsgs.erase(tidalmsgs.begin());
        }
    }
}

void ofxTidalCycles::drawOscMsg( bool argtype = false ){

    string msgString;
    msgString = m.getAddress();
    msgString += ":";
    for(size_t i = 0; i < m.getNumArgs(); i++)
    {
        // get the argument type
        msgString += " ";
        if ( argtype == true )
        {
            msgString += m.getArgTypeName(i);
            msgString += ":";
        }

        // display the argument - make sure we get the right type
        if(m.getArgType(i) == OFXOSC_TYPE_INT32)
        {
            msgString += ofToString(m.getArgAsInt32(i));
        }
        else if(m.getArgType(i) == OFXOSC_TYPE_FLOAT)
        {
            msgString += ofToString(m.getArgAsFloat(i));
        }
        else if(m.getArgType(i) == OFXOSC_TYPE_STRING)
        {
            msgString += m.getArgAsString(i);
        }
        else{
            msgString += "unhandled argument type " + m.getArgTypeName(i);
        }
    }

    ofDrawBitmapStringHighlight("listening for osc messages", 10, 20);
    ofDrawBitmapStringHighlight( msgString, 10, 40 );

}
/*
void ofxTidalCycles::beatShift() {
    for (int i = 0; i < max1; i++) {
        for (int j = resolution; j < max2; j++) {
            msgMatrix[i][j - resolution] = msgMatrix[i][j];
        }
        for (int j = max2 - resolution; j < max2; j++) {
            msgMatrix[i][j] = 0;
        }
    }
}
void ofxTidalCycles::beatMonitor() {
    cout << "-------------------------" << endl;
    int instNumMax = cycleBuffer.size();
    for ( auto msg : tidalmsgs ) {
        msgMatrix[msg.num][int(msg.beatCount) + max2 - resolution] = 1;
        }
    for (int i = 0; i < instNumMax; i++) {
        cout << "part " << i << " : ";
        for (int j = 0; j < max2; j++) {
//            cout << msgMatrix[i][j];
        }
        cout << endl;
    }
    cout << endl;
}

void ofxTidalCycles::calcStat() {
    for (int i = 0; i < max1; i++) {
        syncopation[i] = 0.0;
        tidalmsgsNum[i] = 0.0;
    }
    int instNumMax = cycleBuffer.size();
    for (int i = 0; i < instNumMax; i++) {
        //calculate syncopation
        string bitStr;
        for (uint8_t j = 0; j < resolution; j++) {
            bitStr += to_string(msgMatrix[i][max2 - resolution + j]);
        }
        int digit = stoi(bitStr, nullptr, 2);
        syncopation[i] = SG[digit];

                cout << "digit " << i << " : "
                        << bitStr << " : " << digit
                        << " syncopation : " << SG[digit]
                        << endl;

        //calc note count
        int num = 0;
        for (int j = 0; j < resolution; j++) {
            if (msgMatrix[i][max2 - resolution + j] == 1) {
                num++;
            }
        }
        tidalmsgsNum[i] = num;
    }
}
        */
