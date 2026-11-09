#include <iostream>
#include <string>
using namespace std;

int main() {
    // 1. Definisikan struct waktu_panen
    struct waktu_panen {
        int tanggal;
        string bulan;
    };

    // 2. Definisikan struct buah
    struct buah {
        string name;
        float berat;
        string warna;
        waktu_panen panen; // Struct di dalam struct
    };

    // 3. Buat objek 'b' dari struct buah
    buah b;

    cout << "=== MASUKKAN DATA BUAH ===" << endl;
    
    cout << "Masukkan nama buah: ";
    cin >> b.name; // Input nama buah

    cout << "Masukkan berat buah (kg): ";
    cin >> b.berat; // Input berat

    // Tambahkan cin.ignore() di sini untuk membuang sisa enter (\n)
    cin.ignore(); 

    cout << "Masukkan warna buah: ";
    getline(cin, b.warna); // Sekarang getline bisa berjalan normal dengan aman

    cout << "Masukkan tanggal panen (angka): ";
    cin >> b.panen.tanggal;

    cout << "Masukkan bulan panen: ";
    cin >> b.panen.bulan; // Input bulan di dalam struct nested


    // 4. Tampilkan kembali data yang sudah di-input
    cout << "\n=== DATA BUAH YANG DI-INPUT ===" << endl;
    cout << "Nama Buah   : " << b.name << endl;
    cout << "Berat       : " << b.berat << " kg" << endl;
    cout << "Warna       : " << b.warna << endl;
    cout << "Waktu Panen : " << b.panen.tanggal << "/" << b.panen.bulan << endl;

    return 0;
}