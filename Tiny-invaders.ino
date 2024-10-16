//   >>>>>  T-I-N-Y  I-N-V-A-D-E-R-S v3.0 for ATTINY85  GPLv3 <<<<
//						Tinyjoypad rev2 compatible
//                   Programmer: Daniel C 2018-2020
//                   Enhancements: Sven B 2021
//              Contact EMAIL: electro_l.i.b@tinyjoypad.com
//                    https://www.tinyjoypad.com
//         https://sites.google.com/view/arduino-collection

//  Tiny Invaders v4.2 is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.

//  You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

//the code work at 16MHZ internal
//and use ssd1306xled Library for SSD1306 oled display 128x64

// Uncomment the following line if the resulting code exceeds the flash size.
// You will lose the high score being displayed during the title loop.
//#define _REDUCE_FLASH_MEMORY_FOOTPRINT

#include "tinyJoypadUtils.h"  // sbr
#include "spritebank.h"
#include "displayscore.h" // sbr
#include "RLEdecompression.h" // sbr

#define MAXLEVELSHIELDED 3

// there are three different background bitmaps
const uint8_t GAME_SCREEN  = 0; // sbr
const uint8_t INTRO_SCREEN = 1; // sbr
const uint8_t BLANK_SCREEN = 2; // sbr

// special charset: '0'-'9','!?','A'-'Z'
// '@' means '!', other characters are specials symbols (aliens, UFO)
// The strange text definitions as arrays of chars were done 
// - to save the terminating zero a string definition would cause ;)
// - to change <space> to 0x00
const uint8_t PROGMEM txtOneUp[] = {'1','U','P'}; // sbr
const uint8_t PROGMEM txtNewHiScore[] = {'N','E','W',0,'H','I','S','C','O','R','E','@'}; // sbr
const uint8_t PROGMEM txtEnterName[] = {'E','N','T','E','R',0,'Y','O','U','R',0,'N','A','M','E'}; // sbr
const uint8_t PROGMEM txtLevel[] = {'L','E','V','E','L'}; // sbr
// Alien point values
// storing the full text screen is less expensive than addressing the lines individually
const uint8_t PROGMEM txtPointValues[] =                       // sbr
{                    '^','_' ,'`', 0 , 0 ,'1','0', 0 , 0 , 0 , 0 ,   // sbr
  0 , 0 , 0 , 0 , 0 ,'<','=' ,'>', 0 , 0 ,'2','0', 0 , 0 , 0 , 0 ,   // sbr
  0 , 0 , 0 , 0 , 0 ,'[','\\',']', 0 , 0 ,'4','0', 0 , 0 , 0 , 0 ,   // sbr
  0 , 0 , 0 , 0 , 0 ,'a',':' ,';', 0 , 0 ,'?','?'                 }; // sbr
// GAME OVER!
// storing the full text screen is less expensive than adressing the lines individually
const uint8_t PROGMEM txtGameOver[] = 
{'[','\\',']','<','=','>','[','\\',']','^','_','`','a',':',';' , 0 , // sbr
  0 , 0  , 0 ,'G','A','M','E', 0  ,'O','V','E','R', 0 ,'<','=' ,'>', // sbr
 '^','_' ,'`', 0 ,'1','U','P', 0  , 0 , 0 , 0 , 0 , 0 , 0 , 0  , 0 , // sbr
  0 ,'a',':',';','[','\\',']','^','_' ,'`','<','=','>' ,'[','\\',']'  // sbr
};
// EEPROM storage address for highscore and name
const uint16_t TINY_INVADERS_EEPROM_ADDR = 128; // sbr

// var public
uint8_t Live=0;
uint8_t ShieldRemoved=0;
uint8_t MONSTERrest=0;
// counts from 0 to 9, used for difficulty level
uint8_t LEVELS;/*=0;*/ // sbr
// counts the level (not limited)
uint8_t currentLevel;
uint8_t SpeedShootMonster=0;
uint8_t ShipDead=0;
uint8_t ShipPos/*=56*/; // sbr (initialization not necessary, saves 2 bytes)
// display line sized buffer for on-the-fly RLE decompression
uint8_t chunkBuffer[128]; // sbr
// no new highscore yet
bool newHighScore;  // sbr
bool firstRun = true; // sbr
SPACE space;  // sbr
// a new level will slide in from the right
bool    newLevelAnimation;  // sbr
uint8_t levelShiftOffsetX;  // sbr
bool    displayLevelNumber; // sbr
unsigned char currentLevelTextBuffer[3]; // sbr
// fin var public

void setup() {
  // initialize the pins (and serial port if present)
  InitTinyJoypad();
  // perform display initialization
  InitDisplay();
  // restore highscore from EEPROM
  initHighScoreStruct( TINY_INVADERS_EEPROM_ADDR ); // sbr
}

// Changing the two-loop-copy to memcpy_P saved many bytes :)
void LoadMonstersLevels(int8_t Levels,SPACE *space){
  // simply copy the monsters from flash to RAM as a block
  memcpy_P( &space->MonsterGrid[0][0], MonstersLevels + Levels * 24, 24 );
  // and some empty space below
  memset( &space->MonsterGrid[4][0], -1, 6 );
}

void loop() {
  #define SHOOTS 2
  uint8_t Decompte=0;
  uint8_t VarPot;
  uint8_t MyShootReady=SHOOTS;
  newHighScore = false; // sbr

NEWGAME:
  // remove all text from the buffer
  clearTextBuffer();  // sbr
  // show new highscore?
  if ( newHighScore ) // sbr <start>
  {
    // Congrats!
    showNewHighScore( &space );
  }
  else if ( !firstRun )
  {
    // GAME OVER!
    showGameOver( &space );
  } // sbr <end>
  firstRun = false; // sbr
  resetScore(); // sbr
  Live=3;
  LEVELS=0;
  currentLevel = 1; // sbr
  // don't "mirror" the background
  newLevelAnimation = false;    // sbr
  displayLevelNumber = false;   // sbr
  levelShiftOffsetX = 0;        // sbr 
  clearBattleground( &space );  // sbr

  uint8_t n = 0;
  while(1){
    // clear text buffer
    clearTextBuffer();            // sbr
    
    if ( n++ < 20 )
    {
      showIntroScreen( &space );  // sbr
    }
    else if ( n < 35 )
    {
      showPointValues( &space );
    }
  #ifndef _REDUCE_FLASH_MEMORY_FOOTPRINT    
    else if ( n < 50 )
    {
      showHighScore( &space );
    }
  #endif
    else
    {
      n = 0;
    }
    // exit if button pressed
    if (digitalRead( FIRE_BUTTON )==0) {bebeep(); goto BYPASS2;} // sbr
    // take a nap to save battery
    _delay_ms( 200 );
  }
  
NEWLEVEL:
  //_delay_ms(1000);

BYPASS2:
  // remove all monsters from the screen
  clearBattleground( &space );      // sbr
  // display current level number
  displayLevelNumber = true;        // sbr
  Tiny_Flip( GAME_SCREEN,&space );  // sbr
  displayLevelNumber = false;       // sbr
  // wait a moment
  _delay_ms(1000);                  // sbr

  VarResetNewLevel(&space);
  SpeedControle(&space);
  VarPot=54;
  ShipPos=56;
  // moved to to Tiny_Flip()
  //space.ScrBackV=(ShipPos/14)+52; // sbr
  goto Bypass;
  RestartLevel:
  if (Live>0) {Live--;}else{goto NEWGAME;}
Bypass:
  ShipDead=0;
  Decompte=0;
  while(1){
    if (MONSTERrest==0) {
    //if (MONSTERrest <20) {
      Sound(110,255);_delay_ms(40);Sound(130,255);_delay_ms(40);Sound(100,255);
      _delay_ms(40);Sound(1,155);_delay_ms(20);Sound(60,255);Sound(60,255);
      //memset( space.MonsterGrid, 0xff, sizeof( space.MonsterGrid ) ); 
      clearBattleground( &space );    // sbr
      // let the new level slide in from the right
      newLevelAnimation = true;   // sbr
      currentLevel++;             // sbr
      // the difficulty level reaches from 0..9
      if (LEVELS<9) {LEVELS++;}
      // display new level animation
      while ( newLevelAnimation ) // sbr
      {                                 // sbr
        // display background
        Tiny_Flip( GAME_SCREEN,&space); // sbr
      }                                 // sbr
      _delay_ms(500);                   // sbr

      // start next level
      goto NEWLEVEL;
    }
    // adapt the left and right postion limit to the remaining monsters
    calcMonsterPositionLimits( &space );
    if ((((space.MonsterGroupeYpos)+(space.MonsterFloorMax+1))==7)&&(Decompte==0)) {ShipDead=1;}
    if (SpeedShootMonster<=((9-LEVELS))) {SpeedShootMonster++;}
    else{SpeedShootMonster=0;MonsterShootGenerate(&space);}
    // moved to to Tiny_Flip()
    //space.ScrBackV=(ShipPos/14)+52; // sbr
    Tiny_Flip(GAME_SCREEN,&space); // sbr
    space.oneFrame=!space.oneFrame;
    RemoveExplodOnMonsterGrid(&space);
    MonsterShootupdate(&space);
    UFOUpdate(&space);
    if (    ((space.MonsterGroupeXpos>=26)&&(space.MonsterGroupeXpos<=28))
         && (space.MonsterGroupeYpos==2)
         && (space.DecalageY8==4)
       ) {space.UFOxPos=127;}
    if (VarPot>(ShipPos+2)) {ShipPos=ShipPos+((VarPot-ShipPos)/3);}
    if (VarPot<(ShipPos-2)) {ShipPos=ShipPos-((ShipPos-VarPot)/3);}
    if (ShipDead!=1) {
      if (space.frame<space.frameMax) {
        space.frame++;
      }
      else{
        GRIDMonsterFloorY(&space);
        space.anim=!space.anim;
        if (!space.anim){Sound(100,1);}
        else{Sound(200,1);}
        MonsterRefreshMove(&space);
        space.frame=0;
      }
      // just read the input once
      uint16_t input = analogRead( LEFT_RIGHT_BUTTON ) - 500; // sbr
      // right button pressed?
      //if ( (input>500)&&(input<750) ) 
      if ( input<250 ) {  // sbr
        if (VarPot<108) { VarPot=VarPot+6; }
      }
      // left button pressed?
      //if ( (input>=750)&&(input<200 )
      input -= 250; // sbr
      if ( input < 200 ) { // sbr
        if (VarPot>5) { VarPot=VarPot-6; }
      }
      // check for fire button
      if ((digitalRead( FIRE_BUTTON )==0)&&(MyShootReady==SHOOTS)){ // sbr
        Sound(200,4);
        MyShootReady=0;
        space.MyShootBall=6;
        space.MyShootBallxpos=ShipPos+6;
      }
    }
    else{
      //pgm_playSound( soundCrawling, sizeof( soundCrawling ) / 3 );
      Sound(80,1);Sound(100,1); 
      Decompte++;
      if (Decompte>=30) {
        _delay_ms(600);
        if (((space.MonsterGroupeYpos)+(space.MonsterFloorMax+1))==7) {goto NEWGAME;}
        else{goto RestartLevel;}
      }
    }
    if (space.MyShootBall==-1) {
      if (MyShootReady<SHOOTS) {MyShootReady++;}
    }
  }
}
////////////////////////////////// main end /////////////////////////////////

void SpeedControle(SPACE *space){
  MONSTERrest=0;

  // for counting it's ok to consider the array one dimensional
  int8_t *grid = &space->MonsterGrid[0][0];
  for ( uint8_t n = 0; n < 24; n++ ) {
    if ( ( grid[n] !=-1 ) && ( ( grid[n] <=5 ) ) ){MONSTERrest++;}
  }

  space->frameMax=(MONSTERrest/8 );
}

void GRIDMonsterFloorY(SPACE *space){
  uint8_t y,x; 
  space->MonsterFloorMax=3;
  // check all 4 rows
  for (y=3;y>=0;y--){     // sbr
    // and all 6 columns
    for (x=0;x<=5;x++){   // sbr
      if (space->MonsterGrid[y][x]!=-1) {goto FIN;}
    }
    space->MonsterFloorMax=space->MonsterFloorMax-1;
  }
FIN:;
}

uint8_t LivePrint(uint8_t x,uint8_t y){
#define XLIVEWIDE ((5*Live)-1)
  if (((0>=(x-XLIVEWIDE)))&&(y==7)) {
    return pgm_read_byte(&LIVE[(x)]);
  }
  return 0x00;
}

void Tiny_Flip(uint8_t render0_picture1,SPACE *space){
  uint8_t y,x; 
  uint8_t MYSHIELD /*=0x00*/;

  // select the appropriate bitmap
  const uint8_t *render = ( render0_picture1 == GAME_SCREEN ) ? back_compressed   /* BACKGROUND */
                                                              : intro_compressed; /* INTRO_SCREEN *or* BLANK_SCREEN */

  // we want an arcade style live highscore display
  newHighScore |= updateHighScorePoints();  // sbr
  if ( render0_picture1 == GAME_SCREEN ) // sbr
  { 
    // remove old text from screen
    clearTextBuffer();
    // score in the top left corner
    pgm_printText( 0, txtOneUp, sizeof( txtOneUp ) ); // sbr
    // high score in the top right corner
    printText( 22, getHighScoreName(), 3 ); // sbr
    // if new high score, change name to '1UP'
    if ( newHighScore )
    {
      pgm_printText( 22, txtOneUp, sizeof( txtOneUp ) );
    }
    convertValueToDigits( getScore(), getTextBuffer() + 4 ); // sbr
    convertValueToDigits( getHighScorePoints(), getTextBuffer() + 26 ); // sbr

    // print LEVEL string
    pgm_printText( 36, txtLevel, sizeof( txtLevel ) );            // sbr
    // convert number of current level to a string
    convertValueToDigits( currentLevel, getTextBuffer() + 42  );  // sbr
  } // sbr

  // just calculate the offset once
  //space.ScrBackV=(ShipPos/14)+52;
  calcNewBackgroundOffset( space ); // sbr

  for (y = 0; y < 8; y++)
  {
    // uncompress chunk and save next address
    render = pgm_RLEdecompressExt8( render, chunkBuffer, 128 ); // sbr

    // prepare display of row <y>
    PrepareDisplayRow( y );
    
    for (x = 0; x < 128; x++)
    {
      uint8_t pixels; // sbr
      if (render0_picture1 == GAME_SCREEN) {
        if (ShieldRemoved==0) {MYSHIELD=MyShield(x,y,space);}
        else{MYSHIELD=0x00;}
        pixels = (   background(x,y,space)
                   | LivePrint(x,y)
                   | Vesso(x,y,space)
                   | UFOWrite(x,y,space)
                   | Monster(x,y,space)
                   | MyShoot(x,y,space)
                   | MonsterShoot(x,y,space)
                   | MYSHIELD
                   | displayText(x,y) // sbr
                 );
                 
        // overlay level number?
        if ( displayLevelNumber )
        {
          if ( ( y >= 4 ) && ( y <= 5 ) )
          {
            pixels |= displayZoomedText( x, y );
          }
        }
      }
      // render intro screen
      else if ( render0_picture1 == INTRO_SCREEN ) {
        pixels = chunkBuffer[x];
      }
      // render text mode 16x4 only
      else {
        pixels = displayZoomedText(x,y);        
      }
      // send 8 vertical pixels to the display
      SendPixels( pixels );
    } // for x
    
    if (render0_picture1 == GAME_SCREEN) {
      if (ShieldRemoved==0) {
        ShieldDestroy(0,space->MyShootBallxpos,space->MyShootBall,space);
        // shouldn't the transmission be always stopped?
        //SSD1306.ssd1306_send_data_stop(); // sbr
      }
    }

    // this row has been finished
    FinishDisplayRow();
  } // for y
  
  if (render0_picture1 == GAME_SCREEN) {
    if ((space->MonsterGroupeYpos<(2+(4-(space->MonsterFloorMax+1))))/*&&(LEVELS<=MAXLEVELSHIELDED)*/) {}
    else{
      if (ShieldRemoved!=1) {
        // memset saves some bytes
        memset( &space->Shield[0], 0x00, sizeof( space->Shield ) ); // sbr
        ShieldRemoved=1;
      }
    }
  }

  // display the whole screen
  DisplayBuffer();
}

uint8_t UFOWrite(uint8_t x,uint8_t y,SPACE *space){
  if ((space->UFOxPos!=-120)&&(y==0)&&((space->UFOxPos<=(x))&&(space->UFOxPos>=(x-14)))) {return pgm_read_byte(&Monsters[(x-space->UFOxPos)+(6*14)+(space->oneFrame*14)]); }
  return 0x00;
}

void UFOUpdate(SPACE *space){
  if (space->UFOxPos!=-120) {
    space->UFOxPos=space->UFOxPos-2;
    if (space->UFOxPos<=-20) {space->UFOxPos=-120;}
  }
}

void ShipDestroyByMonster(SPACE *space){ 
  if (   (space->MonsterShoot[1]>=14)
       &&(space->MonsterShoot[1]<=15)
       &&(space->MonsterShoot[0]>=ShipPos)
       &&(space->MonsterShoot[0]<=ShipPos+14))
  {
    ShipDead=1;
  }
}

void MonsterShootupdate(SPACE *space){
  if (space->MonsterShoot[1]!=16) {
    ShipDestroyByMonster(space);
    if (ShieldDestroy(1,space->MonsterShoot[0],space->MonsterShoot[1]/2,space)) {
      space->MonsterShoot[1]=16;
    }
    else{
      space->MonsterShoot[1]=space->MonsterShoot[1]+1;
    }
  }
}

void MonsterShootGenerate(SPACE *space){ 
  uint8_t a=random()%3; 
  uint8_t b=random()%6; 
  //if (b>=5) {b=5;}  // sbr
  if (space->MonsterShoot[1]==16) {  
    if (space->MonsterGrid[a][b]!=-1) {
      space->MonsterShoot[0]=(space->MonsterGroupeXpos+7)+(b*14);
      space->MonsterShoot[1]=(((space->MonsterGroupeYpos)+a)*2)+1;
    }  
  }
}

uint8_t MonsterShoot(uint8_t x,uint8_t y,SPACE *space){
  if ((((space->MonsterShoot[1])/2)==y)&&(space->MonsterShoot[0]==x) ) {
    if (((space->MonsterShoot[1])%2)==0) {return 0b00001111;}
    else{return 0b11110000;}
  }
  //if ((((space->MonsterShoot[1])/2)==y)&&(space->MonsterShoot[0]==x) ) {
  //  if (((space->MonsterShoot[1])%2)==0) {return 0b00001111;}
  //  else{return 0b11110000;}
  //}
  return 0x00;
}

// sbr: reworked the original code to free some flash memory
uint8_t ShieldDestroy(uint8_t Origine,uint8_t VarX,uint8_t VarY,SPACE *space){
  #define OFFSETXSHIELD -1
  if ( VarY==6 )
  {
    // same trick again: VarX is unsigned... let's take advantage of that!
    VarX -= (20+OFFSETXSHIELD);

    // there are three shields
    for ( uint8_t n = 0; n < 6; n++ )
    {
      if ( VarX <= 7 ) {
        if ((BOOLREAD(n,VarX,space))) {ShieldDestroyWrite(n,VarX,space,Origine);return 1;}
      }
      n++;
      
      VarX -= 8;
      if ( VarX <= 7 ) {
        if ((BOOLREAD(n,VarX,space))) {ShieldDestroyWrite(n,VarX,space,Origine);return 1;}
      }
  
      // next shield position
      VarX -= 27;
    }
  }
  return 0;
}

// sbr: reworked the original code to free some flash memory
void ShieldDestroyWrite(uint8_t BOOLWRITE,uint8_t line,SPACE *space,uint8_t Origine){
  space->Shield[BOOLWRITE] &= ~( 0b10000000 >> line ); // sbr
  if (Origine==0) {space->MyShootBall=-1;} // sbr
}


// sbr: reworked the original code to free some flash memory
uint8_t MyShield(uint8_t x,uint8_t y,SPACE *space){
  #define OFFSETXSHIELD -1
  // only check in the line before the last
  if ( y == 6 )
  {
    // same trick again: x is unsigned... let's take advantage of that!
    x -= (20+OFFSETXSHIELD);

    // there are three shields
    for ( uint8_t n = 0; n < 6; n += 2 )
    {
      if ( x <= 7 ) {
        if ((BOOLREAD(n,x,space))) {return ShieldBlitz(0,x);}else{return 0x00;}
      }
      
      x -= 8;
      if ( x <= 7 ) {
        if ((BOOLREAD(n + 1,x,space))) {return ShieldBlitz(1,x);}else{return 0x00;}
      }
  
      // next shield position
      x -= 27;
    }
  }
  return 0x00;
}

uint8_t ShieldBlitz(uint8_t Part,uint8_t LineSH ){
  uint8_t Var0/*=0*/; // sbr
  switch (LineSH){
    case 0:if (Part==0) {Var0=0b11110000;}else{Var0=0b00001111;}break;
    case 1:if (Part==0) {Var0=0b11111100;}else{Var0=0b00001111;}break;
    case 2:
    case 3:
    case 4:
    case 5:Var0=0b00001111;break;
    case 6:if (Part==1) {Var0=0b11111100;}else{Var0=0b00001111;}break;
    case 7:if (Part==1) {Var0=0b11110000;}else{Var0=0b00001111;}break;
    default:Var0=0b00000000;break;
  }
  return Var0;
}

uint8_t BOOLREAD(uint8_t SHnum,uint8_t LineSH,SPACE *space ){
  uint8_t Var0=(0b10000000>>LineSH);
  if ((space->Shield[SHnum]&Var0)!=0) {return 1;}
  else{return 0;}
}

void RemoveExplodOnMonsterGrid(SPACE *space){
  uint8_t x=0,y=0;
  for (y=0;y<=3;y++){ //était a 5
    for (x=0;x<=5;x++){
      if (space->MonsterGrid[y][x]>=11) {space->MonsterGrid[y][x]=-1;} 
      if (space->MonsterGrid[y][x]>=8) {space->MonsterGrid[y][x]=space->MonsterGrid[y][x]+1;}
    }
  }
}
  
uint8_t background(uint8_t x,uint8_t y,SPACE *space){
  uint8_t scr=(space->ScrBackV+x);
  //if ((scr)>127) {scr=(space->ScrBackV+x)-128;}
  scr &= 0x7f;  // sbr
  return 0xff-chunkBuffer[scr]; // sbr
}
 
uint8_t Vesso(uint8_t x,uint8_t y,SPACE *space){
  if (((x-ShipPos)>=0)&&((x-ShipPos)<13)&&(y==7)) {
    if (ShipDead==0) {return pgm_read_byte(&vesso[(x-ShipPos)]);}
    else{return pgm_read_byte(&vesso[(x-ShipPos)+(12*space->oneFrame)]);}
  }
  return 0;
}

void UFO_Attack_Check(uint8_t x,SPACE *space){
  if (space->MyShootBall==0) {
    if ((space->MyShootBallxpos>=space->UFOxPos)&&(space->MyShootBallxpos<=(space->UFOxPos+14))) {
      for (x=1;x<100;x++){
        Sound(x,1);
      }
      if (Live<3) { Live++; addScore( 50 ); } // sbr
      else{ addScore( 150 ); }                // sbr
      space->UFOxPos=-120;
    }
  }
}

 uint8_t MyShoot(uint8_t x,uint8_t y,SPACE *space){
  if ((space->MyShootBallxpos==x)&&(y==((space->MyShootBall)))) {
    if (space->MyShootBall>-1) {space->MyShootBallFrame=!space->MyShootBallFrame;}else{return 0x00;}
    if (space->MyShootBallFrame==1) {space->MyShootBall--;} 
    Monster_Attack_Check(space);
    UFO_Attack_Check(x,space);
    return pgm_read_byte(&SHOOT[(space->MyShootBallFrame)]);
  }
  return 0x00;
}

void Monster_Attack_Check(SPACE *space){
  int8_t Varx=0,Vary=0;
  #define Xmouin (space->MonsterGroupeXpos) 
  #define Ymouin ((space->MonsterGroupeYpos)*8)//-space->DecalageY8
  #define XPlus (Xmouin+84)
  #define YPlus (Ymouin+(4*8))
  #define MYSHOOTX (space->MyShootBallxpos)
  #define MYSHOOTY ((space->MyShootBall*8)+(((space->MyShootBallFrame)+1)*4))
  if ((MYSHOOTX>=Xmouin)&&(MYSHOOTX<=XPlus)&&(MYSHOOTY>=(Ymouin))&&(MYSHOOTY<=YPlus)){
    //enter in the monster zone
    Vary= (round((MYSHOOTY-Ymouin)/8));
    Varx= (round((MYSHOOTX-Xmouin)/14));
    if (Varx<0) {Varx=0;}
    if (Vary<0) {Vary=0;}
    if (Varx>5) {goto End;}
    if (Vary>3) {goto End;}
    // the array contains 'int8_t' but we consider it 'uint8_t'...
    uint8_t monster = space->MonsterGrid[Vary][Varx]; // sbr
    // '-1' equals '255' unsigned, so it is larger than '6'
    if ( monster < 6 ) {                // sbr
      // gotcha!
      Sound(50,10);
      if ( monster < 2 ) { addScore( 10 ); }  // sbr
      if ( monster < 4 ) { addScore( 10 ); }  // sbr
      addScore( 10 );                         // sbr
      space->MonsterGrid[Vary][Varx]=8;
      space->MyShootBall=-1;
      SpeedControle(space);
    }
    //fin monster zone
  }
  End:;
}

int8_t OuDansLaGrilleMonster(uint8_t x,uint8_t y,SPACE *space){
  if (x<space->MonsterGroupeXpos) {return -1;}
  if (y<space->MonsterGroupeYpos) {return -1;}
  space->PositionDansGrilleMonsterX=(x-space->MonsterGroupeXpos)/14; 
  space->PositionDansGrilleMonsterY=(y-space->MonsterGroupeYpos);
  if ((space->PositionDansGrilleMonsterX)>5)  {return -1;}
  if ((space->PositionDansGrilleMonsterY)>4)  {return -1;}
  return 0;
}

uint8_t SplitSpriteDecalageY(uint8_t Input,uint8_t UPorDOWN,SPACE *space){
  if (UPorDOWN) {
    return Input<<space->DecalageY8;
  }else{
    return Input>>(8-space->DecalageY8);
  }
}

uint8_t Murge_Split_UP_DOWN(uint8_t x,SPACE *space){
  int8_t SpriteType=-1;
  int8_t ANIMs=-1;
  uint8_t Murge1=0;
  uint8_t Murge2=0;
  if (space->DecalageY8==0) {
    SpriteType=space->MonsterGrid[space->PositionDansGrilleMonsterY][space->PositionDansGrilleMonsterX];
    if (SpriteType < 8) {ANIMs=(space->anim*14);}else{ANIMs=0;}
    if (SpriteType < 0) {return 0x00;}
    return pgm_read_byte(&Monsters[(WriteMonster14(x-space->MonsterGroupeXpos)+SpriteType*14)+ANIMs]);
  }
  else{ //debut
    if (space->PositionDansGrilleMonsterY==0) {
      SpriteType=space->MonsterGrid[space->PositionDansGrilleMonsterY][space->PositionDansGrilleMonsterX];
      if (SpriteType < 8) {ANIMs=(space->anim*14);}else{ANIMs=0;}
      if (SpriteType >= 0) { Murge2=SplitSpriteDecalageY(pgm_read_byte(&Monsters[(WriteMonster14(x-space->MonsterGroupeXpos)+SpriteType*14)+ANIMs]),1,space);}
      //else{ Murge2=0x00;}
      return Murge2;    
    }
    else{
      SpriteType=space->MonsterGrid[space->PositionDansGrilleMonsterY-1][space->PositionDansGrilleMonsterX];
      if (SpriteType < 8) {ANIMs=(space->anim*14);}else{ANIMs=0;}
      if (SpriteType >= 0) {Murge1=SplitSpriteDecalageY(pgm_read_byte(&Monsters[(WriteMonster14(x-space->MonsterGroupeXpos)+SpriteType*14)+ANIMs]),0,space);}
      //else{Murge1=0x00;}
      SpriteType=space->MonsterGrid[space->PositionDansGrilleMonsterY][space->PositionDansGrilleMonsterX];
      if (SpriteType < 8) {ANIMs=(space->anim*14);}else{ANIMs=0;}
      if (SpriteType >= 0) { Murge2=SplitSpriteDecalageY(pgm_read_byte(&Monsters[(WriteMonster14(x-space->MonsterGroupeXpos)+SpriteType*14)+ANIMs]),1,space);}
      //else{Murge2=0x00;}
      return Murge1|Murge2;    
    }  
  } //fin
}

uint8_t WriteMonster14(uint8_t x){
  while( x >= 14 )
  {
    x -= 14;
  }
  return x;
}

uint8_t Monster(uint8_t x,uint8_t y,SPACE *space){
  if (OuDansLaGrilleMonster(x,y,space)==-1) 
  {return 0x00;} //quiter la fonction si pas dans la grille
  return Murge_Split_UP_DOWN(x,space);
  //return 0x00;
}//end Monster();

uint8_t MonsterRefreshMove(SPACE *space){
  if (space->Direction==1) {
    if ( space->MonsterGroupeXpos<space->MonsterOffsetDroite ) {
      space->MonsterGroupeXpos=space->MonsterGroupeXpos+2;
      return 0;
    }
    else{
      if (space->DecalageY8<7) {
        space->DecalageY8=space->DecalageY8+4;
        if (space->DecalageY8>7) { space->DecalageY8=7; }
      }
      else {
        space->MonsterGroupeYpos++;space->DecalageY8=0;
      }
      // change direction
      space->Direction=0;
      return 0;
    }
  }
  else{
    if ( space->MonsterGroupeXpos>space->MonsterOffsetGauche ) {
      space->MonsterGroupeXpos=space->MonsterGroupeXpos-2;
      return 0;
    }
    else{
      if (space->DecalageY8<7) {
        space->DecalageY8=space->DecalageY8+4;
        if (space->DecalageY8>7) { space->DecalageY8=7; }
      }
      else{
        space->MonsterGroupeYpos++;space->DecalageY8=0;
      }
      space->Direction=1;
      return 0;
    }
  }
}

/*-------------------------------------------------------*/

void VarResetNewLevel(SPACE *space){
  ShieldRemoved=0;
  SpeedShootMonster=0;
  MONSTERrest=24;
  // set full SPACE structure to '0'
  memset( space, 0, sizeof( SPACE ) );
  LoadMonstersLevels(LEVELS,space);
  // set Shield[] to 255
  memset( &space->Shield[0], 255, sizeof( space->Shield ) );
  //space->Shield[0]=255;  
  //space->Shield[1]=255;  
  //space->Shield[2]=255;  
  //space->Shield[3]=255;  
  //space->Shield[4]=255;  
  //space->Shield[5]=255;  
  space->MonsterShoot[0]=16;
  space->MonsterShoot[1]=16;
  space->UFOxPos=-120;
  space->MyShootBall=-1;
  //space->MyShootBallxpos=0;
  //space->MyShootBallFrame=0;
  //space->anim=0;
  //space->frame=0;
  //space->PositionDansGrilleMonsterX=0;
  //space->PositionDansGrilleMonsterY=0;
  space->MonsterFloorMax=3;
  // left and right offsets will be calculated before each frame
  //space->MonsterOffsetGauche=0;  // sbr
  //space->MonsterOffsetDroite=44; // sbr
  space->MonsterGroupeXpos=20;
  if (LEVELS>3) {space->MonsterGroupeYpos=1;}else{space->MonsterGroupeYpos=0;}
  //space->DecalageY8=0;
  space->frameMax=8;
  space->Direction=1; //1 right 0 gauche  
  //space->oneFrame=0;
  newLevelAnimation = false; // sbr
  //space->levelShiftOffsetX = 0;     // sbr
}

/*--------------------------------------------------------------*/
// just the standard 'be-beep'
void bebeep()
{
  Sound(100,125); Sound(50,125);
}

/*--------------------------------------------------------------*/
// Displays the 'NEW HISCORE' message and allows the victorius player
// to enter his initials to be saved for eternity in the EEPROM.
// [The parameter 'space' isn't really necessary here, a NULL pointer
//  would do for 'Tiny_Flip()', but when the compiler inlines the code, 
//  this version saves 2 bytes]
// Valid characters for the name are '?','*','A'-'Z'
void showNewHighScore(SPACE *space )
{
  pgm_printText( 0 * 16 + 2, txtNewHiScore, sizeof( txtNewHiScore ) );
  pgm_printText( 2 * 16 + 0, txtEnterName, sizeof( txtEnterName ) );

  // get pointer to the text buffer (3rd line, highscore position)
  uint8_t *textBuffer = getTextBuffer() + 1 * 16 + 6;
  convertValueToDigits( getHighScorePoints(), textBuffer );
  // place cursor in the center of the last line
  textBuffer += 33;

  uint8_t *initials = getHighScoreName();
  uint8_t *initialsEnd = initials + 3;

  while ( initials < initialsEnd )
  {
    // print name
    printText( 3 * 16 + 7, getHighScoreName(), 3 );
    // wait for 'fire'
    do
    {
      // get current letter
      uint8_t letter = *initials;
      
      // read the analog input only once
      uint16_t inputX = analogRead( LEFT_RIGHT_BUTTON ) - 500;
      uint16_t inputY = analogRead( UP_DOWN_BUTTON ) - 500;

      // right/up button pressed?
      if ( ( inputX < 250 ) || ( inputY < 250 ) )
      {
        letter++;
        if ( letter > 'Z' ) { letter = '?'; }
        // signal button press detected
        bebeep();
      }
      
      // left/down button pressed?
      inputX -= 250;
      inputY -= 250;
      if ( ( inputX < 200 ) || ( inputY < 200 ) )
      {
        letter--;
        if ( letter < '?' ) { letter = 'Z'; }
        // signal button press detected
        bebeep();
      }

      // save changed letter to high score structure
      *initials = letter;
      // highlight the current letter on screen
      *textBuffer = letter | 0x80;
      // display letter
      Tiny_Flip( BLANK_SCREEN,space);
    }
    while ( digitalRead( FIRE_BUTTON ) ); // sbr
    
    // ok, got it!
    bebeep();
    // wait for 'fire' released
    while ( !digitalRead( FIRE_BUTTON ) );  // sbr

    // next letter
    initials++;
    textBuffer++;
  }

  // store the new highscore with the entered name 
  storeHighScoreToEEPROM( TINY_INVADERS_EEPROM_ADDR );
  newHighScore = false;
}

/*--------------------------------------------------------------*/
// Displays the game over message along with the player's score
// [The parameter 'space' isn't really necessary here, a NULL pointer
//  would do for 'Tiny_Flip()', but when the compiler inlines the code, 
//  this version saves 2 bytes]
void showGameOver(SPACE *space)
{
  // just a simple 'GAME OVER!' screen with some aliens
  pgm_printText( 0, txtGameOver, sizeof( txtGameOver ) );
  // print player score
  uint8_t *textBuffer = getTextBuffer() + 2 * 16 + 8;
  convertValueToDigits( getScore(), textBuffer ); 
  // display!
  Tiny_Flip( BLANK_SCREEN,space);
  _delay_ms(3000);
}

/*--------------------------------------------------------------*/
// [The parameter 'space' isn't really necessary here, a NULL pointer
//  would do for 'Tiny_Flip()', but when the compiler inlines the code, 
//  this version saves 2 bytes]
void showIntroScreen( SPACE *space )
{
  Tiny_Flip( INTRO_SCREEN, space );
}

/*--------------------------------------------------------------*/
// [The parameter 'space' isn't really necessary here, a NULL pointer
//  would do for 'Tiny_Flip()', but when the compiler inlines the code, 
//  this version saves 2 bytes]
void showPointValues( SPACE *space )
{
  pgm_printText( 5, txtPointValues, sizeof( txtPointValues ) );

  // display!
  Tiny_Flip( BLANK_SCREEN, space);
}

#ifndef _REDUCE_FLASH_MEMORY_FOOTPRINT
/*--------------------------------------------------------------*/
// [The parameter 'space' isn't really necessary here, a NULL pointer
//  would do for 'Tiny_Flip()', but when the compiler inlines the code, 
//  this version saves 2 bytes]
void showHighScore( SPACE *space )
{
  // just a simple 'GAME OVER!' screen with some aliens
  pgm_printText( 0, txtGameOver, sizeof( txtGameOver ) );
  
  // replace 'GAME OVER!' with ' HISCORE '
  pgm_printText( 1 * 16 + 3, txtNewHiScore + 3, sizeof( txtNewHiScore ) - 3 );
  // remove the '!'
  uint8_t *textBuffer = getTextBuffer();
  textBuffer[1 * 16 + 11] = 0;

  // print player name
  printText( 2 * 16 + 4, getHighScoreName(), 3 );
  // print high score
  convertValueToDigits( getHighScorePoints(), textBuffer + 2 * 16 + 8 ); 
  // display!
  Tiny_Flip( BLANK_SCREEN,space);
}
#endif

/*--------------------------------------------------------------*/
// Calculate the backgound offset to get the background a little
// less static.
// Between the levels the background image will be shifted by about 50%.
// This image wrapping gives almost the same result as mirroring the 
// image (which would cost some more bytes)
void calcNewBackgroundOffset( SPACE *space )
{
  uint8_t scrBackV = (ShipPos/14) + 52;

  if ( newLevelAnimation )
  {
    // move the background image by four pixels
    levelShiftOffsetX += 4;
    // limit the offset to 0..127
    levelShiftOffsetX &= 0x7f;
    // sliding is finished if 'levelShiftOffsetX' reaches the next base position 
    // (it's 68 because 64 didn't look good with the planet wrapping around when moving to the borders of the screen)
    if (    ( levelShiftOffsetX == 0 )
         || ( levelShiftOffsetX == 68 )
       )
    {
      // done!
      newLevelAnimation = false;
    }
  }
  space->ScrBackV = ( scrBackV + levelShiftOffsetX ) & 0x7f;
}

/*--------------------------------------------------------------*/
// remove all monsters, shields, explosions and shots from the screen
void clearBattleground(SPACE *space)
{
  memset( space->MonsterGrid, 0xff, sizeof( space->MonsterGrid ) ); 
  memset( space->Shield, 0, sizeof( space->Shield ) );
  // reset UFO and all shots
  space->UFOxPos=-120;
  space->MyShootBall=-1;
  space->MonsterShoot[0]=16;
  space->MonsterShoot[1]=16;
  // center ship
  ShipPos=56;
}

/*--------------------------------------------------------------*/
// Calculate the left and right limits for the moving monsters 
void calcMonsterPositionLimits( SPACE *space )
{
  // no valid positions found 
  space->MonsterOffsetGauche = 127;
  space->MonsterOffsetDroite = -128;

  /*--- determine the leftmost position ---*/
  for ( int8_t x = 0; x < 6; x++ ) {
    uint8_t monsterPresent = 0xff;
    // rows
    for ( int8_t y = 0; y < 4; y++ ) {
      // if any monster is present, bits will be removed
      monsterPresent &= space->MonsterGrid[y][x];
    }
    // monster found?
    if ( monsterPresent != 0xff )
    {
      // monster found!
      if ( space->MonsterOffsetGauche == 127 ) { space->MonsterOffsetGauche = - ( x * 14 ); }
    }
  }

  /*--- determine the rightmost position ---*/
  for ( int8_t x = 5; x >= 0; --x ) {
    uint8_t monsterPresent = 0xff;
    // rows
    for ( uint8_t y = 0; y < 4; y++ ) {
      // if any monster is present, bits will be removed
      monsterPresent &= space->MonsterGrid[y][x];
    }
    // monster found?
    if ( monsterPresent != 0xff )
    {
      // monster found!
      if ( space->MonsterOffsetDroite == -128 ) { space->MonsterOffsetDroite = 44 + ( 5 - x ) * 14; }
    }
  }
}
