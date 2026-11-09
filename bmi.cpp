#include <iostream>
using namespace std;

int main() {
    
    int Weight;
    int Height;
    int bmi;

    cout << " === Kalkulator BMI === \n";
    cout << "Masukan Berat Badan: ";
    cin >> Weight;

    cout << "Masukan Tinggi Badan: ";
    cin >> Height;

    Height=Height / 100;

    bmi=Weight / (Height * Height);

    cout << "BMI Anda: " << bmi << "\n";

    if (bmi < 18.5) {
        cout << "Kategori Anda: ";
        
    }
}