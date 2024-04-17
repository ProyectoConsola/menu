#include <fabgl.h>
#include <Ps3Controller.h>
#include "state.hpp"

fabgl::Canvas canvas(&State::display);

struct Vec2 {
    uint16_t x;
    uint16_t y;
};

constexpr uint8_t GAMES_NUMBER = 3;
constexpr uint64_t INPUT_DELAY = 100;
constexpr uint16_t SELECT_ARROW_STEP = 20;
constexpr Vec2 SELECT_ARROW_START_POS { 1, 35 };

State::Game selectedGame;

/*
 * Información que debería ir en clase juego.
 */
uint64_t lastInputRecieved;
uint64_t currentTime;

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
        Scene(4, 20, State::display.getViewPortWidth(), State::display.getViewPortHeight()),
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
        State::display.setSprites(sprites, 4);
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

    void update(int updateCount) 
    {
        static uint8_t currentSelection = 0;
        static bool downPressed = false, upPressed = false, crossPressed = false;
        currentTime = millis();
        if ((currentTime - lastInputRecieved) > INPUT_DELAY) {
            downPressed = Ps3.data.button.down;
            upPressed = Ps3.data.button.up;
            crossPressed = Ps3.data.button.cross;

            lastInputRecieved = currentTime;
            if (downPressed || upPressed) {
                if (downPressed) {
                    currentSelection = (currentSelection + 1) % GAMES_NUMBER;
                }
                if (upPressed) {
                    currentSelection = (currentSelection != 0)? currentSelection - 1: GAMES_NUMBER - 1;
                }
                sprites[3].y = SELECT_ARROW_START_POS.y + currentSelection * SELECT_ARROW_STEP;
                State::display.refreshSprites();
            }
            else if (crossPressed) {
                switch (currentSelection)
                {
                case 0:
                    selectedGame = State::Game::SPACE_INVADERS;
                    Serial.println("Space Invaders seleccionado");
                    break;
                case 1:
                    selectedGame = State::Game::PONG;
                    Serial.println("Selected Pong!");
                    break;
                case 2:
                    selectedGame = State::Game::BICYCLE;
                    Serial.println("Selected Bicicleta");
                    break;
                default:
                    Serial.println("Selección no válida");
                    break;
                }
            }
        }
    }

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

namespace menu {



State::Game execute()
{
    MainScene menu;
    menu.start();
    return selectedGame;
}

}