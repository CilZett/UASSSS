#include <iostream>
#include <string>
#include <conio.h>
using namespace std;

int main() {

    char jagoan[10]; 
    char *pjagoan = jagoan;

    cout << "Siapa jagoan mu: ";
    cin >> jagoan;


    cout << "jagoan = " << jagoan << "\n";
    cout << "pjagoan = " << pjagoan << "\n";

    
    pjagoan++; //LEGAL

    cout << "pjagoan = " << pjagoan << "\n";



}
