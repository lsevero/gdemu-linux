#include <Keypad.h>
#include <SPI.h>
#include <GD.h>
#include <stdlib.h>
#include <inttypes.h>

#include "mario_platformer.h"
#include "mario_sprites.h"
//#include "mario_som.h"

#define boolean bool
#define byte char

// ----------------------> Controles do Arduino <----------------------

#define P1_LEFT   bitMap[0]&1 
#define P1_DOWN   bitMap[0]&2 
#define P1_UP     bitMap[0]&4 
#define P1_RIGHT  bitMap[0]&8 
 
#define P2_LEFT   bitMap[1]&1 
#define P2_DOWN   bitMap[1]&2 
#define P2_UP     bitMap[1]&4 
#define P2_RIGHT  bitMap[1]&8 
  
#define P1_B4     bitMap[2]&1 
#define P1_B1     bitMap[2]&2 
#define P1_B3     bitMap[2]&4 
#define P1_B2     bitMap[2]&8 
  
#define P2_B4     bitMap[3]&1 
#define P2_B1     bitMap[3]&2 
#define P2_B3     bitMap[3]&4 
#define P2_B2     bitMap[3]&8 

//PINOS NO ARDUINO
#define ROW0 0 //FIO VERDE
#define ROW1 1 //FIO VERMELHO
#define ROW2 2 //FIO ROXO
#define ROW3 3 //FIO OU MARRINZA

#define COL0 4 //FIO VERDE - JOYSTICK LEFT
#define COL1 5 //FIO AMARELO - JOYSTICK UP
#define COL2 6 //FIO PRETO - JOYSTICK DOWN
#define COL3 7 // FIO LARANJA - JOYSTICK RIGHT

long lastAnimTime = 0;
int animTime = 250;

char previousPressedKey;
boolean hasReleasedKey = false;

const byte ROWS = 4 ; // Four rows
const byte COLS = 4; // Three columns

byte rowPins[ROWS] = {ROW0,ROW1,ROW2, ROW3};
byte colPins[COLS] = {COL0, COL1, COL2, COL3};

char keys[ROWS][COLS]={
  {'a','w','s','d'},//setas wasd
  {'h','k','j','l'},//setas VIM
  {'1','2','3','4'},
  {'5','6','7','8'}
};

Keypad player = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

boolean avancando = true; // VARIAVEL A SER PASSADA PARA BAIXO!

int atxy(int x, int y)
{
  return (y << 6) + x;
}

// --------> Colisao <---------

void readn(byte *dst, unsigned int addr, int c)
{
  GD.__start(addr);
  while (c--) {
    *dst++ = SPI.transfer(0);
  }
  GD.__end();
}

static byte coll[256];
static void load_coll() {
  //while (GD.rd(VBLANK) == 0);  // Wait until vblank
  //while (GD.rd(VBLANK) == 1);  // Wait until display
  //while (GD.rd(VBLANK) == 0);  // Wait until vblank
  readn(coll, COLLISION, sizeof(coll));
}

byte spr;

// copy a (w,h) rectangle from the source image (x,y) into picture RAM
static void rect(unsigned int dst, byte x, byte y, byte w, byte h)
{
  uint8_t *src = platformer_pic + (16 * y) + x;
  while (h--) {
    GD.copy(dst, src, w);
    dst += 64;
    src += 16;
  }
}

#define SINGLE(x, y) (pgm_read_byte_near(&platformer_pic[(y) * 16 + (x)]))

// Draw a random 8-character wide background column at picture RAM dst
boolean pt1 = false;
boolean pt2 = false;

void draw_column(unsigned int dst)
{
  byte y;
  byte x;
  byte ch;

  // Clouds and sky, 11 lines
  rect(dst, 0, 0, 8, 11);

  // bottom plain sky, lines 11-28
  ch = SINGLE(0, 11);
  for (y = 11; y < 28; y++)
    GD.fill(dst + (y << 6), ch, 8);


  // randomly choose between background elements
  byte what = random(256);

  if (pt1) {
    rect(dst + atxy(0, 19), 8, 23, 8, 9);
    pt1 = false;

  }
  else if (pt2) {
    rect(dst + atxy(0, 19), 0, 23, 8, 9);
    pt2 = false;
  }
  else if (what < 10) {
    // big mushroom thing
    if (!avancando) {
      rect(dst + atxy(0, 19), 8, 23, 8, 9);
      pt2 = true;
    }
    else {
      rect(dst + atxy(0, 19), 0, 23, 8, 9);
      pt1 = true;
    }

    //y += 9;
    //byte i;
    //while (y < 28) {
    //  rect(dst + atxy(0, y), 8, 23 + (i & 3), 8, 1);
    //  i++, y++;
    //}
  }
  else if (what < 32) {
    // pair of green bollards
    for (x = 0; x < 8; x += 4) {
      y = random(20, 25);
      rect(dst + atxy(x, y), 6, 11, 4, 3);
      y += 3;
      while (y < 28) {
        rect(dst + atxy(x, y), 6, 13, 4, 1);
        y++;
      }
    }
  }
  else {
    // hills
    for (x = 0; x < 8; x += 2) {
      y = random(20, 25);
      rect(dst + atxy(x, y), 4, 11, 2, 3);
      y += 3;
      while (y < 28) {
        rect(dst + atxy(x, y), 4, 13, 2, 1);
        y++;
      }
    }
    // foreground blocks
    x = random(5);
    y = random(11, 24);
    byte blk = random(4);
    rect(dst + atxy(x, y), blk * 4, 14, 4, 3);
    y += 3;
    while (y < 28) {
      rect(dst + atxy(x, y), blk * 4, 17, 4, 1);
      y++;
    }
  }

  // Ground, line 28. (essa é a linha fina do chão. Converter para sprite.)
  ch = SINGLE(0, 18);
  GD.fill(dst + atxy(0, 28), ch, 8); //(preenchimento natural de background)


                     // Underground, line 29 (abaixo da linha do chão.) **Preenchido
  ch = SINGLE(0, 19);
  for (int pr = 29; pr < 38; pr++) { //pr corresponde as linhas que serão preenchidas. Da 29 a 38
    GD.fill(dst + atxy(0, pr), ch, 8);
  }

}

// -----------------------------> Variáveis Globais <-----------------------------

// -----------------------------> Scroll <-----------------------------
unsigned long xscroll;
long tscroll = 0;

// -----------------------------> Animacao <-----------------------------
boolean m_animando = false;
int mario_estagio = 0;
int mario_orientacao = 0;
boolean l_animando = false;
int luigi_estagio = 0;
int luigi_orientacao = 0;
int anim_delay = 0;

// -----------------------------> Objetos <-----------------------------

int xmario = 176;
int ymario = 208;

int xluigi = 190;
int yluigi = 208;

boolean provas = false;
boolean dprovas = false;
int xprovas = 0;
int yprovas = 0;
unsigned nprovas;
long ultprovas = 0;

int xt1 = 300;
int xt2 = 200;
int xt3 = 100;
int xt4;
int turtcont = 0;
int t_mario_estagio = 23;
int t_anim_delay = 0;
int t_mario_estagio2 = 21;
int turtcont2;
boolean t1 = true;
boolean t2 = true;
boolean t3 = true;
boolean t4 = true;

int nabocont=0;
int tNabo1, tNabo3;
int tNabo2;

// -----------------------------> Pulo <-----------------------------

boolean mario_pulando = false;
unsigned long t0_pulo_m;
unsigned long y0_pulo_m;
boolean luigi_pulando = false;
unsigned long t0_pulo_l;
unsigned long y0_pulo_l;
int gravidade = 320;
int y_delay;
boolean caindo;
int cont_pulo;

// -----------------------------> Vidas <-----------------------------

int vida_mario = 4;
int vida_luigi = 4;
boolean m_invencivel = false;
int m_t_invencivel = 0;
boolean l_invencivel = false;
int l_t_invencivel = 0;

void setup()
{
  GD.begin();
  GD.copy(RAM_CHR, platformer_chr, sizeof(platformer_chr));
  GD.copy(RAM_PAL, platformer_pal, sizeof(platformer_pal));
  GD.copy(PALETTE16A, sprites_sprpal, sizeof(sprites_sprpal));
  GD.uncompress(RAM_SPRIMG, sprites_sprimg);
  GD.wr(JK_MODE, 1); // Ativando colisao JK

  // paleta(); // Configurando a paleta de cores

  int i;
  for (i = 0; i < 256; i++)
    GD.sprite(i, 400, 400, 0, 0, 0);

  for (i = 0; i < 64; i += 8) {
    draw_column(atxy(i, 0));
  }

  boolean inicio = true;

  // Serial.begin(1000000);
}

void loop() {

  char key = player.getKey(); 
  

  // -----------------------------> Animacao (delay) <-----------------------------

  anim_delay++;
  if (anim_delay == 5) {
    anim_delay = 0;
  }
  y_delay++;
  if (y_delay == 3) {
    y_delay = 0;
  }

  // -----------------------------> Scroll / Movimentação horizontal <-----------------------------

  if ((tscroll - millis()) > 1200) {
    tscroll = millis();
    xscroll++;
    if (xmario > 8) {
      xmario--;
    }
    if (xluigi > 8) {
      xluigi--;
    }
    xt1--;
    xt2--;
    xt3--;
    xt4--;
  }
  if (player.P1_RIGHT) {
    m_animando = true;
    mario_orientacao = 0;
    if (xmario < 392) {
      xmario += 2;
    }
  }
  if (player.P1_LEFT) {
    m_animando = true;
    mario_orientacao = 2;
    if (avancando) {
      if (xmario >= 8) {
        xmario--;
      }
    }
  }
  if (player.P2_RIGHT) {
    l_animando = true;
    luigi_orientacao = 0;
    if (xluigi < 392) {
      xluigi += 2;
    }
  }
  if (player.P2_LEFT) {
    l_animando = true;
    luigi_orientacao = 2;
    if (xluigi >= 8) {
      xluigi--;
    }
  }
  /// -----------------------------> Fim scroll <-----------------------------

  // -----------------------------> Pulo <-----------------------------
  // Mario
  if ((player.P1_B1) && !mario_pulando) {
    mario_pulando = true;
    t0_pulo_m = millis();
    y0_pulo_m = ymario;
  }
  if (mario_pulando) {
    float tempo = millis() - t0_pulo_m;
    float velocidade = 260 - 320 * tempo;
    if (velocidade < 1) {
      gravidade = 520;
    }
    ymario = round(y0_pulo_m - (260 * tempo / 1000) + ((gravidade / 2)  * (tempo / 1000) * (tempo / 1000)));
    m_animando = false;
  }
  if (ymario > 208) {
    ymario = 208;
    mario_pulando = false;
    gravidade = 320;
  }
  // Luigi
  if ((player.P2_B1) && !luigi_pulando) {
    luigi_pulando = true;
    t0_pulo_l = millis();
    y0_pulo_l = yluigi;
  }
  if (luigi_pulando) {
    float tempo = millis() - t0_pulo_l;
    float velocidade = 260 - 320 * tempo;
    if (velocidade < 1) {
      gravidade = 520;
    }
    yluigi = round(y0_pulo_l - (260 * tempo / 1000) + ((gravidade / 2)  * (tempo / 1000) * (tempo / 1000)));
    l_animando = false;
  }
  if (yluigi > 208) {
    yluigi = 208;
    luigi_pulando = false;
    gravidade = 320;
  }

  // -----------------------------> Animacao <-----------------------------
  // Mario
  if (!(player.P1_RIGHT) && !(player.P1_LEFT)) {
    m_animando = false;
  }
  if (m_animando) {
    if (anim_delay == 1) {
      mario_estagio++;
      if (mario_estagio > 3) {
        mario_estagio = 1;
      }
    }
  }
  else {
    if (mario_pulando) {
      mario_estagio = 5;
    }
    else {
      mario_estagio = 0;
    }
  }
  if (t_anim_delay == 20) {
    if (t_mario_estagio == 23) {
      t_mario_estagio = 24;
    }
    else {
      t_mario_estagio = 23;
    }
    t_anim_delay = 0;
  }

  if (!(player.P2_RIGHT) && !(player.P2_LEFT)) {
    l_animando = false;
  }
  if (l_animando) {
    if (anim_delay == 1) {
      luigi_estagio++;
      if (luigi_estagio > 3) {
        luigi_estagio = 1;
      }
    }
  }
  else {
    if (luigi_pulando) {
      luigi_estagio = 5;
    }
    else {
      luigi_estagio = 0;
    }
  }
  if (t_anim_delay == 20) {
    if (luigi_estagio == 23) {
      luigi_estagio = 24;
    }
    else {
      luigi_estagio = 23;
    }
    t_anim_delay = 0;
  }

  // -----------------------------> Objetos <-----------------------------

  // Sprite dos Jogadores

  GD.__wstartspr(252);
  draw_sprites(xmario, ymario, mario_estagio + (m_invencivel? 27 : 0), mario_orientacao, 1);
  draw_sprites(xluigi, yluigi, luigi_estagio + 6 + (l_invencivel ? 27 : 0), luigi_orientacao, 1);
  GD.__end();

  tNabo1 = millis();
  
  //Sprite da tartaruga
  GD.__wstartspr(0);

  if (t1) {
    draw_sprites(xt1, 216, t_mario_estagio, 0);
  }
  if (t2) {
    draw_sprites(xt2, 216, t_mario_estagio, 0);
  }
  if (t3) {
    draw_sprites(xt3, 216, t_mario_estagio, 0);
  }
  if (t4) {
    draw_sprites(xt4, 216, t_mario_estagio2, 0);
  }
  
  turtcont++;
  turtcont2++;
  t_anim_delay++;
  xt4-=2;

  if(turtcont2 == 30){
    if(t_mario_estagio2 == 21){
      t_mario_estagio2 = 22;
      
    }else{
      t_mario_estagio2 = 21;
    }
    turtcont2 = 0;
  }

  if (turtcont == 2) {
    xt1--;
    xt2--;
    xt3--;
    turtcont = 0;
  }

  GD.__end();

  if (xscroll - ultprovas >= 400) {
    provas = true;
    if (true) { //rand() % 2 == 0
      dprovas = true;
    } else {
      dprovas = false;
      nprovas = rand() % 4;
    }
    ultprovas = xscroll;
  }
  if (provas) {
    if (dprovas) {
      if (yprovas < 320) {
        yprovas += 2;
        GD.__wstartspr(10);
        draw_sprites(16, yprovas, 25, 6, 0);
        draw_sprites(80, yprovas, 25, 6, 0);
        draw_sprites(144, yprovas, 25, 6, 0);
        draw_sprites(208, yprovas, 25, 6, 0);
        draw_sprites(272, yprovas, 25, 6, 0);
        draw_sprites(336, yprovas, 25, 6, 0);
        draw_sprites(400, yprovas, 25, 6, 0);
        GD.__end();
      } else {
        yprovas = 0;
        provas = false;
      }
    } else {
      if (xprovas >= 0) {
        xprovas--;
        GD.__wstartspr(10);
        switch (nprovas) {
        case(1):
          draw_sprites(xprovas, 216, 25, 3);
          break;
        case(2):
          draw_sprites(xprovas, 216, 25, 3);
          draw_sprites(xprovas, 200, 25, 3);
          break;
        case(3):
          draw_sprites(xprovas, 216, 25, 3);
          draw_sprites(xprovas, 200, 25, 3);
          draw_sprites(xprovas, 184, 25, 3);
          break;
        }
        GD.__end();
      } else {
        xprovas = 0;
        provas = false;
      }
    }
  }
  
  nabocont++;

  // -----------------------------> Colisao <-----------------------------
  load_coll();

  if (millis() - m_t_invencivel > 2000) {
    m_invencivel = false;
  }

  if (coll[252] != 0xff) { // Parte de cima colidindo
    int col = coll[252];
    if (col < 10 && !m_invencivel) {
      vida_mario--;
      m_invencivel = true;
      m_t_invencivel = millis();
    } else if (col < 30 && !m_invencivel) {
      vida_mario--;
      m_invencivel = true;
      m_t_invencivel = millis();
    }
  }
  if (coll[253] != 0xff) { // Parte de baixo colidindo
    int col = coll[253];
    if (col < 10) {
      if (216 >= (ymario + 26)) { // mario_pulando em cima
        mario_pulando = true;
        t0_pulo_m = millis();
        y0_pulo_m = ymario;
      } else if (!m_invencivel) {
        vida_mario--;
        m_invencivel = true;
        m_t_invencivel = millis();
      }
    } else if (col < 30 && !m_invencivel) {
      vida_mario--;
      m_invencivel = true;
      m_t_invencivel = millis();
    }
  }

  // Luigi

  if (millis() - l_t_invencivel > 2000) {
    l_invencivel = false;
  }

  if (coll[254] != 0xff) { // Parte de cima colidindo
    int col = coll[254];
    if (col < 10 && !l_invencivel) {
      vida_luigi--;
      l_invencivel = true;
      l_t_invencivel = millis();
    }
    else if (col < 30 && !l_invencivel) {
      vida_luigi--;
      l_invencivel = true;
      l_t_invencivel = millis();
    }
  }
  if (coll[255] != 0xff) { // Parte de baixo colidindo
    int col = coll[255];
    if (col < 10) {
      if (216 >= (yluigi + 26)) { // luigi_pulando em cima
        luigi_pulando = true;
        t0_pulo_l = millis();
        y0_pulo_l = yluigi;
      }
      else if (!l_invencivel) {
        vida_luigi--;
        l_invencivel = true;
        l_t_invencivel = millis();
      }
    }
    else if (col < 30 && !l_invencivel) {
      vida_luigi--;
      l_invencivel = true;
      l_t_invencivel = millis();
    }
  }

  // -----------------------------> Vidas <-----------------------------

  GD.__wstartspr(100);
  switch (vida_mario) {
  case 4:
    draw_sprites(30, 40, 26, 0);
    draw_sprites(46, 40, 26, 0);
    draw_sprites(62, 40, 26, 0);
    draw_sprites(78, 40, 26, 0);
    draw_sprites(0, 0, 39, 0);
    draw_sprites(0, 0, 40, 0);
    draw_sprites(0, 0, 41, 0);
    break;
  case 3:
    draw_sprites(30, 40, 26, 0);
    draw_sprites(46, 40, 26, 0);
    draw_sprites(62, 40, 26, 0);
    draw_sprites(0, 40, 26, 0);
    draw_sprites(0, 0, 39, 0);
    draw_sprites(0, 0, 40, 0);
    draw_sprites(0, 0, 41, 0);
    break;
  case 2:
    draw_sprites(30, 40, 26, 0);
    draw_sprites(46, 40, 26, 0);
    draw_sprites(0, 40, 26, 0);
    draw_sprites(0, 40, 26, 0);
    draw_sprites(0, 0, 39, 0);
    draw_sprites(0, 0, 40, 0);
    draw_sprites(0, 0, 41, 0);
    break;
  case 1:
    draw_sprites(30, 40, 26, 0);
    draw_sprites(0, 40, 26, 0);
    draw_sprites(0, 40, 26, 0);
    draw_sprites(0, 40, 26, 0);
    draw_sprites(0, 0, 39, 0);
    draw_sprites(0, 0, 40, 0);
    draw_sprites(0, 0, 41, 0);
    break;
  case 0:
    draw_sprites(0, 0, 26, 0);
    draw_sprites(0, 0, 26, 0);
    draw_sprites(0, 0, 26, 0);
    draw_sprites(0, 0, 26, 0);
    draw_sprites(xluigi - 16, yluigi - 24, 39, 0);
    draw_sprites(xluigi, yluigi - 24, 40, 0);
    draw_sprites(xluigi + 16, yluigi - 24, 41, 0);
    delay(5000);
    vida_mario = 4;
    vida_luigi = 4;
    xmario = 176;
    ymario = 208;
    xluigi = 190;
    yluigi = 208;
    setup();
  }
  switch (vida_luigi) {
  case 4:
    draw_sprites(354, 40, 26, 0);
    draw_sprites(338, 40, 26, 0);
    draw_sprites(322, 40, 26, 0);
    draw_sprites(306, 40, 26, 0);
    draw_sprites(0, 0, 39, 0);
    draw_sprites(0, 0, 40, 0);
    draw_sprites(0, 0, 41, 0);
    break;
  case 3:
    draw_sprites(354, 40, 26, 0);
    draw_sprites(338, 40, 26, 0);
    draw_sprites(322, 40, 26, 0);
    draw_sprites(0, 40, 26, 0);
    draw_sprites(0, 0, 39, 0);
    draw_sprites(0, 0, 40, 0);
    draw_sprites(0, 0, 41, 0);
    break;
  case 2:
    draw_sprites(354, 40, 26, 0);
    draw_sprites(338, 40, 26, 0);
    draw_sprites(0, 40, 26, 0);
    draw_sprites(0, 40, 26, 0);
    draw_sprites(0, 0, 39, 0);
    draw_sprites(0, 0, 40, 0);
    draw_sprites(0, 0, 41, 0);
    break;
  case 1:
    draw_sprites(354, 40, 26, 0);
    draw_sprites(0, 40, 26, 0);
    draw_sprites(0, 40, 26, 0);
    draw_sprites(0, 40, 26, 0);
    draw_sprites(0, 0, 39, 0);
    draw_sprites(0, 0, 40, 0);
    draw_sprites(0, 0, 41, 0);
    break;
  case 0:
    draw_sprites(0, 0, 26, 0);
    draw_sprites(0, 0, 26, 0);
    draw_sprites(0, 0, 26, 0);
    draw_sprites(0, 0, 26, 0);
    draw_sprites(xmario - 16, ymario - 24, 39, 0);
    draw_sprites(xmario, ymario - 24, 40, 0);
    draw_sprites(xmario + 16, ymario - 24, 41, 0);
    delay(5000);
    vida_mario = 4;
    vida_luigi = 4;
    xmario = 176;
    ymario = 208;
    xluigi = 190;
    yluigi = 208;
    setup();
  }
  GD.__end();

  // -----------------------------> Background <-----------------------------

  if ((xscroll & 63) == 0) {
    int offscreen_pixel = ((xscroll + (7 * 64)) & 511);
    byte offscreen_ch = (offscreen_pixel >> 3);
    draw_column(atxy(offscreen_ch, 0));
  }

  // -----------------------------> Som <----------------------------

  // Canal 1
//  int freq = 0;
//  if ((millis() - soundLastTime1 >= soundNextTime1) && enableSound) {
//    if (soundPointer1 < soundSize1) {
//      freq = scaleFrequency[voice1[soundPointer1]] * 4.65;
//      if (!soundPlay1) {
//        playTone(freq, 0, 50);
//        soundPlay1 = 1;
//        soundNextTime1 = (tempo1[soundPointer1] * 30);
//      }
//      else {
//        stopTone(0);
//        soundNextTime1 = (tempo1[soundPointer1] * 2);
//        soundPlay1 = 0;
//        soundPointer1++;
//      }
//    }
//    else {
//      soundPointer1 = 0;
//      soundNextTime1 = 0;
//    }
//    soundLastTime1 = millis();
//  }
//
//  // Canal 2
//  if ((millis() - soundLastTime2 >= soundNextTime2) && enableSound) {
//    if (soundPointer2 < soundSize2) {
//      freq = scaleFrequency[voice1[soundPointer2] - 8] * 4.65;
//      if (!soundPlay2) {
//        playTone(freq, 4, 50);
//        soundPlay2 = 1;
//        soundNextTime2 = (tempo1[soundPointer2] * 30);
//      }
//      else {
//        stopTone(4);
//        soundNextTime2 = (tempo1[soundPointer2] * 2);
//        soundPlay2 = 0;
//        soundPointer2++;
//      }
//    }
//    else {
//      soundPointer2 = 0;
//      soundNextTime2 = 0;
//    }
//    soundLastTime2 = millis();
//  }

  // -----------------------------> Atrasos do loop <----------------------------

  GD.waitvblank();
  GD.wr16(SCROLL_X, xscroll);
}
