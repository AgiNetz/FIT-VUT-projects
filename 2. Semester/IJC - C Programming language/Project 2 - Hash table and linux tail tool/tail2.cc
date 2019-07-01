/*
Nazev: tail.cc
Reseni IJC-DU2, uloha 1b, 22.04.2017
Autor: Tomas Danis, 1r FIT, xdanis05@stud.fit.vutbr.cz
Prelozeno: g++ 5.4.0
Popis: Implementace POSIX programu tail. Podporuje vstup ze souboru i stdin. Je mozne zadat
argument -n, kterym se specifikuje pocet radku, ktere vypsat.
*/

#include <iostream>
#include <string>
#include <cstdlib>
#include <queue>
#include <fstream>

using namespace std;

/**
 * @brief Varianta programu tail pro cteni ze souboru
 *
 * Vypise poslednich n radku ze zadaneho souboru.
 *
 * @param filename - Jmeno vstupniho souboru
 * @param n - Pocet radku na vypis
 * @return Vraci 0 pri uspechu, <0 pri selhani
 */
int tail_file(const string filename, unsigned int n)
{
    ifstream myFile;
    myFile.open(filename.c_str(), ios_base::in);
    if(!myFile.is_open())
    {
        cerr << "Nepovedlo se otevrit soubor " << filename << "!" << endl;
        return -2;
    }
	if(n == 0)
	{
		myFile.close();
		return 0;
	}
    queue<string> buffer;
    string tmp;
    while(getline(myFile, tmp))
    {
        if(buffer.size() == n)
        {
            buffer.pop();
        }
        buffer.push(tmp);
    }
    while(!buffer.empty())
    {
        cout << buffer.front() << endl;
        buffer.pop();
    }
    myFile.close();
    return 0;
}

/**
 * @brief Varianta programu tail pro cteni z stdin
 *
 * Vypise poslednich n radku ze zadaneho souboru. Delka radku je neomezena
 *
 * @param n - Pocet radku na vypis
 * @return Vraci 0 pri uspechu, <0 pri selhani
 */
int tail_std(unsigned int n)
{
	if(n == 0)
		return 0;
    queue<string> buffer;
    string tmp;
    while(getline(cin, tmp))
    {
        if(buffer.size() == n)
        {
            buffer.pop();
        }
        buffer.push(tmp);
    }
    while(!buffer.empty())
    {
        cout << buffer.front() << endl;
        buffer.pop();
    }
    return 0;
}

/**
 * @brief Funkce na zpracovani argumentu programu
 *
 * Zpracuje nasledujici argument z konzolove radky, vraci pocet nactenych argumentu, urceno pro volani v cyklu while
 *
 * @param argc - Pocet argumentu programu
 * @param argv - Pole obsahujici jednotlive argumenty programu
 * @param arg - Parameter, do ktereho se ulozi naposled zpracovany argument z konzolove radky
 * @param argpar - parameter k naposled zpracovanemu argumentu, pokud existuje. Pri chybe "?"
 * @return Vraci pocet nactenych argumentu
 */

int procArg(int argc, char* argv[], string* arg, string* argpar)
{
    static int argNum = 0;
    argNum++;
    if(argc - 1 < argNum)
        return 0;
    *arg = argv[argNum];
    if((*arg).compare("-n") == 0)
    {
        if(argNum == argc - 1)
        {
            *argpar = "?"; //chyba
            return 1;
        }
		argNum++;
        *argpar = argv[argNum];
        return 2;
    }
    return 1;
}

int main(int argc, char* argv[])
{
    std::ios::sync_with_stdio(false);
    string arg;
    string parameter;
    unsigned int n = 10;
    string fileName = "";
    bool fileSet = false;
	bool nSet = false;
    while(procArg(argc, argv, &arg, &parameter))
    {
        if(arg.compare("-n") == 0)
        {
			if(nSet)
			{
				cerr << "Parametr n byl zadan vicekrat!" << endl;
				return -1;
			}
            size_t err = 0;
            n = std::stoi(parameter, &err, 10);
            if(err != parameter.size())
            {
                cerr << "Nastala chyba pri nacitavani argumentu!" << endl;
                return -1;
            }
			nSet = true;
        }
        else
        {
            if(!fileSet)
            {
                fileName = arg;
                fileSet = true;
            }
            else
            {
                cerr << "Bylo zadano vice jmen souboru!" << endl;
                return -1;
            }
        }
    }

    if(fileSet)
        return tail_file(fileName, n);
    return tail_std(n);
}
