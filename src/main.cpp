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


class MenuScene: public Scene {
private:
    const uint8_t bitmap1_data[14] = {
        0x07, 0xe0, 0x1f, 0xf8, 0x3f, 0xfc, 0x6d, 0xb6, 0xff, 0xff, 0x39,
        0x9c, 0x10, 0x08
    };
    Bitmap bitmap1;
    Sprite sprite;


public:
    MenuScene()
    : Scene(1, 20, DisplayController.getViewPortWidth(), DisplayController.getViewPortHeight())
    {
        bitmap1 = Bitmap(16, 7, bitmap1_data, PixelFormat::Mask, RGB888(255, 255, 0));
        sprite.addBitmap(&bitmap1);
        sprite.moveTo(100, 100);
        sprite.visible = true;
        DisplayController.setSprites(&sprite, 1);
    }

    void init()
    {
        canvas.selectFont(&fabgl::FONT_8x8);
        canvas.setPenColor(Color::BrightWhite);
        canvas.drawText(10, 10, "Título del juego");

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
    DisplayController.setResolution("\"300x240@60Hz\" 12.375 304 312 360 400 240 245 246 262 -HSync -VSync DoubleScan");
    Serial.println("Ready.");
}

void loop()
{
    MenuScene menu;
    menu.start();
}
