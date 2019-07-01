
/* c206.c **********************************************************}
{* Téma: Dvousměrně vázaný lineární seznam
**
**                   Návrh a referenční implementace: Bohuslav Křena, říjen 2001
**                            Přepracované do jazyka C: Martin Tuček, říjen 2004
**                                            Úpravy: Kamil Jeřábek, říjen 2017
**
** Implementujte abstraktní datový typ dvousměrně vázaný lineární seznam.
** Užitečným obsahem prvku seznamu je hodnota typu int.
** Seznam bude jako datová abstrakce reprezentován proměnnou
** typu tDLList (DL znamená Double-Linked a slouží pro odlišení
** jmen konstant, typů a funkcí od jmen u jednosměrně vázaného lineárního
** seznamu). Definici konstant a typů naleznete v hlavičkovém souboru c206.h.
**
** Vaším úkolem je implementovat následující operace, které spolu
** s výše uvedenou datovou částí abstrakce tvoří abstraktní datový typ
** obousměrně vázaný lineární seznam:
**
**      DLInitList ...... inicializace seznamu před prvním použitím,
**      DLDisposeList ... zrušení všech prvků seznamu,
**      DLInsertFirst ... vložení prvku na začátek seznamu,
**      DLInsertLast .... vložení prvku na konec seznamu,
**      DLFirst ......... nastavení aktivity na první prvek,
**      DLLast .......... nastavení aktivity na poslední prvek,
**      DLCopyFirst ..... vrací hodnotu prvního prvku,
**      DLCopyLast ...... vrací hodnotu posledního prvku,
**      DLDeleteFirst ... zruší první prvek seznamu,
**      DLDeleteLast .... zruší poslední prvek seznamu,
**      DLPostDelete .... ruší prvek za aktivním prvkem,
**      DLPreDelete ..... ruší prvek před aktivním prvkem,
**      DLPostInsert .... vloží nový prvek za aktivní prvek seznamu,
**      DLPreInsert ..... vloží nový prvek před aktivní prvek seznamu,
**      DLCopy .......... vrací hodnotu aktivního prvku,
**      DLActualize ..... přepíše obsah aktivního prvku novou hodnotou,
**      DLSucc .......... posune aktivitu na další prvek seznamu,
**      DLPred .......... posune aktivitu na předchozí prvek seznamu,
**      DLActive ........ zjišťuje aktivitu seznamu.
**
** Při implementaci jednotlivých funkcí nevolejte žádnou z funkcí
** implementovaných v rámci tohoto příkladu, není-li u funkce
** explicitně uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam
** předá někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**/

#include "c206.h"

int solved;
int errflg;

void DLError() {
/*
** Vytiskne upozornění na to, že došlo k chybě.
** Tato funkce bude volána z některých dále implementovaných operací.
**/
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;             /* globální proměnná -- příznak ošetření chyby */
    return;
}

void DLInitList (tDLList *L) {
/*
** Provede inicializaci seznamu L před jeho prvním použitím (tzn. žádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádět nad již inicializovaným
** seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
** že neinicializované proměnné mají nedefinovanou hodnotu.
**/
	L->First = NULL;
    L->Act = NULL;
    L->Last = NULL;
}

void DLDisposeList (tDLList *L) {
/*
** Zruší všechny prvky seznamu L a uvede seznam do stavu, v jakém
** se nacházel po inicializaci. Rušené prvky seznamu budou korektně
** uvolněny voláním operace free.
**/
	tDLElemPtr iterator = L->First;
	tDLElemPtr remove = L->First;
	while(iterator != NULL)
	{
		remove = iterator;
		iterator = iterator->rptr;
		free(remove);
	}
	L->Act = NULL;
	L->First = NULL;
	L->Last = NULL;
}

void DLInsertFirst (tDLList *L, int val) {
/*
** Vloží nový prvek na začátek seznamu L.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
	tDLElemPtr newElem = (tDLElemPtr)malloc(sizeof(struct tDLElem));
	if(newElem == NULL)
	{
		DLError();
		return;
	}
	newElem->data = val;
	newElem->rptr = L->First;
	newElem->lptr = NULL;
	if(L->First != NULL) //Je seznam neprazdny?
        L->First->lptr = newElem;
    else
        L->Last = newElem; //V prazdnem seznamu je prvy prvek je zaroven i posledny
    L->First = newElem;
}

void DLInsertLast(tDLList *L, int val) {
/*
** Vloží nový prvek na konec seznamu L (symetrická operace k DLInsertFirst).
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
    tDLElemPtr newElem = (tDLElemPtr)malloc(sizeof(struct tDLElem));
    if(newElem == NULL)
    {
        DLError();
        return;
    }
    newElem->data = val;
    newElem->lptr = L->Last;
    newElem->rptr = NULL;
    if(L->First != NULL) //Je seznam neprazdny?
        L->Last->rptr = newElem;
    else
        L->First = newElem; //V prazdnem seznamu je posledni prvek je zaroven i prvy
    L->Last = newElem;
}

void DLFirst (tDLList *L) {
/*
** Nastaví aktivitu na první prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
    L->Act = L->First;
}

void DLLast (tDLList *L) {
/*
** Nastaví aktivitu na poslední prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/

    L->Act = L->Last;
}

void DLCopyFirst (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
    if(L->First == NULL)
        DLError();
    else
        *val = L->First->data;
}

void DLCopyLast (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu posledního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
        if(L->First == NULL)
        DLError();
    else
        *val = L->Last->data;
}

void DLDeleteFirst (tDLList *L) {
/*
** Zruší první prvek seznamu L. Pokud byl první prvek aktivní, aktivita
** se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/
    if(L->First != NULL) //Je seznam neprazdny?
    {
        if(L->First == L->Act) //Kontrola aktivity ruseneho prvku
            L->Act = NULL;
        if(L->First == L->Last) //Je seznam jednoprvkovy?
            L->Last = NULL;
        tDLElemPtr first = L->First;
        L->First = L->First->rptr;
        if(L->First != NULL) //Byl seznam vic nez 1 prvkovy?
            L->First->lptr = NULL;
        free(first);
    }
}

void DLDeleteLast (tDLList *L) {
/*
** Zruší poslední prvek seznamu L. Pokud byl poslední prvek aktivní,
** aktivita seznamu se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/

    if(L->First != NULL) //Je seznam neprazdny?
    {
        if(L->Last == L->Act) //Kontrola aktivity ruseneho prvku
            L->Act = NULL;
        if(L->First == L->Last) //Je seznam jednoprvkovy?
            L->First = NULL;
        tDLElemPtr last = L->Last;
        L->Last = L->Last->lptr;
        if(L->Last != NULL) //Byl seznam vic nez 1 prvkovy?
            L->Last->rptr = NULL;
        free(last);
    }
}

void DLPostDelete (tDLList *L) {
/*
** Zruší prvek seznamu L za aktivním prvkem.
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** posledním prvkem seznamu, nic se neděje.
**/
    if(L->Act != NULL && L->Act != L->Last) //je seznam aktivni a existuje za aktivnim prvkem dalsi prvek?
    {
        tDLElemPtr toRemove = L->Act->rptr;
        if(toRemove->rptr != NULL) //existuje za prvkem na odstraneni dalsi prvek?
            toRemove->rptr->lptr = L->Act;
        else
            L->Last = L->Act; //odstranujeme posledni prvek, upravime L->Last
        L->Act->rptr = toRemove->rptr;
        free(toRemove);
    }

}

void DLPreDelete (tDLList *L) {
/*
** Zruší prvek před aktivním prvkem seznamu L .
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** prvním prvkem seznamu, nic se neděje.
**/

    if(L->Act != NULL && L->Act != L->First) //je seznam aktivni a existuje pred aktivnim prvkem dalsi prvek?
    {
        tDLElemPtr toRemove = L->Act->lptr;
        if(toRemove->lptr != NULL) //existuje pred prvkem na odstraneni dalsi prvek?
            toRemove->lptr->rptr = L->Act;
        else
            L->First = L->Act; //odstranujeme prvni prvek, upravime L->First
        L->Act->lptr = toRemove->lptr;
        free(toRemove);
    }
}

void DLPostInsert (tDLList *L, int val) {
/*
** Vloží prvek za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
    if(L->Act != NULL) //je seznam aktivni?
    {
        tDLElemPtr newElem = (tDLElemPtr)malloc(sizeof(struct tDLElem));
        if(newElem == NULL)
        {
            DLError();
            return;
        }
        newElem->data = val;
        newElem->lptr = L->Act;
        newElem->rptr = L->Act->rptr;
        if(newElem->rptr != NULL) //existuje za novym prvkem dalsi prvek?
            newElem->rptr->lptr = newElem;
        else
            L->Last = newElem; //pridavame posledni prvek, upravime L->Last
        L->Act->rptr = newElem;
	}
}

void DLPreInsert (tDLList *L, int val) {
/*
** Vloží prvek před aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
    if(L->Act != NULL) //je seznam aktivni?
    {
        tDLElemPtr newElem = (tDLElemPtr)malloc(sizeof(struct tDLElem));
        if(newElem == NULL)
        {
            DLError();
            return;
        }
        newElem->data = val;
        newElem->rptr = L->Act;
        newElem->lptr = L->Act->lptr;
        if(newElem->lptr != NULL) //existuje pred novym prvkem dalsi prvek?
            newElem->lptr->rptr = newElem;
        else
            L->First = newElem; //pridavame prvni prvek, upravime L->First
        L->Act->lptr = newElem;
	}
}

void DLCopy (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam L není aktivní, volá funkci DLError ().
**/
    if(L->Act == NULL)
        DLError();
    else
        *val = L->Act->data;
}

void DLActualize (tDLList *L, int val) {
/*
** Přepíše obsah aktivního prvku seznamu L.
** Pokud seznam L není aktivní, nedělá nic.
**/
    if(L->Act != NULL)
        L->Act->data = val;
}

void DLSucc (tDLList *L) {
/*
** Posune aktivitu na následující prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
**/
    if(L->Act != NULL)
        L->Act = L->Act->rptr;
}


void DLPred (tDLList *L) {
/*
** Posune aktivitu na předchozí prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
**/
    if(L->Act != NULL)
        L->Act = L->Act->lptr;
}

int DLActive (tDLList *L) {
/*
** Je-li seznam L aktivní, vrací nenulovou hodnotu, jinak vrací 0.
** Funkci je vhodné implementovat jedním příkazem return.
**/
    return (L->Act != NULL);
}

/* Konec c206.c*/
