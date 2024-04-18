#include <Arduino.h>
#include "fabgl.h"
#include <Ps3Controller.h>
#include "state.hpp"

#include "menu.hpp"
#include "pong.hpp"
#include "spaceInvaders.hpp"
#include "bycicle.hpp"

void setup()
{
    Serial.begin(115200);
    State::init();
}

void loop()
{
    bool isGameSelected = false;
    State::Game selected;
    if(!isGameSelected){
        selected = menu::execute();
    }
    Serial.println("IDK");
    switch(selected){
        case State::Game::SPACE_INVADERS:
            isGameSelected = true;
            spaceInvaders::execute();
            break;
        case State::Game::PONG:
            isGameSelected = true;
            pong::execute();
            break;
        case State::Game::BICYCLE:
            isGameSelected = true;
            bycicle::execute();
            break;
        default:
            break;
    }
}
