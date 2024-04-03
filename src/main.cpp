#include <Arduino.h>
#include "HardwareSerial.h"
#include "fabgl.h"
#include "fabutils.h"
#include <Ps3Controller.h>
#include <sys/_stdint.h>


fabgl::VGAController DisplayController;
fabgl::Canvas        canvas(&DisplayController);
fabgl::PS2Controller PS2Controller;
SoundGenerator       soundGenerator;

namespace Menu {

/*
 * Clase representante de la escena del menú principal.
 * 
 */
class MainScene: public Scene {
private:
    static constexpr uint8_t space_invaders_bits[16] = {
        0b00010000, 0b01000000,
        0b01001000, 0b10010000,
        0b01011111, 0b11010000,
        0b01110111, 0b01110000,
        0b01111111, 0b11110000,
        0b00111111, 0b11100000,
        0b00010000, 0b01000000,
        0b00100000, 0b00100000
    };
    static constexpr uint8_t pong_bits[22] = {
        0x60, 0x00, 0x60, 0x00, 0x60, 0x00, 0x60, 0x00, 0x61, 0x86, 0x61, 0x86,
        0x60, 0x06, 0x00, 0x06, 0x00, 0x06, 0x00, 0x06, 0x00, 0x06
    };
    static constexpr uint8_t bicycle_bits[176] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf4, 0xf4, 0xd5, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xea, 0xf4, 0xf4, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf4, 0xf4, 0xea, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xd1, 0xd1, 0x00, 0x00, 0xf4, 0xf4, 0x00, 0xea, 0xea, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0xd1, 0xd1, 0xd1, 0xd1, 0x00, 0xd5, 0x00, 0xea, 0x00, 0xea, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0xea, 0xea, 0x00, 0x00, 0x00, 0xea, 0x00, 0xea, 0xea, 0x00, 0x00,
        0x00, 0x00, 0xd5, 0xd5, 0xd5, 0x00, 0x00, 0xea, 0x00, 0xea, 0x00, 0xd5, 0xd5, 0xea, 0x00, 0x00,
        0x00, 0xd5, 0xd5, 0x00, 0xd5, 0xd5, 0x00, 0xea, 0xea, 0x00, 0xd5, 0xd5, 0x00, 0xd5, 0xd5, 0x00,
        0x00, 0xd5, 0x00, 0x00, 0x00, 0xd5, 0xea, 0xea, 0x00, 0xea, 0xd5, 0x00, 0x00, 0x00, 0xd5, 0x00,
        0x00, 0xd5, 0xd5, 0x00, 0xd5, 0xd5, 0x00, 0x00, 0x00, 0x00, 0xd5, 0xd5, 0x00, 0xd5, 0xd5, 0x00,
        0x00, 0x00, 0xd5, 0xd5, 0xd5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd5, 0xd5, 0xd5, 0x00, 0x00,
    };
    static constexpr uint8_t selection_arrow_bits[18] = {
        0x00, 0x40, 0x00, 0x60, 0x00, 0x70, 0x1f, 0xf8, 0x1f, 0xfc, 0x1f, 0xf8,
        0x00, 0x70, 0x00, 0x60, 0x00, 0x40
    };
    Bitmap space_invaders_bmp, pong_bmp, bicycle_bmp, selection_arrow_bmp;
    Sprite sprites[4];


public:
    MainScene():
        Scene(4, 20, DisplayController.getViewPortWidth(), DisplayController.getViewPortHeight()),
        space_invaders_bmp {Bitmap(12, 8, space_invaders_bits, PixelFormat::Mask, RGB888(255, 255, 0))},
        pong_bmp {Bitmap(16, 11, pong_bits, PixelFormat::Mask, RGB888(255, 255, 255))}, 
        bicycle_bmp {Bitmap(16, 11, bicycle_bits, PixelFormat::RGBA2222)},
        selection_arrow_bmp {Bitmap(16, 9, selection_arrow_bits, PixelFormat::Mask, RGB888(255, 255, 255))}
    {
        sprites[0].addBitmap(&space_invaders_bmp);
        sprites[1].addBitmap(&pong_bmp);
        sprites[2].addBitmap(&bicycle_bmp);
        sprites[3].addBitmap(&selection_arrow_bmp);
        sprites[0].moveTo(202, 38);
        sprites[1].moveTo(200, 57);
        sprites[2].moveTo(200, 77);
        sprites[3].moveTo(1, 35);
        sprites[0].visible = sprites[1].visible = sprites[2].visible = sprites[3].visible = true;
        DisplayController.setSprites(sprites, 4);
    }

    void init()
    {
        canvas.selectFont(&fabgl::FONT_8x13);
        canvas.setPenColor(255, 255, 255);
        canvas.setBrushColor(255, 0, 0);
        canvas.drawText(10, 10, "Seleccione el Juego:");
        canvas.selectFont(&fabgl::FONT_8x9);
        canvas.drawText(16, 36, "Space Invaders");
        canvas.drawText(16, 56, "Pong!");
        canvas.drawText(16, 76, "Bicicleta");
    }

    void update(int updateCount) { }

    void collisionDetected(Sprite * spriteA, Sprite * spriteB, Point collisionPoint) { }
};

/*
 * Declaraciones redundantes necesarias en C++ 2011.
 * Ver: https://stackoverflow.com/questions/8016780/undefined-reference-to-static-constexpr-char
 */
constexpr uint8_t MainScene::space_invaders_bits[];
constexpr uint8_t MainScene::pong_bits[];
constexpr uint8_t MainScene::bicycle_bits[];
constexpr uint8_t MainScene::selection_arrow_bits[];

}

/*
void drawScaleBitmap(int16_t x, int16_t y, Bitmap &b, uint8_t scale) {
    switch (b.format) {
    case PixelFormat::Undefined:
    case PixelFormat::Native:
        break;
    case PixelFormat::Mask:
        break;
    case PixelFormat::RGBA2222:
        break;
    case PixelFormat::RGBA8888:
        break;
    }
    for (int16_t i = 0; i < b.height*scale; i += 1) {
        for (int16_t j = 0; j < b.width*scale; j += 1) {
            DisplayController.setRawPixel(x + i, y + j, b.data[]);
        }
    }
    if (b.format == PixelFormat::Mask){
        Serial.println("idk");
    }
    for (int16_t i = 0; i < (b.height*b.width); i++){
        DisplayController.setRawPixel(x + i % b.width);
    }
}
*/



void setup()
{
    Serial.begin(115200);

    Ps3.begin("24:6f:28:af:1c:66");
    DisplayController.begin();
    DisplayController.setResolution(VGA_320x200_75Hz);
    Serial.println("Ready.");
}

void loop()
{
    Menu::MainScene menu;
    menu.start();
}
