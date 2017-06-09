/*
 * =====================================================================================
 *
 *       Filename:  Keypad.cpp
 *
 *    Description:  Versão da biblioteca Keypad do arduino para computadores
 *                  para simular a entrada de botões no teclado.
 *
 *        Version:  1.0
 *        Created:  06/06/17 19:25:16
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucas Vitalino Severo Pais (ls), lucas.severo@aluno.ufabc.edu.br
 *        Company:  Universidade Federal do ABC
 *
 * =====================================================================================
 */

#include <cstdlib>
#include "Keypad.h"
#include "Globals.h"

//userKeymap was fixed with 4 colums

Keypad::Keypad(char userKeymap[][4],char row[],char col[], const char & numRows, const char & numCols) {
    this->numRows = numRows;
    this->numCols = numCols;

    this->keymap = new char*[numRows];
    for(int i = 0;i<numCols;i++){
        this->keymap[i] = new char[numCols];
    }

    memset(this->bitMap,0,MAPSIZE*sizeof(unsigned int));

    this->t = new std::thread(read_keys_loop,this);
    for(int i=0;i<this->numRows;i++){
        for(int j=0;j<this->numCols;j++){
            printf("i:%d j:%d %c\n",i,j,userKeymap[i][j]);
            this->keymap[i][j]=userKeymap[i][j];
        }
    }
}

//implemented to be compatible with the arduino version
void Keypad::getKeys(){
}

//implemented to be compatible with the arduino version
char Keypad::getKey(){
    return '0';
}

void read_keys_loop(Keypad *k){
    while(true){
        if(SDL_PollEvent(&k->e)){

            if(k->e.type == SDL_QUIT){
                Globals::instance().thread_do_exit=1;
            }

            if(k->e.type == SDL_KEYDOWN){

                for(int i=0;i<k->numRows;i++){
                    for(int j=0;j<k->numCols;j++){
                        if(k->e.key.keysym.sym==k->keymap[i][j]){
                            printf("i:%d j:%d PRESSED KEY %c\n",i,j,k->keymap[i][j]);
                            k->bitMap[i] |= 1<<j;//set bit
                        }
                    }
                }

            }

            if(k->e.type == SDL_KEYUP){

                for(int i=0;i<k->numRows;i++){
                    for(int j=0;j<k->numCols;j++){
                        if(k->e.key.keysym.sym==k->keymap[i][j]){
                            printf("i:%d j:%d RELEASED KEY %c\n",i,j,k->keymap[i][j]);
                            k->bitMap[i] &= ~(1<<j);//clear bit
                        }
                    }
                }
            }

        }
        std::chrono::milliseconds sleepDuration(10);
        std::this_thread::sleep_for(sleepDuration);
    }

}
