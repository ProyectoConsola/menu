// #include<ESP32Time.h>
#include <Arduino.h>
#include "fabgl.h"
#include "fabutils.h"
#include <iostream>
#include <stdlib.h>
#include <Ps3Controller.h>
#include "state.hpp"
#define MAX_V  40

using fabgl::iclamp;
using std::string;

//fabgl::VGAController State::display;
fabgl::Canvas gameCanvas3(&State::display);

const int pinEncoder = 12;
unsigned int cntPulsos;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 12;

uint64_t bycicleCurrentTime;
uint64_t bycicleLastInputRecieved;
constexpr uint64_t INPUT_DELAY = 200;

// Funcion de interrupcion
void IRAM_ATTR contadorPulsos_ISR()
{
  if ((millis() - lastDebounceTime) > debounceDelay)
  {
    cntPulsos++;
    lastDebounceTime = millis();
  }
}

static constexpr uint8_t selection_arrow_bits[18] = {
        0x00, 0x40, 0x00, 0x60, 0x00, 0x70, 0x1f, 0xf8, 0x1f, 0xfc, 0x1f, 0xf8,
        0x00, 0x70, 0x00, 0x60, 0x00, 0x40
    };
Bitmap flecha_seleccion = Bitmap(16, 9, selection_arrow_bits, PixelFormat::Mask, RGB888(231, 158, 35));
Sprite arrow;
struct IntroScene : public Scene
{
  static const int TEXTROWS = 4;
  static const int TEXT_X   = 130;
  static const int TEXT_Y   = 122;
  
  int starting_ = 0;
  int selectedLevel = 0;
  uint8_t LEVEL_NUMBER = 5;
  int start_pos=35;


  IntroScene()
  : Scene(0, 20, State::display.getViewPortWidth(), State::display.getViewPortHeight())
  {
  }

  void init(){
    gameCanvas3.selectFont(&fabgl::FONT_8x13);
    gameCanvas3.setPenColor(255, 255, 255);
    gameCanvas3.setBrushColor(255, 0, 0);
    gameCanvas3.drawText(10, 10, "SELECCIONE UN NIVEL:");
    gameCanvas3.selectFont(&fabgl::FONT_8x9);
    gameCanvas3.drawText(16, 36, "Nivel 1");
    gameCanvas3.drawText(16, 66, "Nivel 2");
    gameCanvas3.drawText(16, 96, "Nivel 3");
    gameCanvas3.drawText(16, 126, "Nivel 4");
    gameCanvas3.drawText(16, 156, "Nivel 5");

    arrow.addBitmap(&flecha_seleccion);
    arrow.moveTo(1,35);
    arrow.visible=true;
    State::display.setSprites(&arrow, 1);
    
    
  }
  void update(int updateCount){

    if(starting_){
      if(starting_ > 50){
        stop();
      }
      ++starting_;
      gameCanvas3.setBrushColor(Color::Black);
      arrow.visible=false;
      gameCanvas3.scroll(0, -5);
    }

    bycicleCurrentTime = millis();
    if ((bycicleCurrentTime - bycicleLastInputRecieved) > INPUT_DELAY){
      bycicleLastInputRecieved = bycicleCurrentTime;
      if(Ps3.data.button.down){
        selectedLevel=(selectedLevel + 1) % LEVEL_NUMBER;
        arrow.y= start_pos + selectedLevel * 30;
      }
      else if(Ps3.data.button.up){
        selectedLevel = (selectedLevel != 0)? selectedLevel - 1: LEVEL_NUMBER - 1;
        arrow.y= start_pos + selectedLevel * 30;
      }
      else if(Ps3.data.button.cross){
        switch(selectedLevel){
          case 0:
          starting_=true;
              //Cantidad de pulsos
              break;
          case 1:
          starting_=true;
              break;
          case 2:
          starting_=true;
              break;
          case 3:
          starting_=true;
              break;
          case 4:
          starting_=true;
              break;
          default:
              break;
        }
        
      }
    }
    
    State::display.refreshSprites();
    
  }
  void collisionDetected(Sprite * spriteA, Sprite * spriteB, Point collisionPoint){

  }

};


const uint8_t barra[]{
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
};
Bitmap bitmapV = Bitmap(28, 30, barra, PixelFormat::Mask, RGB888(79, 245, 7));
Bitmap bitmapVL = Bitmap(28, 30, barra, PixelFormat::Mask, RGB888(185, 245, 7));
Bitmap bitmapA = Bitmap(28, 30, barra, PixelFormat::Mask, RGB888(245, 200, 7));
Bitmap bitmapN = Bitmap(28, 30, barra, PixelFormat::Mask, RGB888(255, 153, 0));
Bitmap bitmapR = Bitmap(28, 30, barra, PixelFormat::Mask, RGB888(249, 28, 28));



Sprite sprites[5];

struct SpeedOmeter : public Scene
{
  static const int TEXTROWS = 4;
  static const int TEXT_X = 130;
  static const int TEXT_Y = 122;

  float distanciaM=0, distanciaT=0;
  float distanciaPulsos=0, Vmps=0;
  unsigned int cntTiempo=0, bloqueTiempo=0;
  float MaxV = MAX_V;
  char strDistancia[20];
  char strVelocidad[20];

  SpeedOmeter()
      : Scene(0, 250, State::display.getViewPortWidth(), State::display.getViewPortHeight())
  {
  }

  void init()
  {
    gameCanvas3.setBrushColor(Color::Black);

    
    gameCanvas3.selectFont(&fabgl::FONT_6x8);
    gameCanvas3.setGlyphOptions(GlyphOptions().FillBackground(true));
    gameCanvas3.setGlyphOptions(GlyphOptions().DoubleWidth(1));
    
    
    // Agregar sprites :D >:b
    sprites[0].addBitmap(&bitmapV);
    sprites[1].addBitmap(&bitmapVL);
    sprites[2].addBitmap(&bitmapA);
    sprites[3].addBitmap(&bitmapN);
    sprites[4].addBitmap(&bitmapR);
    
    sprites[0].moveTo(260, 150);
    sprites[1].moveTo(260, 120);
    sprites[2].moveTo(260, 90);
    sprites[3].moveTo(260, 60);
    sprites[4].moveTo(260, 30);

    sprites[0].visible = true;
    sprites[1].visible = false;
    sprites[2].visible = false;
    sprites[3].visible = false;
    sprites[4].visible = false;

    State::display.setSprites(sprites, 5);
  }
  void update(int updateCount)
  {
    // Serial.println(updateCount);
    cntTiempo = millis() - bloqueTiempo;
    if (cntTiempo >= 1000)
    {
      bloqueTiempo += cntTiempo;
      distanciaM = ((cntPulsos / 20.0) * 2 * PI * 0.32);
      distanciaT += distanciaM;
      Vmps = distanciaM / (cntTiempo / 1000.0) * 3.66667; // 3.6667 es para pasar los m/s a km/h
      
      snprintf(strDistancia, 20, "%.4g", distanciaT);
      snprintf(strVelocidad, 20, "%.2g", Vmps);
      cntPulsos = 0;
    }

    gameCanvas3.clear();
    
    gameCanvas3.setPenColor(128, 128, 128);
    gameCanvas3.drawRectangle(257, 27, 290, 183);
    
    gameCanvas3.setPenColor(255,255,255);
    gameCanvas3.drawText(15, 60, "Distancia (m): ");
    gameCanvas3.drawText(15, 80, strDistancia);
    gameCanvas3.drawText(15, 100, "Velocidad (Km/h): ");
    gameCanvas3.drawText(15, 120, strVelocidad);

    if(Vmps > 0)
    {
      sprites[0].visible= true;
    }
    else
    {
      sprites[0].visible= false;
    }
    if(Vmps > MaxV/5){
      sprites[1].visible= true;
    }
    else
    {
      sprites[1].visible= false;
    }
    if(Vmps > MaxV/4){
      sprites[2].visible= true;
    }
    else
    {
      sprites[2].visible= false;
    }
    if(Vmps > MaxV/2.3){
      sprites[3].visible= true;
    }
    else
    {
      sprites[3].visible= false;
    }
    if(Vmps > MaxV/1.5){
      sprites[4].visible= true;
    }
    else
    {
      sprites[4].visible= false;
    }
    
    
  }
  void collisionDetected(Sprite *spriteA, Sprite *spriteB, Point collisionPoint)
  {
  }
};

namespace bycicle{

void  execute()
{
  pinMode(pinEncoder, INPUT);
  attachInterrupt(digitalPinToInterrupt(pinEncoder), contadorPulsos_ISR, RISING);
  IntroScene introScene;
  introScene.start();
  SpeedOmeter speedOmeter;
  speedOmeter.start();
  // mediciones en el encoder
  /*
  cntTiempo = millis() - bloqueTiempo;
  if (cntTiempo >= 1000)
  {
    bloqueTiempo += cntTiempo;

    distanciaM = ((cntPulsos / 16.0) * 2 * PI * 0.32);
    distanciaT += distanciaM;
    Vmps = distanciaM / (cntTiempo / 1000.0) * 3.66667;
    Serial.print("Cant pulsos: ");
    Serial.println(cntPulsos);
    Serial.print("Distancia pulsos: ");
    Serial.println(distanciaPulsos);
    Serial.print("Distancia (m): ");
    Serial.println(distanciaT);
    Serial.print("Velocidad (km/h): ");
    Serial.println(Vmps);

    cntPulsos = 0;
  }
  */
  delay(1);
}
}