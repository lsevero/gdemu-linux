/*
 * =====================================================================================
 *
 *       Filename:  Globals.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/09/17 00:53:14
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucas Vitalino Severo Pais (ls), lucas.severo@aluno.ufabc.edu.br
 *        Company:  Universidade Federal do ABC
 *
 * =====================================================================================
 */

#pragma once

//singleton that holds global variables
//needed to hold the thread flags to exit the main thread from a Keypad instance
class Globals final
{
    public:
        static Globals& instance() {
            // Guaranteed to be destroyed.
            // Instantiated on first use.
            static Globals INSTANCE;
            return INSTANCE;
        }
        char thread_do_exit = 0;
        Globals(Globals const&)               = delete;
        void operator=(Globals const&)  = delete;

    private:
        Globals() {}

};
