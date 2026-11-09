#include <iostream>
using namespace std;

double hitungTotalNilai(double arr[], int n) { // Ubah double n menjadi int n
    if (n <= 0) {
        return 0;
    }
    return arr[n - 1] + hitungTotalNilai(arr, n - 1);
}

int main() {
    int banyakData;

    cout << "Masukan jumlah Data yang dibutuhkan: ";
    cin >> banyakData;

    while (banyakData <= 0) {
        cout << "Banyak Data tidak boleh 0 atau mines!" << "\n";
        cout << "Silahkan masukan ulang jumlah Data: ";
        cin >> banyakData;    
    }

    double* data = new double [banyakData];

    for (int i = 0; i < banyakData; i++) {
        cout << "Data ke " << i + 1 << ": ";
        cin >> data[i];
    }

    double batasAtas;
    cout << "Masukan Batas Atas: ";
    cin >> batasAtas;

    double total = hitungTotalNilai(data, banyakData);
    double rataRata = total / banyakData;
    double min = data[0];
    double max = data[0];

    for (int i = 1; i < banyakData; i++) {
        if (data[i] > max) {
            max = data[i];
        }
        if (data[i] < min) {
            min = data[i];
        }
    }

    cout << "\n--- HASIL ---\n";
    cout << "N: " << banyakData << "\n";
    cout << "Data: ";
    for (int i = 0; i < banyakData; i++) {
        cout << data[i] << " ";
    }
    cout << "\n";
    cout << "Batas atas: " << batasAtas << "\n";
    cout << "Rata-rata: " << rataRata << "\n";
    cout << "Min: " << min << ", Max: " << max << "\n";
    cout << "Melebihi batas pada indeks: ";
    
    bool first = true;
    for (int i = 0; i < banyakData; i++) { // Diubah dari N menjadi banyakData
        if (data[i] > batasAtas) {
            if (!first) {
                cout << ", ";
            }
            cout << i;
            first = false;
        }
    }
    cout << "\n";

    delete[] data; // Membersihkan alokasi memori dinamis
    return 0;
}