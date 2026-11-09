#include <iostream>
#include <limits>
using namespace std;

int main() {
    int tahun;

    cout << "=== Penghitung Tahun Kabisat ===\n";

    cout << "Masukan Tahun: ";
    cin >> tahun;

    while (!((tahun % 400 == 0) || (tahun % 4 == 0 && tahun % 100 != 0))) {
        cout << "Bukan Tahun Kabisat\n";
        cout << "Masukan Ulang Tahun: ";
        cin >> tahun;

    }
    cout << "Tahun Kabisat Coyy!";
    return 0;

}