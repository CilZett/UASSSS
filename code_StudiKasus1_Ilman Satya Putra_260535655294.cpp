#include <iostream>
using namespace std;

// Fungsi rekursif untuk menghitung total jumlah nilai
int hitungTotalNilai(int arr[], int n) {
    if (n <= 0) {
        return 0;
    }
    return arr[n - 1] + hitungTotalNilai(arr, n - 1);
}

int main() {
    int jumlahMahasiswa;
    
    cout << "Masukkan jumlah mahasiswa: ";
    cin >> jumlahMahasiswa;

    // Menangani kasus jika jumlah mahasiswa yang dimasukkan adalah 0 atau negatif
    if (jumlahMahasiswa <= 0) {
        cout << "Jumlah mahasiswa tidak boleh 0 atau negatif." << endl;
        return 0;
    }

    // Menggunakan array dinamis untuk menyimpan nilai mahasiswa
    int* nilai = new int[jumlahMahasiswa];

    cout << "Masukkan nilai mahasiswa\n";
    for (int i = 0; i < jumlahMahasiswa; i++) {
        // Validasi input: mengulangi permintaan jika nilai < 0 atau > 100
        do {
            cout << "Mahasiswa " << i + 1 << ": ";
            cin >> nilai[i];
            
            if (nilai[i] < 0 || nilai[i] > 100) {
                cout << "-> Peringatan: Nilai harus di antara 0 sampai 100! Silakan masukkan ulang.\n";
            }
        } while (nilai[i] < 0 || nilai[i] > 100);
    }

    // Mencari nilai terbesar dan terkecil dari array
    int nilaiTerbesar = nilai[0];
    int nilaiTerkecil = nilai[0];

    for (int i = 1; i < jumlahMahasiswa; i++) {
        if (nilai[i] > nilaiTerbesar) {
            nilaiTerbesar = nilai[i];
        }
        if (nilai[i] < nilaiTerkecil) {
            nilaiTerkecil = nilai[i];
        }
    }

    // Menggunakan fungsi rekursif untuk menghitung rata-rata nilai
    double total = hitungTotalNilai(nilai, jumlahMahasiswa);
    double rataRata = total / jumlahMahasiswa;

    // Menampilkan Output
    cout << "\nOutput:\n";
    cout << "Nilai terbesar: " << nilaiTerbesar << endl;
    cout << "Nilai terkecil: " << nilaiTerkecil << endl;
    cout << "Rata-rata nilai: " << rataRata << endl;

    // Dealokasi memori array dinamis untuk mencegah memory leak
    delete[] nilai;

    return 0;
}