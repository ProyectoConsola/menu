#include <Arduino.h>
#include "fabgl.h"
#include <Ps3Controller.h>
#include "state.hpp"

#include "menu.hpp"

void setup()
{
    Serial.begin(115200);
    State::init();
}

void loop()
{
    menu::execute();
    /* Se realiza la ejecución del juego seleccionado. */
}
