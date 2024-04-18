#include <Arduino.h>
#include "fabgl.h"
#include <Ps3Controller.h>
#include "state.hpp"

#include "menu.hpp"
#include "pong.hpp"
#include "spaceInvaders.hpp"

void setup()
{
    Serial.begin(115200);
    State::init();
}

void loop()
{
    State::Game selected = menu::execute();
    Serial.println("IDK");
    bool isGameSelected = false;
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
            //bycicle::execute();
            break;
        default:
            break;
    }
}
