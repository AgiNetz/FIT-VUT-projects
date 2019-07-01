/*
* Tomas Danis (xdanis05)
* 1. Projekt IZP
* 2016/2017
*/
/*Direktivy preprocesoru
---------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

#define NAPOVEDA "Nespravne zadane argumenty! \n" \
                 "Syntax spusteni:  \n" \
                 "./proj1 [-s M] [-n N] \n" \
                 "./proj1 -x \n" \
                 "./proj1 -S N \n" \
                 "./proj1 -r \n" \
                 "Spusteni bez argumentu: Prepise textovy vstup do hexadecimalni podoby\n" \
                 "                        Argument -s specifikuje adresu na ktere ma vypis zacit\n" \
                 "                        Argument -n specifikuje maximalni pocet bajtu na spracovani\n" \
                 "Spusteni s argumentem -x: Prepise textovy vstup do hexadecimalni podoby(bez formatovani)\n" \
                 "Spusteni s argumentem -S: Tisknou se jen retezce nalezene ve vstupu.\n" \
                 "                          Retezec je posloupnost znaku obsahujici jen tisknutelne nebo prazdne znaky\n" \
                 "                          Argument N specifikuje minimalni pocet znaku, ktere musi retezec obsahovat\n" \
                 "Spusteni s argumentem -r: Prepise hexadecimalni vstup do textove podoby\n" \

/*Prototypy
---------------------------------------------------------------------------------------------------------*/
bool zpracujArgumenty(int, char**, char*, int*, int*);
int funkceSN(int, int);
int funkceX();
int funkceS(int);
int funkceR();
int strcmp(const char*, const char*);
bool jeKladneCislo(const char*);
int stoi(const char*);

/*Main
---------------------------------------------------------------------------------------------------------*/

int main(int argc, char* argv[])
{
    /**
    1. bit: -s priznak
    2. bit: -n priznak
    3. bit: -x priznak
    4. bit: -S priznak
    5. bit: -r priznak
    */
    char flags = 0;
    int sNum = 0; //specifikace k argumentu -s
    int nNum = -1; //specifikace k argumentu -n
    /**
    0 - Program probehl uspesne
    1 - Argument mimo povolenzch hranic
    2 - Spatny vstup
    */
    int chybovyKod = 0; //vysledek programu
    if(!zpracujArgumenty(argc, argv, &flags, &sNum, &nNum))
    {
        fprintf(stderr, NAPOVEDA);
        return 0;
    }
    if(flags & (1 << 3)) //je nastaven argument -x
    {
        chybovyKod = funkceX();
    }
    else if(flags & (1 << 4)) //je nastaven argument -S
    {
        chybovyKod = funkceS(stoi(argv[2]));
    }
    else if(flags & (1 << 5)) //je nastaven argument -r
    {
        chybovyKod = funkceR();
    }
    else //je nastavena libovolna kombinace argumentu -s a -n
    {
        chybovyKod = funkceSN(sNum, nNum);
    }

    switch(chybovyKod)
    {
    case 1:
        fprintf(stderr,"Chyba! Argument je mimo povolenych hranic!\n");
        return 1;
    case 2:
        fprintf(stderr,"Chyba! Vstup programu ma spatny format!\n");
        return 2;
    }
    return 0;
}

/*Deklarace funkci
---------------------------------------------------------------------------------------------------------*/

/**
Zjisti nastavene argumenty a zkontroluje jejich spravnost
@argc - Pocet argumentu
@argv[] - Pole ukazatelu obsahujici jednotlive argumenty
@flags - promenna, ktere jednotlive bity reprezentuji jednotlive příznaky argumentu
@sNum - promenna, do ktere se ulozi ciselny argument, ktery je soucasti -s
@nNum - promenna, do ktere se ulozi ciselny argument, ktery je soucasti -n
Vraci datovy typ bool - znaci jestli pri spracovavani argumentu doslo ke chybe
*/
bool zpracujArgumenty(int argc, char* argv[], char* flags, int* sNum, int* nNum)
{
    for(int i = 1; i < argc; i++)
    {
        if(strcmp(argv[i],"-s") == 0)
        {
            if((argc > i + 1) && (jeKladneCislo(argv[i + 1])))
            {
                *sNum = stoi(argv[i + 1]);
                *flags |= 1; // flags | 0000 0001 - nastavi prvni bit zprava na 1
                i++; //cislo za -s uz je spracovane
            }
            else
                return false;
        }
        else if(strcmp(argv[i],"-n") == 0)
        {
            if((argc > i + 1) && (jeKladneCislo(argv[i + 1])))
            {
                *nNum = stoi(argv[i + 1]);
                *flags |= 1 << 2; // flags | 0000 0010 - nastavi druhy bit zprava na 1
                i++; //cislo za -n uz je spracovane
            }
            else
                return false;
        }
        else if(strcmp(argv[i],"-x") == 0)
        {
            if(argc == 2)
            {
                *flags |= 1 << 3; // flags | 0000 0100 - nastavi treti bit zprava na 1
                return true;
            }
            else
                return false;
        }
        else if(strcmp(argv[i],"-S") == 0)
        {
            if((argc == 3) && (jeKladneCislo(argv[2])))
            {
                *flags |= 1 << 4; // flags | 0000 1000 - nastavi ctvrty bit zprava na 1
                return true;
            }
            else
                return false;
        }
        else if(strcmp(argv[i],"-r") == 0)
        {
            if(argc == 2)
            {
                *flags |= 1 << 5; // flags | 0001 0000 - nastavi paty bit zprava na 1
                return true;
            }
            else
                return false;
        }
        else
            return false;
    }
    return true;
}

/**
Vykona se, kdyz je program spusten bez argumentu, nebo s argumentama -s nebo -n
@sNum - ciselny argument soucasti argumentu -s
@nNum - ciselny argument soucasti argumentu -n
Vraci datovy typ int - chybovy kod znacici uspesnost/neuspesnost vykonani funkce
*/
int funkceSN(int sNum, int nNum)
{
    int c;
    int adresa = 0; //adresa aktualniho bajtu
    int spracovanychPrvkov = 0; //pro kontrolu s nNum
    char buffer[17]; // pole, do kteryho budem ukladat znaky na výpis
    for(int i = 0; i < 16; i++)
    {
        buffer[i] = ' ';
    }
    buffer[16] = '\0';
    while((c = getchar()) != EOF)
    {
        if(adresa < sNum) //dokud sme spracovali mene prvku nez sNum
        {
            adresa++;
            continue;
        }
        buffer[(adresa-sNum)%16] = c;
        adresa++;
        spracovanychPrvkov++;
        if(nNum != -1 && spracovanychPrvkov == nNum) //kdyz sme spracovali nNum prvku
            break;
        if((adresa-sNum) % 16 == 0) //kdyz sme spracovali 16 prvku od posledniho vypisu
        {
            printf("%08x  ", adresa - 16); //vypis adresy
            for(int i = 0; i < 16; i++)
            {
                if(i == 8)
                    printf(" ");
                printf("%02x ", buffer[i]); //vypis 2 znaku + mezery
                if(!isprint(buffer[i]))
                    buffer[i] = '.';
            }
            printf(" |%s|\n", buffer);
            for(int i = 0; i < 16; i++)
            {
                buffer[i] = ' '; //reset pole na mezery - v pripade jen castecneho zaplneni se vypisou mezery
            }
        }
    }
    if((adresa - sNum) > 0) //vypisujeme adresu a znaky jen kdyz je co vypisovat
    {
        /*
        Vypis posledniho radku
        */
        printf("%08x  ", adresa - (adresa - sNum)%16);
        for(int i = 0; i < 16; i++)
        {
            if(i == 8)
                printf(" ");
            if(i < (adresa-sNum)%16) //kontrola, aby sme vypsali jen tolik znaku, kolik je nactenych
                printf("%02x ", buffer[i]);
            else
                printf("   "); //dorovnani mezerami namisto znaku
            if(!isprint(buffer[i]))
                buffer[i] = '.';
        }
        printf(" |%s|\n", buffer);
    }
    return 0;
}

/**
Vykona se, kdyz je program spusten s argumentem -x
Vraci datovy typ int - chybový kod znacici uspesnost/neuspesnost vykonani funkce
*/
int funkceX()
{
    int c;
    while((c = getchar()) != EOF)
        printf("%02x", c);
    printf("\n");
    /*
        char hexChars[] = {'0' , '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
        int c;
        while((c = getchar()) != EOF)
            printf("%c%c", hexChars[c/16], hexChars[c%16]);
    */
    return 0;
}

/**
Vykona se, kdyz je program spusten s argumentem -S
@minPocet - minimalni pocet znak, ktere musi retezec obsahovat, aby byl vypsan
Vraci datovy typ int - chybovy kod znacici uspesnost/neuspesnost vykonani funkce
*/
int funkceS(int minPocet)
{
    if(minPocet <= 0 && minPocet >= 200) //kontrola spravnosti minPocet
        return 1;
    char buffer[minPocet + 1]; //sem se ukladaji retezce, ktere jeste nevime, jestli vypsat
    buffer[minPocet] = '\0';
    int c;
    int sucasnyPocet = 0; //aktualni pocet znaku v retezci
    while((c = getchar()) != EOF)
    {
        sucasnyPocet++;
        if((!isprint(c)) && (!isblank(c))) //pri nalezeni znaku, ktery nemuze byt v retezci, vynulujeme pocitadlo a vypiseme \n
        {
            if(sucasnyPocet > minPocet)
                printf("\n");
            sucasnyPocet = 0;
        }
        else if(sucasnyPocet == minPocet) //vypiseme ulozene znaky pri dosazeni minimalniho poctu znaku retezce
        {
            buffer[sucasnyPocet-1] = c;
            printf("%s", buffer);
        }
        else if(sucasnyPocet < minPocet) //jestli jeste neni dosazeny minimalni pocet, znak jen docasne ulozime
        {
            buffer[sucasnyPocet-1] = c;
        }
        else //po dosazeni minimalniho poctu jen vypisujeme znaky
        {
            printf("%c", c);
        }
    }
    if(sucasnyPocet > minPocet)
        printf("\n");
    return 0;
}

/**
Vykona se, kdyz je program spusten s argumentem -r
Vraci datovy typ int - chybovy kod znacici uspesnost/neuspesnost vykonani funkce
*/
int funkceR()
{
    int oneChar[2] = {-1, -1}; //na jeden znak potrebujeme nacist 2 hex cislice
    int counter = 0;
    while((oneChar[counter] = getchar()) != EOF)
    {
        if(isspace(oneChar[counter])) //ignoruje prazne znaky ve vstupu
            continue;
        oneChar[counter] = toupper(oneChar[counter]); //Pro lepsi konverzi a,b,c,d,e,f na cisla
        if((!isdigit(oneChar[counter])) && (oneChar[counter] < 48 || oneChar[counter] > 57)) //vstup musi byt hexa cislice
            return 2;
        if(oneChar[counter] >= 65)
        {
            oneChar[counter] -= 55; //A = 65, B = 66....
        }
        else
        {
            oneChar[counter] -= '0';
        }
        if(counter == 1) //vypisujeme jednom kdyz sme nacetli 2 hex cislice
        {
            printf("%c", (oneChar[0]*16 + oneChar[1]));
            counter = 0;
            continue;
        }
        counter++;
    }
    if(oneChar[0] != -1) //kdyz byl pocet cislic neparny
        printf("%c", oneChar[0]);
    return 0;
}

/**
Porovna dva zadane retezce
@arg1 - Prvy retezec k porovnani
@arg2 - Druhy retezec k porovnani
Vraci datovy typ int:
0 - pokud se retezce rovnaji
-1 - pokud ma prvni neshodny znak vetsi ASCII hodnotu v arg2
1 - pokud ma prvni neshodny znak vetsi ASCII hodnotu v arg1
*/
int strcmp(const char* arg1, const char* arg2)
{
    int i = 0;
    for(; arg1[i] == arg2[i]; i++)
    {
        if(arg1[i] == '\0')
            return 0;
    }
    return ((arg1[i] < arg2[i]) ? -1 : 1);
}

/**
Zjisti, jestli je zadany retezec kladne cele cislo
@s - retezec k otestovani
Vraci datovy typ bool - odpoved jestli je zadany retezec kladne cele cislo
*/
bool jeKladneCislo(const char* s)
{
    for(int i = 0; (((s[i]-'0') >= 0) && (s[i]-'0') <= 9); i++) //Kontroluje, jestli dany znak je cislice
    {
        if(s[i+1] == '\0') //Dorazili sme na konec?
        {
            return true;
        }
    }
    return false;
}

/**
Prekonvertuje zadany retezec na kladne cele cislo
@s - retezec k prekonvertovani
Vraci datovy typ int - ciselnou reprezentaci zadaneho retezce
NEKONTROLUJE VALIDITU ARGUMENTU
*/
int stoi(const char* s)
{
    int result = 0;
    for(int i = 0; s[i] != '\0'; i++)
    {
        /*
        Mezivysledek je vzdy vynasoben 10, takze na miste jednotek vznikne 0.
        Na tu se doplni ciselna hodnota prave zpracovavane cislice.
        Jelikoz se slo od nejvyssiho radu, ve vysledku maj vsechny cislice spravny rad
        */
        result = result*10 + (s[i] - '0');
    }
    return result;
}
