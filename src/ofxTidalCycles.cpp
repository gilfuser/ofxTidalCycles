#include "ofxTidalCycles.h"

ofxTidalCycles::ofxTidalCycles(int port ) {
    receiver.setup(port);
    events.reserve(128);
    eventBuffer.reserve(64);
}

void ofxTidalCycles::update() {
    ofSetColor(255);
//    uint8_t beatCount;
    while (receiver.hasWaitingMessages())
    {
//        ofxOscMessage m;
        receiver.getNextMessage(m);
        if (m.getAddress() == "/play2")
        {
            TidalEvent event;
            for ( uint8_t i = 0; i < m.getNumArgs(); i += 2)
            {
                if (m.getArgAsString(i) == "cycle")
                {
                    float bar;
                    float fract = modff(m.getArgAsFloat(i + 1), &bar);
//                    }
                    event.cycle = m.getArgAsFloat(i + 1);
                    event.fract = fract;
                    event.bar = bar;
                }
                else if (m.getArgAsString(i) == "cps")
                    event.cps = m.getArgAsFloat(i + 1);

                else if (m.getArgAsString(i) == "delta")
                    event.delta = m.getArgAsFloat(i + 1);
                else if (m.getArgAsString(i) == "legato")
                {
                    event.haveLegato = true;
                    event.legato = m.getArgAsFloat(i + 1);
                }
                else if (m.getArgAsString(i) == "n")
                {
                    event.n = m.getArgAsInt(i + 1);
                    get<1>(event.sound) = event.n;
                }
                else if (m.getArgAsString(i) == "orbit")
                {
                    get<0>(event.orbit) = m.getArgAsInt(i + 1);
                    get<2>(event.sound) = get<0>(event.orbit);
                    // get orbit index
                    auto match = find(activeOrbs.begin(), activeOrbs.end(), get<0>(event.orbit));
                    if( match != activeOrbs.end() )
                        get<1>(event.orbit) = match - activeOrbs.begin();
                    if ( !( find( activeOrbs.begin(), activeOrbs.end(),
                                  get<0>(event.orbit) ) != activeOrbs.end() ) )
                    {
                        activeOrbs.push_back( get<0>(event.orbit));
                        ofSort(activeOrbs);
                        // update orbits indexes in all events
                        for (auto evt : events )
                        {
                            if ( get<0>(evt.orbit) == get<0>(event.orbit) )
                                get<1>(evt.orbit) = get<1>(event.orbit);
                        }
                    }
                }
                else if (m.getArgAsString(i) == "s")
                {
                    event.s = m.getArgAsString(i + 1);
                    get<0>(event.sound) = event.s;

                    bool newInst = true;
                    for (size_t i = 0; i < eventBuffer.size(); i++)
                    {
                        if ( event.sound == eventBuffer[i] )
                        {
                            newInst = false;
                            event.index = i;
                        }
                    }
                    if ( find( begin( minmax[get<0>(event.orbit)] ), end( minmax[get<0>(event.orbit)] ), event.n ) == end( minmax[get<0>(event.orbit)] ) )
                    {
                        transform( begin( orbSounds[get<0>(event.orbit)] ),
                                end( orbSounds[ get<0>(event.orbit) ] ),
                                back_inserter( minmax[get<0>(event.orbit)] ),
                                [](auto const& pair) { return pair.second; }
                        );
                    }

                    // orbSounds map get orbits sizes and min max
                    orbSounds[ get<0>(event.orbit) ].insert( make_pair(event.s, event.n) ) ;
                    get<2>(event.orbit) = orbSounds[get<0>(event.orbit)].size();

                    if (newInst)
                    {
                        eventBuffer.push_back(event.sound);
                        ofSort(eventBuffer);

//                    if ( find( begin( orbUniqueS[get<0>(event.orbit)] ), end( orbUniqueS[get<0>(event.orbit)] ), event.s ) == end( orbUniqueS[get<0>(event.orbit)] ) )
                        orbUniqueS[get<0>(event.orbit)].push_back(event.s);
                    }
                }
                else if (m.getArgAsString(i) == "zz" && eventBuffer.size() > 1 )
                {
                    //erace unused inst
                for ( auto thisSound : eventBuffer )
                {
                    bool instExist = false;

                    for ( auto thisEvent : events )
                    {
                        if ( thisEvent.bar > events[events.size() - 1].bar - maxBar * 2) {
                            if( thisSound == thisEvent.sound )
                                instExist = true;
                        }
                    }
                    if ( instExist == false )
                    {
                        minmax[get<0>(event.orbit)].erase(
                                    remove( begin( minmax[get<0>(event.orbit)] ),
                                    end( minmax[get<0>(event.orbit)] ), get<1>(thisSound) ),
                                end( minmax[get<0>(event.orbit)] ) );

                        orbSounds[get<2>(thisSound)].erase(
                        { get<0>(thisSound), get<1>(thisSound) } );

                        get<2>(event.orbit) = orbSounds[get<0>(event.orbit)].size();

                        eventBuffer.erase( remove( eventBuffer.begin(), eventBuffer.end(), thisSound ), eventBuffer.end() );
                        ofSort(eventBuffer);
//                    if ( find( begin( orbUniqueS[get<0>(event.orbit)] ), end( orbUniqueS[get<0>(event.orbit)] ), get<0>(thisSound) ) != end( orbUniqueS[get<0>(event.orbit)] ) )
                        orbUniqueS[get<0>(event.orbit)].erase(
                                    remove( begin( orbUniqueS[get<0>(event.orbit)] ),
                                    end( orbUniqueS[get<0>(event.orbit)] ), get<0>(thisSound) ), end( orbUniqueS[get<0>(event.orbit)] ) );
                        if ( find( begin( minmax[get<0>(event.orbit)] ),
                                      end( minmax[get<0>(event.orbit)] ),
                                      event.n ) != end( minmax[get<0>(event.orbit)] ) )
                        {
                            transform( begin( orbSounds[get<0>(event.orbit)] ),
                                    end( orbSounds[ get<0>(event.orbit) ] ),
                                    back_inserter( minmax[get<0>(event.orbit)] ),
                                    [](auto const& pair) { return pair.second; }
                            );
                        }
                    }
                }
                    for ( auto thisOrb : activeOrbs )
                    {
                        bool orbExist = false;
                        for ( auto thisEvent : events )
                        {
                            if ( thisEvent.bar > events[events.size() - 1].bar - maxBar * 2 ) {
                                if ( thisOrb == get<0>(thisEvent.orbit) ) {
                                    orbExist = true;
                                }
                            }
                        }
                        if ( orbExist == false )
                        {
                            activeOrbs.erase(
                                        remove(activeOrbs.begin(), activeOrbs.end(), thisOrb),
                                        activeOrbs.end() );
                            ofSort(activeOrbs);
                            get<1>(event.orbit) = activeOrbs.size() - 1;

                            auto match = find(
                                        activeOrbs.begin(), activeOrbs.end(),  get<0>(event.orbit));
                            if( match != activeOrbs.end() )
                                get<1>(event.orbit) = match - activeOrbs.begin();
                        }
                    }
                    const auto min_iter = min_element(begin( minmax[get<0>(event.orbit)] ),
                            end( minmax[get<0>(event.orbit)] ) );
                    const auto max_iter = max_element(begin( minmax[get<0>(event.orbit)] ),
                            end( minmax[get<0>(event.orbit)] ));
                    if ( min_iter == minmax[get<0>(event.orbit)].end())
                    {}
                    else
                    {
                        auto const min = *min_iter;
                        get<3>(event.orbit) = min;
                    }
                    if ( max_iter == minmax[get<0>(event.orbit)].end())
                    {}
                    else
                    {
                        auto const max = *max_iter;
                        get<4>(event.orbit) = max;
                    }
                }
            }
            events.push_back(event);
            if (events.size() > noteMax)
            {
                events.erase(events.begin());
            }
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
            eventMatrix[i][j - resolution] = eventMatrix[i][j];
        }
        for (int j = max2 - resolution; j < max2; j++) {
            eventMatrix[i][j] = 0;
        }
    }
}

void ofxTidalCycles::beatMonitor() {
    cout << "-------------------------" << endl;
    int instNumMax = eventBuffer.size();
    for ( auto event : events ) {
        eventMatrix[event.num][int(event.beatCount) + max2 - resolution] = 1;
        }
    for (int i = 0; i < instNumMax; i++) {
        cout << "part " << i << " : ";
        for (int j = 0; j < max2; j++) {
//            cout << eventMatrix[i][j];
        }
        cout << endl;
    }
    cout << endl;
}

void ofxTidalCycles::calcStat() {
    for (int i = 0; i < max1; i++) {
        syncopation[i] = 0.0;
        eventsNum[i] = 0.0;
    }
    int instNumMax = eventBuffer.size();
    for (int i = 0; i < instNumMax; i++) {
        //calculate syncopation
        string bitStr;
        for (uint8_t j = 0; j < resolution; j++) {
            bitStr += to_string(eventMatrix[i][max2 - resolution + j]);
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
            if (eventMatrix[i][max2 - resolution + j] == 1) {
                num++;
            }
        }
        eventsNum[i] = num;
    }
}
        */

