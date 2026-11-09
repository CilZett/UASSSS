#include <iostream>
#include <iomanip>
using namespace std;

void printNumberTriangle(int n) {

for (int i = 1; i <= n; i++) {

    for (int j = 1; j <= i; j++) {

    cout << '*' << " ";

}

cout << endl;

}

}

int main() {

    int size;
    cout << "Masukan Jumlah Segitiga: ";
    cin >> size;


    printNumberTriangle(size);

}