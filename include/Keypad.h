/*
 * =====================================================================================
 *
 *       Filename:  Keypad.h
 *
 *    Description: Versão da biblioteca Keypad do arduino para computadores
 *                  para simular a entrada de botões no teclado.
 *
 *        Version:  1.0
 *        Created:  06/06/17 19:26:34
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucas Vitalino Severo Pais (ls), lucas.severo@aluno.ufabc.edu.br
 *        Company:  Universidade Federal do ABC
 *
 * =====================================================================================
 */

#include <SDL/SDL.h>
#include <thread>
#include <chrono>

#define MAPSIZE 10 // MAPSIZE is the number of rows (times 16 columns)
//userKeymap was fixed with 4 columns
const int NUM_COLS=4;
#define makeKeymap(x) (x) //just to make it compatible with the arduino version

struct Keypad {
    std::thread *t;
    unsigned int bitMap[MAPSIZE];// 10 row x 16 column array of bits. Except Due which has 32 columns.
    char **keymap;
    char numCols;
    char numRows;
    SDL_Event e;
    //bool game_running = true;

    void getKeys();
    char getKey();
    Keypad(char[][NUM_COLS],char*,char*,const char &, const char &);
};

void read_keys_loop(Keypad *);
