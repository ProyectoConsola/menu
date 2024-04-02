#include <Arduino.h>
#include "fabgl.h"
#include "fabutils.h"
#include "fonts/font_8x8.h"
#include <Ps3Controller.h>
#include "WiFiGeneric.h"

using fabgl::iclamp;

fabgl::VGAController DisplayController;
fabgl::Canvas        canvas(&DisplayController);
fabgl::PS2Controller PS2Controller;
SoundGenerator       soundGenerator;

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

class MenuScene: public Scene {
private:
    const uint8_t bitmap1_data[16] = {
        0b00010000, 0b01000000,
        0b01001000, 0b10010000,
        0b01011111, 0b11010000,
        0b01110111, 0b01110000,
        0b01111111, 0b11110000,
        0b00111111, 0b11100000,
        0b00010000, 0b01000000,
        0b00100000, 0b00100000
    };

    const uint8_t bitmap2_data[22] = {
        0x60, 0x00, 0x60, 0x00, 0x60, 0x00, 0x60, 0x00, 0x61, 0x86, 0x61, 0x86,
        0x60, 0x06, 0x00, 0x06, 0x00, 0x06, 0x00, 0x06, 0x00, 0x06
    };

    const uint8_t bitmap3_data[176] = {
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

    const uint8_t arrowbm_data[18] = {
        0x00, 0x40, 0x00, 0x60, 0x00, 0x70, 0x1f, 0xf8, 0x1f, 0xfc, 0x1f, 0xf8,
        0x00, 0x70, 0x00, 0x60, 0x00, 0x40
    };
    Bitmap bitmap1, bitmap2, bitmap3, arrowbm;
    Sprite sprites[4];


public:
    MenuScene()
    : Scene(4, 20, DisplayController.getViewPortWidth(), DisplayController.getViewPortHeight())
    {
        bitmap1 = Bitmap(12, 8, bitmap1_data, PixelFormat::Mask, RGB888(255, 255, 0));
        bitmap2 = Bitmap(16, 11, bitmap2_data, PixelFormat::Mask, RGB888(255, 255, 255));
        bitmap3 = Bitmap(16, 11, bitmap3_data, PixelFormat::RGBA2222);
        arrowbm = Bitmap(16, 9, arrowbm_data, PixelFormat::Mask, RGB888(255, 255, 255));
        sprites[0].addBitmap(&bitmap1);
        sprites[1].addBitmap(&bitmap2);
        sprites[2].addBitmap(&bitmap3);
        sprites[3].addBitmap(&arrowbm);
        sprites[0].moveTo(202, 38);
        sprites[1].moveTo(200, 57);
        sprites[2].moveTo(200, 77);
        sprites[3].moveTo(1, 35);
        sprites[0].visible = true;
        sprites[1].visible = true;
        sprites[2].visible = true;
        sprites[3].visible = true;
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
        /*canvas.fillRectangle(10, 10, 310, 20);
        canvas.drawRectangle(10, 10, 310, 20);
        canvas.setBrushColor(128, 0, 0);
        canvas.fillRectangle(10, 21, 310, 30);
        canvas.drawRectangle(10, 21, 310, 30);*/
    }

    void update(int updateCount)
    {
    }

    void collisionDetected(Sprite * spriteA, Sprite * spriteB, Point collisionPoint)
    {
    }

};

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
    MenuScene menu;
    menu.start();
}
