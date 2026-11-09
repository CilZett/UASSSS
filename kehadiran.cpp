#include <iostream>
using namespace std;

int main() {
    
    float hadir;
    float pertemuan = 5;
    float persentase;

    cout << "=== Kehadiran Mahasiswa ===\n";
    cout << "Jumlah Kehadiran: ";
    cin >> hadir;

    while (hadir > 5 || hadir < 1) {
        cout << "Data Tidak Valid!\n";
         cout << "Masukan Ulang Jumlah Kehadiran: ";
        cin >> hadir;
    }
    
    
    persentase = (hadir / pertemuan) * 100;

    cout << "Kehadiran Anda: " << persentase << "%" << "\n";

    if (persentase > 75) {
        cout << "Kategori Kehadiran: Baik" << "\n";

    }
    else if (persentase >= 50) {
        cout << "Kategori Kehadiran: Cukup" << "\n";

    }
    else if (persentase < 50) {
        cout << "Kategori Kehadiran: Kurang" << "\n";

    }
    else {
        cout << "Data Tidak Valid!";
    }
    return 0;
}