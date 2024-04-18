#include "displaycontroller.h"
#include "fabgl.h"
#include "fabutils.h"
#include <Ps3Controller.h>
#include <string.h>
#include "state.hpp"

const uint8_t paddle_data[] = {
  0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc,
  0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc,
  0xfc, 0xfc, 0xfc, 0xfc,
};
Bitmap bmpPaddle = Bitmap(6, 36, &paddle_data[0], PixelFormat::Mask, RGB888(255, 255, 255));

const uint8_t ball_data[] = {
  0b11110000, 0b11110000, 0b11110000, 0b11110000, 
};
Bitmap bmpBall = Bitmap(4, 4, &ball_data[0], PixelFormat::Mask, RGB888(255, 255, 255));

const uint8_t ball_data_blurry1[] = {
  0b11110000, 0b11110000, 0b11110000, 0b11110000, 
};
Bitmap bmpBall_1 = Bitmap(4, 4, &ball_data[0], PixelFormat::Mask, RGB888(170, 170, 170));

const uint8_t ball_data_blurry2[] = {
  0b11110000, 0b11110000, 0b11110000, 0b11110000, 
};
Bitmap bmpBall_2 = Bitmap(4, 4, &ball_data[0], PixelFormat::Mask, RGB888(85, 85, 85));

// Ambas barras se mueven con los dos sticks del mando
// La pelota comienza a moverse una vez se haya pulsado D-Pad abajo, o X en el
// mando

using fabgl::iclamp;

/*
 * Obtiene la posición correcta para un texto centrado en el eje X.
 */
int
centerAxisX(Canvas &canvas, const uint8_t length)
{
  return (State::display.getViewPortWidth()) -
         length * canvas.getFontInfo()->width;
}

//fabgl::VGAController State::display;
fabgl::Canvas gameCanvas2(&State::display);
SoundGenerator soundGenerator2;

constexpr unsigned long TIEMPO_LIMITE = 9000;
constexpr unsigned long TiempoInicio = 0;

// PongIntroScene
struct PongIntroScene : public Scene
{
  static const int PADDLE1_START_X = 303;
  static const int PADDLE2_START_X = 9;
  static const int PADDLE_START_Y = 81;
  static const int BALL_START_X = 157;
  static const int BALL_START_Y = 95;

  int starting_ = 0;

  SamplesGenerator *music_ = nullptr;

  PongIntroScene()
      : Scene(0, 20, State::display.getViewPortWidth(),
              State::display.getViewPortHeight()) {}

  void init()
  {
    gameCanvas2.setBrushColor(fabgl::Black);
    gameCanvas2.clear();
    gameCanvas2.setGlyphOptions(GlyphOptions().FillBackground(true));
    gameCanvas2.selectFont(&fabgl::FONT_10x20);
    gameCanvas2.setPenColor(217, 245, 255);
    gameCanvas2.setGlyphOptions(GlyphOptions().DoubleWidth(1));
    gameCanvas2.drawText(centerAxisX(gameCanvas2, 4), 20, "PONG");

    gameCanvas2.selectFont(&fabgl::FONT_8x8);
    gameCanvas2.setGlyphOptions(GlyphOptions().DoubleWidth(0));
    gameCanvas2.setPenColor(224, 158, 16);
    gameCanvas2.drawText(centerAxisX(gameCanvas2, 17), 65, "con ESP32 por FIE");
    gameCanvas2.drawText(centerAxisX(gameCanvas2, 33), 80,
                        "Facultad de Ingenieria Electrica.");

    gameCanvas2.drawBitmap(PADDLE1_START_X, PADDLE_START_Y - 20, &bmpPaddle);
    gameCanvas2.drawBitmap(PADDLE2_START_X, PADDLE_START_Y + 15, &bmpPaddle);

    gameCanvas2.drawBitmap(BALL_START_X - 60, BALL_START_Y + 55, &bmpBall_2);
    gameCanvas2.drawBitmap(BALL_START_X - 58, BALL_START_Y + 57, &bmpBall_1);
    gameCanvas2.drawBitmap(BALL_START_X - 56, BALL_START_Y + 59, &bmpBall);

    gameCanvas2.setPenColor(255, 255, 255);
    gameCanvas2.drawRectangle(17, 4, 301, 5);
    gameCanvas2.drawRectangle(17, 188, 301, 189);


  }

  void update(int updateCount)
  {
    
    if (starting_)
    {
      if (starting_ > 50){
        soundGenerator2.detach(music_);
        stop();
      }
      ++starting_;
      gameCanvas2.scroll(0, -5);
    }
    else
    {
      if (updateCount % 20 == 0)
      {
        gameCanvas2.setPenColor(255, random(255), random(255));
        gameCanvas2.drawText(centerAxisX(gameCanvas2, 27), 100,
                            "Presiona [START] para jugar");
      }

      if (updateCount > 50)
      {
        if (Ps3.event.button_down.start)
          starting_ = true;
      }
    }
  }

  void collisionDetected(Sprite *spriteA, Sprite *spriteB,
                         Point collisionPoint) {}
};

// PongGameScene
struct PongGameScene : public Scene
{

  enum SpriteType
  {
    TYPE_PADDLE1,
    TYPE_PADDLE2,
    TYPE_BALL,
  };

  enum GameState
  {
    GAMESTATE_PLAYING,
    GAMESTATE_PLAYERSCORE,
    GAMESTATE_GAMEOVER,
  };

  GameState gameState_ = GAMESTATE_PLAYING;


  struct SISprite : Sprite
  {
    SpriteType type;
  };

  static const int PADDLECOUNT = 1;
  static const int BALLCOUNT = 1;
  static const int SPRITESCOUNT = 2 * PADDLECOUNT + BALLCOUNT;

  static const int BALL_START_X = 157;
  static const int BALL_START_Y = 95;
  static const int PADDLE1_START_X = 303;
  static const int PADDLE2_START_X = 9;
  static const int PADDLE_START_Y = 81;
  static const int POINT_START_X = 158;
  static const int POINT_END_X = 159;
  int POINT_START_Y = 8;
  int POINT_END_Y = 9;

  static int scoreP1_;
  static int scoreP2_;

  SISprite *sprites_ = new SISprite[SPRITESCOUNT];
  SISprite *player1_ = sprites_;
  SISprite *player2_ = player1_ + 1;
  SISprite *ball_ = player2_ + 1;

  int player1VelY_ = 0;
  int player2VelY_ = 0;
  
  int ballVelX = 0;
  int ballVelY = 0;
  
  bool updateScore_ = true;
  bool scored_ = false;
  bool reseted_ = true;
  bool impulsed_ = false;
  bool collided_ = false;
  
  PongGameScene()
      : Scene(3, 20, State::display.getViewPortWidth(), State::display.getViewPortHeight())
  {
  }

  ~PongGameScene()
  {
    delete[] sprites_;
  }

  void init()
  {
    // setup player 1
    player1_->addBitmap(&bmpPaddle);
    player1_->moveTo(PADDLE1_START_X, PADDLE_START_Y);
    player1_->type = TYPE_PADDLE1;
    addSprite(player1_);

    // setup player 2
    player2_->addBitmap(&bmpPaddle);
    player2_->moveTo(PADDLE2_START_X, PADDLE_START_Y);
    player2_->type = TYPE_PADDLE2;
    addSprite(player2_);

    // setup ball
    ball_->addBitmap(&bmpBall);
    ball_->moveTo(BALL_START_X, BALL_START_Y);
    ball_->type = TYPE_BALL;
    addSprite(ball_);
    
    State::display.setSprites(sprites_, 3);

    gameCanvas2.setBrushColor(0, 0, 0);
    gameCanvas2.clear();

    gameCanvas2.setPenColor(255, 255, 255);
    gameCanvas2.drawRectangle(17, 4, 301, 5);
    gameCanvas2.drawRectangle(17, 188, 301, 189);
    for(int i = 1; i <= 46; i++){
      gameCanvas2.drawRectangle(POINT_START_X, POINT_START_Y, POINT_END_X, POINT_END_Y);
      POINT_START_Y += 4;
      POINT_END_Y += 4;
    }
    gameCanvas2.setGlyphOptions(GlyphOptions().FillBackground(true));
    gameCanvas2.selectFont(&fabgl::FONT_8x16);
  }

  void moveBall(int rn){
    if((ball_->y < 7 || ball_->y > 184) && !collided_){
      ballVelY = -ballVelY;
      collided_ = true;
      impulsed_ = false;
    }
    if (!impulsed_ && rn % 2){
      if(ballVelY > 3){
        ballVelY--;
      }
      else if (ballVelY < -3){
        ballVelY++;
      }
      else if(ballVelX > 3){
        ballVelX--;
      }
      else if (ballVelX < -3){
        ballVelX++;
      }
    }
    else{
      collided_ = false;
    }
    ball_->x += ballVelX;
    ball_->y += ballVelY;
    updateSpriteAndDetectCollisions(ball_);
  }

  void movePlayer(){
    if (Ps3.data.analog.stick.ry < -80){
      player1VelY_ = -2;
    }
    else if (Ps3.data.analog.stick.ry > 80){
      player1VelY_ = +2;
    }
    else{
      player1VelY_ = 0;
    }
    if (Ps3.data.analog.stick.ly < -80){
      player2VelY_ = -2;
    }
    else if (Ps3.data.analog.stick.ly > 80){
      player2VelY_ = +2;
    }
    else{
      player2VelY_ = 0;
    }
  }

  void drawScore()
  {
    gameCanvas2.setPenColor(255, 255, 255);
    gameCanvas2.drawTextFmt(130, 10, "%02d" , scoreP2_);
    gameCanvas2.drawTextFmt(172, 10, "%02d" ,scoreP1_);
  }

  void scoreGoal(){
    if (ball_->x > 308 && !scored_){
      scoreP2_++;
      updateScore_ = true;
      scored_ = true;
    }
    else if (ball_->x < 10 && !scored_){
      scoreP1_++;
      updateScore_ = true;
      scored_ = true;
    }
  }

  void resetBall(){
    ballVelX = 0;
    ballVelY = 0;
    ball_->moveTo(BALL_START_X, BALL_START_Y);
    reseted_ = true;
  }

  void startBall(int rn){
    int random = rand() % 100;
    if (rn % 2){
      ballVelX = 2;
    }
    else {
      ballVelX = -2;
    }
    if (random % 2){
      ballVelY = -2;
    }
    else{
      ballVelY = 2;
    }
    reseted_ = false;
  }

  void update(int updateCount)
  {
    if(updateScore_){
      drawScore();
      updateScore_ = false;
    }
    if (scored_){
      resetBall();
      scored_ = false;
    }
    if (reseted_ && (Ps3.event.analog_changed.button.cross || Ps3.event.analog_changed.button.down)){
      startBall(updateCount);
    }
    movePlayer();
    moveBall(updateCount);
    scoreGoal();
    player1_->y += player1VelY_;
    player1_->y= iclamp(player1_->y, 0, getHeight() - player1_->getHeight());
    updateSpriteAndDetectCollisions(player1_);
    player2_->y += player2VelY_;
    player2_->y= iclamp(player2_->y, 0, getHeight() - player2_->getHeight());
    updateSpriteAndDetectCollisions(player2_);

    State::display.refreshSprites();
  }

  void collisionDetected(Sprite *spriteA, Sprite *spriteB, Point collisionPoint)
  {
    SISprite *sA = (SISprite *)spriteA;
    SISprite *sB = (SISprite *)spriteB;

    if (sA->type == TYPE_PADDLE1 && sB->type == TYPE_BALL){
      ballVelX = -ballVelX;
      if (ballVelX >= -4)
        ballVelX--;
      if (player1VelY_){
        ballVelY += player1VelY_;
      }
      else{
        ballVelY/=2;
      }
      impulsed_ = true;
    }

    if (sA->type == TYPE_PADDLE2 && sB->type == TYPE_BALL){
      ballVelX = -ballVelX;
      if (ballVelX <= 4)
        ballVelX++;
      if (player2VelY_){
        ballVelY += player2VelY_;
      }
      else{
        ballVelY/=2;
      }
      impulsed_ = true;
    }
  }
};

int PongGameScene::scoreP1_ = 0;
int PongGameScene::scoreP2_ = 0;

namespace pong {

void execute()
{
  Serial.println("Pong is executing");
  PongIntroScene pongIntroScene;
  pongIntroScene.start();
  PongGameScene pongGameScene;
  pongGameScene.start();
}
}
