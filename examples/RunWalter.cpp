#include <Keypad.h>
#include <SPI.h>
#include <GD.h>
#include "runwalter_walter.h"
#include "runwalter_image.h"

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


#define boolean bool
#define byte char

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

const char ROWS = 4 ; // Four rows
const char COLS = 4; // Three columns

char rowPins[ROWS] = {ROW0,ROW1,ROW2, ROW3};
char colPins[COLS] = {COL0, COL1, COL2, COL3};

char keys[ROWS][COLS]={
  {'a','w','s','d'},//setas wasd
  {'h','k','j','l'},//setas VIM
  {'1','2','3','4'},
  {'5','6','7','8'}
};

/*VERDE PINO 0
  {'P1 - LEFT','P1 - DOWN','P1 - UP','P1 - RIGHT'},    
  {P2-LEFT,P2-DOWN,P2-UP',P2-RIGHT},   
  {'P1-GREEN-RIGHT','P1-BLUE','P1 - GREEN_LEFT','P1 - RED'},    
  {'P2-GREEN_RIGHT','P2-BLUE','P1-GREEN_LEFT','P2 - RED'},    
  */

Keypad player = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );


struct WalterWhite{
int x;
int y;
int rot;
bool anim;
};

static struct WalterWhite W1, W2;

void setup()
{
  GD.begin();
  GD.copy(RAM_SPRPAL, walter_sprpal, sizeof(walter_sprpal));
  GD.copy(RAM_SPRIMG, walter_sprimg, sizeof(walter_sprimg));
  for (byte y = 0; y < 37; y++)
    GD.copy(RAM_PIC + y * 64, image_pic + y * 49, 49);
  GD.copy(RAM_CHR, image_chr, sizeof(image_chr));
  GD.copy(RAM_PAL, image_pal, sizeof(image_pal));
  W1.x = 100;
  W1.y = 190;
  W1.rot = 0;
  W1.anim = 0;
  
  W2.x = 300;
  W2.y = 190;
  W2.rot = 0;
  W2.anim = 0;
}

void mostraSprite()
{

    GD.waitvblank();
    GD.__wstartspr(0);
    draw_ball(W1.x, W1.y, W1.anim, W1.rot);
    draw_ball(W2.x, W2.y, W2.anim, W2.rot);
    GD.__end();
}

void loop()
{
  char key = player.getKey();  
  if(player.P1_LEFT)W1.x--;
  if(player.P1_DOWN)W1.y--;
  if(player.P1_UP)W1.y++;
  if(player.P1_RIGHT)W1.x++;
  
  if(player.P2_LEFT)W2.x--;
  if(player.P2_DOWN)W2.y--;
  if(player.P2_UP)W2.y++;
  if(player.P2_RIGHT)W2.x++;
  
  if(player.P1_B1)W1.rot=1;
  if(player.P1_B2)W1.rot=2;
  if(player.P1_B3)W1.rot=3;
  if(player.P1_B4)W1.rot=0;
  
  if(player.P2_B1)W2.rot=0;
  if(player.P2_B2)W2.rot=1;
  if(player.P2_B3)W2.rot=2;
  if(player.P2_B4)W2.rot=3;
 
  mostraSprite();

  if(millis()-lastAnimTime >= animTime){
    lastAnimTime = millis();
    if(W1.anim==0) W1.anim=1;
    else W1.anim=0;
    W2.anim = !W1.anim;
  }



  
}







