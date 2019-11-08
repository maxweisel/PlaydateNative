//
//  MusicBox.h
//  PlaydateNative
//
//  Created by Max Weisel on 10/20/19.
//  Copyright © 2019 Max Weisel. All rights reserved.
//

#pragma once

// TODO: This is gross. Why doesn't "Playdate/Playdate.h" work?
#include "../../Playdate/Playdate.h"

using namespace Playdate;

class MusicBox : public Application {
public:
    MusicBox() {
        _sample = make_shared<Audio::Sample>("musicbox");
        _player.SetSample(_sample);
        _player.Play();
    }
    
    
    
    void Update() override {
        
    }
private:
    std::shared_ptr<Audio::Sample> _sample;
    Audio::SamplePlayer _player;
};
