#ifndef PRINTDISPLAY_H
#define PRINTDISPLAY_H
enum State {scan, search};

/*
*Printet das Menüfeld der Menufunktion aus.
*Diese Methode wird ausgeführt wen der "C-Knopf" gedrückt wird und der Zustand >nicht< auf "Menu" ist.
*/
void printMenu();
/*
*Printet das Menüfeld der Searchfunktion aus.
*Diese Methode wird ausgeführt wen der "A-Knopf" gedrückt wird und der Zustand >nicht< auf "search" ist.
*/
void printSearch();
/*
*Printet das Menüfeld der Scanfunktion aus.
*Diese Methode wird ausgeführt wen der "B-Knopf" gedrückt wird und der Zustand >nicht< auf "scan" ist.
*/
void printScan();

#endif