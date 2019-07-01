/*
* Tomas Danis (xdanis05)
* 2. Projekt IZP
* 2016/2017
*/

/*Direktivy preprocesoru
---------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

#define NAPOVEDA "Nespravne zadane argumenty! \n" \
                 "Syntax spusteni:  \n" \
                 "./proj2 --log X N \n" \
                 "./proj2 --pow X Y N \n" \
                 "Spusteni s argumentem --log: Vypocte logaritmus z cisla X pomoci Taylorova polynomu a zretezenych zlomku\n" \
                 "                             Argument X specifikuje argument logaritmu\n" \
                 "                             Argument N specifikuje pocet iteraci Taylorova polynomu a zretezenych zlomku\n" \
                 "Spusteni s argumentem --pow: Vypocte Ytou mocninu cisla X pomoci Taylorova polynomu\n" \
                 "                             Vypocet se vykona dva krat.\n" \
                 "                             Jednou se pri vypoctu logaritmu pouzije taylor_log, po druhe cfrag_log\n" \

/*Prototypy
---------------------------------------------------------------------------------------------------------*/

double taylor_log(double, unsigned int);
double cfrag_log(double, unsigned int);
double taylor_pow(double, double, unsigned int);
double taylorcf_pow(double, double, unsigned int);
bool zpracujArgumenty(int, char**, char*, double*, double*, long*);

/*Main
---------------------------------------------------------------------------------------------------------*/

int main(int argc, char* argv[])
{
	/**
	 * 1. bit - --log priznak
	 * 2. bit - --pow priznak
	 */
	char flags = 0;
    long pocetIteraci = 0;
    double zaklad = -1;
    double exponent = -1;
    if(!zpracujArgumenty(argc, argv, &flags, &zaklad, &exponent, &pocetIteraci))
    {
        fprintf(stderr, NAPOVEDA);
        return 1;
    }
    if(pocetIteraci <= 0)
    {
        fprintf(stderr, NAPOVEDA);
        return 1;
	}
    if(flags & 1)
    {
        printf("       log(%g) = %.12g\n", zaklad, log(zaklad));
        printf(" cfrac_log(%g) = %.12g\n", zaklad, cfrag_log(zaklad, (unsigned int)pocetIteraci));
		printf("taylor_log(%g) = %.12g\n", zaklad, taylor_log(zaklad, (unsigned int)pocetIteraci));
    }
    else
    {
		printf("         pow(%g,%g) = %.12g\n", zaklad, exponent, pow(zaklad, exponent));
        printf("  taylor_pow(%g,%g) = %.12g\n", zaklad, exponent, taylor_pow(zaklad, exponent, (unsigned int)pocetIteraci));
        printf("taylorcf_pow(%g,%g) = %.12g\n", zaklad, exponent, taylorcf_pow(zaklad, exponent, (unsigned int)pocetIteraci));
    }
}

/*Deklarace funkcí
---------------------------------------------------------------------------------------------------------*/

/**
Zjisti nastavene argumenty a zkontroluje jejich spravnost
@argc - Pocet argumentu
@argv[] - Pole ukazatelu obsahujici jednotlive argumenty
@flags - promenna, ktere jednotlive bity reprezentuji jednotlive riznaky argumentu
@zaklad - promenna, do ktere se ulozi argument logaritmu nebo zaklad exponencialne funkce
@exponent - promenna, do ktere se ulozi exponent exponencialne funkce
@pocetIteraci - promena, do ktere se ulozi pozadovany pocet iteracii pri vypoctu logaritmu nebo exponencialne funkce
Vraci datovy typ bool - znaci jestli pri spracovavani argumentu doslo ke chybe
*/
bool zpracujArgumenty(int argc, char* argv[], char* flags, double* zaklad, double* exponent, long* pocetIteraci)
{
    if((argc == 4) && (strcmp(argv[1], "--log") == 0))
    {
        char* err = NULL;
        char* err2 = NULL; //err12 ukazuji v pripade chyby na prvni znak ktery se strtod/l nepovedlo provest na cislo
        *zaklad = strtod(argv[2], &err);
        *pocetIteraci = strtol(argv[3], &err2, 10);
        if((strcmp(err, "")) || strcmp(err2, "")) //v pripade ze nektere z err promennych obsahuje nejake znaky skoncit - chybne argumenty
            return false;
        *flags |= 1 << 0;
    }
    else if((argc == 5) && (strcmp(argv[1], "--pow") == 0))
    {
        char* err = NULL;
        char* err2 = NULL;
        char* err3 = NULL; //err123 ukazuji v pripade chyby na prvni znak ktery se strtod/l nepovedlo provest na cislo
        *zaklad = strtod(argv[2], &err);
        *exponent = strtod(argv[3], &err2);
        *pocetIteraci = strtol(argv[4], &err3, 10);
        if((strcmp(err, "")) || strcmp(err2, "") || strcmp(err3, "")) //v pripade ze nektere z err promennych obsahuje nejake znaky skoncit - chybne argumenty
            return false;
        *flags |= 1 << 1;
    }
    else
        return false; // jiny variace argumenty jsou spatne
    return true;
}

/**
Vypocte logaritmus ze zadaneho cisla pomoci Taylorova polynomu se zadanym poctem iteraci
@x - argument logaritmu
@n - pocet iteraci Taylorova polynomu
Vraci datovy typ double - logaritmus ze zadaneho cisla
*/
double taylor_log(double x, unsigned int n)
{
	if(x == 0)
		return -INFINITY;
	if(x < 0 || isnan(x))
		return NAN;
    if(isinf(x))
        return INFINITY;
    double res = 0;
    if(x < 1) //prvni variace Taylorova polynomu
    {
        double pom = 1 - x;
        double sucasnyClen = res = -pom;
        for(unsigned int i = 1; i < n; i++)
        {
            sucasnyClen *= i*pom/(i+1);
            res += sucasnyClen;
        }
    }
    else // druha variace Taylorova polynomu
    {
        double sucasnyClen = res = (x-1)/(x);
        for(unsigned int i = 1; i < n; i++)
        {
            sucasnyClen *= (i*(x-1)/(x))/(i + 1);
            res += sucasnyClen;
        }
    }
    return res;
}

/**
Vypocte logaritmus ze zadaneho cisla pomoci zretezenych zlomku
@x - argument logaritmu
@n - udava rozvoj zretezenho zlomku
Vraci datovy typ double - logaritmus ze zadaneho cisla
*/
double cfrag_log(double x, unsigned int n)
{
	if(x == 0)
		return -INFINITY;
	if(x < 0 || isnan(x))
		return NAN;
    if(isinf(x))
        return INFINITY;
    double res = 0;
    double z = (x - 1)/(x + 1);
    while(n > 1)
    {
        res = (z*z*(n-1)*(n-1))/(2*n - 1 - res);
        n--;
    }
    res = 2*z/(2*n - 1 - res);
    return res;
}

/**
Vypocte y-tou mocninu cisla x pomoci Taylorova polynomu, logaritmus pri vypoctu se pocita pomoci Taylorova polynomu
@x - zaklad mocniny
@y - exponent mocniny
@n - pocet clenu Taylorova polynomu
Vraci datovy typ double - cislo x umocnene na ytou
*/
double taylor_pow(double x, double y, unsigned int n)
{
	if(isnan(x) || isnan(y))
		return NAN;
    if(y == 0 || x == 1)
        return 1.0;
    if(x <= 0)
        return NAN;
    if(isinf(x) || isinf(y))
    {
        if(y > 0)
            return INFINITY;
        return 0;
    }
    bool neg = false;
    if(y < 0)
    {
        neg = true;
        y = -y;
    }
    double res = 1;
    double clen = 1;
    for(unsigned int i = 0; i < n; i++)
    {
        clen = clen*y*taylor_log(x, n)/(i+1);
        res += clen;
    }
    if(neg)
        res = 1/res;
    return res;
}

/**
Vypocte y-tou mocninu cisla x, logaritmus pri vypoctu se pocita pomoci zretezenych zlomku
@x - zaklad mocniny
@y - exponent mocniny
@n - pocet clenu Taylorova polynomu
Vraci datovy typ double - cislo x umocnene na ytou
*/
double taylorcf_pow(double x, double y, unsigned int n)
{
	if(isnan(x) || isnan(y))
		return NAN;
    if(y == 0 || x == 1)
        return 1.0;
    if(x <= 0)
        return NAN;
    if(isinf(x) || isinf(y))
    {
        if(y > 0)
            return INFINITY;
        return 0;
    }
    bool neg = false;
    if(y < 0)
    {
        neg = true;
        y = -y;
    }
    double res = 1;
    double clen = 1;
    for(unsigned int i = 0; i < n; i++)
    {
        clen = clen*y*cfrag_log(x, n)/(i+1);
        res += clen;
    }
    if(neg)
        res = 1/res;
    return res;
}

