#ifndef CONSOLE_STATE_HPP
#define CONSOLE_STATE_HPP

#include <fabgl.h>

namespace State {

enum class Game {
    SPACE_INVADERS,
    PONG,
    BICYCLE
};

extern const char PS3_HOST_ADDRESS[];
extern const char VGA_RES[];
extern const uint32_t TIME_LIMIT;

extern fabgl::VGAController display;
extern fabgl::SoundGenerator sound;

void init();

}

#endif // !CONSOLE_STATE_HPP
