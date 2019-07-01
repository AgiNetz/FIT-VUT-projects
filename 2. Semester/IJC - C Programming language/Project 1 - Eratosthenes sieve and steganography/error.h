// error.h
// Reseni IJC-DU1, priklad b), 24.3.2017
// Autor: Tomas Danis, 1r FIT, xdanis05@stud.fit.vutbr.cz
// Pøeloženo: gcc 5.4.0
// Popis: Hlavickovy soubor pro modul error.c obsahujici prototypy funkcii

#ifndef ERROR_H_INCLUDED
#define ERROR_H_INCLUDED

void warning_msg(const char *fmt, ...);
void error_msg(const char *fmt, ...);

#endif // ERROR_H_INCLUDED
