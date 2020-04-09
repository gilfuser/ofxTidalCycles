#include "ofxTidalCycles.h"
//#include "SyncopationData.h"

ofxTidalCycles::ofxTidalCycles(int port /*, uint8_t _barBuffer*/) {
    //    barBuffer = _barBuffer;
        receiver.setup(port);
//    lastBar = 0;
//    resolution = 16;
    events.reserve(128);
    eventBuffer.reserve(64);
//    msgBuffer[0] = .0f;
//    msgBuffer[1] = 0;

    //init arrays
//    for (ushort i = 0; i < max1; i++) {
//        syncopation[i] = 0;
//        eventsNum[i] = 0;
////        bgAlpha[i] = 0;
//        for (ushort j = 0; j < max2; j++) {
//            eventMatrix[i][j] = 0;
//        }
//    }
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
//            event.timeStamp = ofGetElapsedTimef();

            for ( uint8_t i = 0; i < m.getNumArgs(); i += 2)
            {

                if (m.getArgAsString(i) == "cycle")
                {
                    float bar;
                    float fract = modff(m.getArgAsFloat(i + 1), &bar);
//                    if (events.size() == 0) {
//                        startBar = bar;
//                    }
                    event.cycle = m.getArgAsFloat(i + 1);
                    event.fract = fract;
                    event.bar = bar;
                    counter++;
                    /*
                    beatCount = int(fract * resolution);
                    if (event.bar > lastBar) {
                        //beatMonitor();
//                        calcStat();
//                        beatShift();
                    }
                    lastBar = int(bar);
                    */
                }
                if (m.getArgAsString(i) == "cps")
                    event.cps = m.getArgAsFloat(i + 1);

                if (m.getArgAsString(i) == "delta")
                {
                    event.delta = m.getArgAsFloat(i + 1);
                    get<0>(msgBuffer) = event.delta;
                }
                if (m.getArgAsString(i) == "n")
                {
                    event.n = m.getArgAsInt(i + 1);
                    get<1>(event.sound) = event.n;
                    get<1>(msgBuffer).push_back(event.n);
                }
                if (m.getArgAsString(i) == "orbit")
                {
                    get<0>(event.orbit) = m.getArgAsInt(i + 1);
                    get<2>(event.sound) = get<0>(event.orbit);
                    // get orbit index
                    auto match = find(activeOrbs.begin(), activeOrbs.end(), get<0>(event.orbit));
                    if( match != activeOrbs.end() )
                    {
                        get<1>(event.orbit) = match - activeOrbs.begin();
//                        cout << "orb index:  " << get<1>(event.orbit) << std::endl;
                    }
                    if ( !( find( activeOrbs.begin(), activeOrbs.end(),
                                  get<0>(event.orbit) ) != activeOrbs.end() ) )
                    {
                        activeOrbs.push_back( get<0>(event.orbit));
                        ofSort(activeOrbs);
                        // update orbits indexes in all events
                        for (auto evt : events ) {
                            if ( get<0>(evt.orbit) == get<0>(event.orbit) ) {
                                get<1>(evt.orbit) = get<1>(event.orbit);
                            }
                        }
                    }
                }
                if (m.getArgAsString(i) == "s")
                {
                    event.s = m.getArgAsString(i + 1);
                    get<0>(event.sound) = event.s;

                    bool newInst = true;
                    for (size_t i = 0; i < eventBuffer.size(); i++)
                    {
                        if ( event.sound == eventBuffer[i] ) {
                            newInst = false;
                            event.index = i;
                        }
                    }
                    if ( !( find( begin( minmax[get<0>(event.orbit)] ), end( minmax[get<0>(event.orbit)] ), event.n ) != end( minmax[get<0>(event.orbit)] ) ) )
                    {
                        transform( begin( orbSounds[get<0>(event.orbit)] ),
                                end( orbSounds[ get<0>(event.orbit) ] ),
                                back_inserter( minmax[get<0>(event.orbit)] ),
                                [](auto const& pair) { return pair.second; }
                        );
                    }

                    // orbSounds map is used to get orbits sizes and min max
                    orbSounds[get<0>(event.orbit)].insert( make_pair(event.s, event.n) ) ;
                    get<2>(event.orbit) = orbSounds[get<0>(event.orbit)].size();

                    if (newInst)
                    {

//                        for (auto sInOrb : orbSounds ) {
//                            cout << " ADD orb: " << sInOrb.first << endl;
//                            for (auto snSet : sInOrb.second)
//                                cout << " sound: " << snSet.first << " note: "
//                                     << snSet.second << endl;
//                            cout << " size: " << sInOrb.second.size() << " X "
//                                 << get<2>(event.orbit) << endl;
//                        }

                        eventBuffer.push_back(event.sound);
                        ofSort(eventBuffer);
                    }
                }
                if (m.getArgAsString(i) == "zz" && eventBuffer.size() > 1 )
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

//                        for (auto minmax : minmax[get<0>(event.orbit)]) {
//                            cout << "minmax after erase orb " << get<0>(event.orbit)
//                                 << ": " << minmax << endl;
//                        }

                        orbSounds[get<2>(thisSound)].erase(
                        { get<0>(thisSound), get<1>(thisSound) } );

                        get<2>(event.orbit) = orbSounds[get<0>(event.orbit)].size();

//                        cout << "orb " << get<0>(event.orbit) << " size " <<
//                                get<2>(event.orbit) << " after remove" << endl;

                        eventBuffer.erase( remove( eventBuffer.begin(), eventBuffer.end(), thisSound ), eventBuffer.end() );

                        ofSort(eventBuffer);

                        if ( !( find( begin( minmax[get<0>(event.orbit)] ),
                                      end( minmax[get<0>(event.orbit)] ),
                                      event.n ) != end( minmax[get<0>(event.orbit)] ) ) )
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
//        cout << "orb " << get<0>(event.orbit) << " index " << get<1>(event.orbit) << " size " <<
//                get<2>(event.orbit) << "    min __ " << get<3>(event.orbit) << "    max ___ " <<
//                get<4>(event.orbit) << endl;

            //add to note matrix
//            for (size_t i = 0; i < events.size(); i++) {
//                eventMatrix[event.index][beatCount + max2 - resolution] = 1;
//            }
            if (events.size() > noteMax)
            {
                events.erase(events.begin());
                counter = 0;
            }
        }
//        counter++;
    }
}

void ofxTidalCycles::drawOscMsg(){

    // hide old messages
    for(int i = 0; i < NUM_MSG_STRINGS; i++)
    {
        if(timers[i] < ofGetElapsedTimef())
        {
            msgStrings[i] = "";
        }
    }
    string msgString;
    msgString = m.getAddress();
    msgString += ":";
    for(size_t i = 0; i < m.getNumArgs(); i++)
    {

        // get the argument type
        msgString += " ";
        msgString += m.getArgTypeName(i);
        msgString += ":";

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

    // add to the list of strings to display
    msgStrings[currentMsgString] = msgString;
    timers[currentMsgString] = ofGetElapsedTimef() + 5.0f;
    currentMsgString = (currentMsgString + 1) % NUM_MSG_STRINGS;

    // clear the next line
    msgStrings[currentMsgString] = "";

    // draw recent messages
    for(int i = 0; i < NUM_MSG_STRINGS; i++)
    {
        ofDrawBitmapStringHighlight(msgStrings[i], 10, 40 + 15 * i);
    }
    ofDrawBitmapStringHighlight("listening for osc messages", 10, 20);

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

