#include "state.hpp"

#include <Arduino.h>
#include "Ps3Controller.h"

namespace State {
//constexpr char PS3_HOST_ADDRESS[] = "78:dd:08:4d:94:a4";
constexpr char PS3_HOST_ADDRESS[] = "24:6f:28:af:1c:66";
constexpr char VGA_RES[] = VGA_320x200_75Hz;
constexpr uint32_t TIME_LIMIT = 90000;

/*
 * Controlador de video que será usado en todos los juegos.
 */
fabgl::VGAController display;
fabgl::SoundGenerator sound;

/*
 * Realiza inicializaciones necesarias para poder ejecutar juegos dentro del
 * microcontrolador.
 * Inicializaciones incluidas:
 * - Habilitación de conexión VGA.
 * - Aseguramiento de conexión con el mando de PS3.
 */
void
init()
{
    Serial.printf("Iniciando display con resolución %.14s", VGA_RES);
    display.begin();
    display.setResolution(VGA_RES);
    Serial.println("¡listo!");

    Ps3.begin(PS3_HOST_ADDRESS);
    Serial.println("Esperando conección con mando");
    while (!Ps3.isConnected()) {
        Serial.print(".");
        delay(1000);
    }
    Serial.println(" ¡listo!");
    Ps3.attachOnConnect([] { Serial.println("Mando conectado."); });
    Ps3.attachOnDisconnect([] { Serial.println("Mando desconectado."); });

    Serial.println("Inicialización completa");
}

}
