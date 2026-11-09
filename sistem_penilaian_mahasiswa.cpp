#include <iostream>
#include <string>
#include <limits>
#include <iomanip>
using namespace std;
 
int main() {
    int jumlahMahasiswa;
 
    // Data untuk statistik
    int lulus = 0, tidakLulus = 0;
    int jumlahA = 0, jumlahB = 0, jumlahC = 0, jumlahD = 0, jumlahE = 0;
    float totalNilai = 0;
    float nilaiTertinggi = -1, nilaiTerendah = 101;
    string namaTertinggi, namaTerendah;
 
    cout << "===== SISTEM PENILAIAN MAHASISWA =====" << "\n";
    cout << "Masukan jumlah mahasiswa: ";
    cin >> jumlahMahasiswa;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');  // buang newline sisa biar getline gak keskip
 
    for (int i = 1; i <= jumlahMahasiswa; i++) {
        string name;
        long long nim;
        float nilaiTugas, nilaiUTS, nilaiUAS, kehadiran, nilaiAkhir;
        char grade;
        string status, predikat;
            
        cout << "\n--- Data Mahasiswa ke-" << i << " ---" << "\n";
 
        cout << "Masukan Nama: ";
        getline(cin, name);
 
        cout << "Masukan NIM: ";
        cin >> nim;
 
        cout << "Masukan Nilai Tugas: ";
        cin >> nilaiTugas;

        while (nilaiTugas < 0 || nilaiTugas > 100) {
            cout << "Nilai Melebihi Batas!\n";
            cout << "Masukan Kembali Nilai Tugas: ";
            cin >> nilaiTugas;
        }
 
        cout << "Masukan Nilai UTS: ";
        cin >> nilaiUTS;

        while (nilaiUTS < 0 || nilaiUTS > 100) {
            cout << "Nilai Melebihi Batas!\n";
            cout << "Masukan Kembali Nilai UTS: ";
            cin >> nilaiUTS;
        }
 
        cout << "Masukan Nilai UAS: ";
        cin >> nilaiUAS;

        while (nilaiUAS < 0 || nilaiUAS > 100) {
            cout << "Nilai Melebihi Batas!\n";
            cout << "Masukan Kembali Nilai UAS: ";
            cin >> nilaiUAS;
        }
 
        cout << "Masukan Kehadiran (%): ";
        cin >> kehadiran;
        cin.ignore(); // buang newline sisa sebelum getline berikutnya

        while (kehadiran < 0 || kehadiran > 100) {
            cout << "Nilai Melebihi Batas!\n";
            cout << "Masukan Kembali Kehadiran (%): ";
            cin >> kehadiran;
        }

        
 
        // 1. Hitung nilai akhir
        nilaiAkhir = (0.3 * nilaiTugas) + (0.3 * nilaiUTS) + (0.4 * nilaiUAS);
 
        // 2. Tentukan grade
        if (nilaiAkhir >= 85) {
            grade = 'A';
            jumlahA++;
        } else if (nilaiAkhir >= 75) {
            grade = 'B';
            jumlahB++;
        } else if (nilaiAkhir >= 65) {
            grade = 'C';
            jumlahC++;
        } else if (nilaiAkhir >= 50) {
            grade = 'D';
            jumlahD++;
        } else {
            grade = 'E';
            jumlahE++;
        }
 
        // 3. Tentukan status kelulusan
        if (nilaiAkhir >= 65 && kehadiran >= 75) {
            status = "LULUS";
            lulus++;
        } else {
            status = "TIDAK LULUS";
            tidakLulus++;
        }
 
        // 4. Tentukan predikat
        if (status == "LULUS") {
            if (nilaiAkhir >= 90) {
                predikat = "Sangat Memuaskan";
            } else if (nilaiAkhir >= 80) {
                predikat = "Memuaskan";
            } else if (nilaiAkhir >= 70) {
                predikat = "Baik";
            } else {
                predikat = "Cukup";
            }
        } else {
            predikat = "Tidak Mendapat Predikat";
        }
 
        // Tampilkan hasil per mahasiswa
        cout << fixed;
        cout.precision(2);
        cout << "\n>> Hasil Mahasiswa ke-" << i << " <<" << "\n";
        cout << left << setw(25) << "Nama" << ": " << name << "\n";
        cout << left << setw(25) << "NIM" << ": " << nim << "\n";
        cout << left << setw(25) << "Nilai Akhir" << ": " << nilaiAkhir << "\n";
        cout << left << setw(25) << "Grade" << ": " << grade << "\n";
        cout << left << setw(25) << "Status" << ": " << status << "\n";
        cout << left << setw(25) << "Predikat" << ": " << predikat << "\n";
 
        // Update statistik untuk rekap
        totalNilai += nilaiAkhir;
 
        if (nilaiAkhir > nilaiTertinggi) {
            nilaiTertinggi = nilaiAkhir;
            namaTertinggi = name;
        }
        if (nilaiAkhir < nilaiTerendah) {
            nilaiTerendah = nilaiAkhir;
            namaTerendah = name;
        }
    }
 
    // Rekap akhir setelah semua mahasiswa diproses
    cout << "\n===== REKAP HASIL SELURUH MAHASISWA =====" << "\n";
    cout << left << setw(25) << "Jumlah Mahasiswa" << ": " << jumlahMahasiswa << "\n";
    cout << left << setw(25) << "Jumlah Mahasiswa Lulus" << ": " << lulus << "\n";
    cout << left << setw(25) << "Jumlah Tidak Lulus" << ": " << tidakLulus << "\n";
    cout << left << setw(25) << "Jumlah Grade A" << ": " << jumlahA << "\n";
    cout << left << setw(25) << "Jumlah Grade B" << ": " << jumlahB << "\n";
    cout << left << setw(25) << "Jumlah Grade C" << ": " << jumlahC << "\n";
    cout << left << setw(25) << "Jumlah Grade D" << ": " << jumlahD << "\n";
    cout << left << setw(25) << "Jumlah Grade E" << ": " << jumlahE << "\n";
 
    cout << fixed;
    cout.precision(2);
    if (jumlahMahasiswa > 0) {
        cout << left << setw(25) << "Rata-rata Nilai" << ": " << (totalNilai / jumlahMahasiswa) << "\n";
    }
 
    cout << "\n----- Bonus -----" << "\n";
    cout << left << setw(25) << "Nilai Tertinggi" << ": " << nilaiTertinggi << " (" << namaTertinggi << ")" << "\n";
    cout << left << setw(25) << "Nilai Terendah" << ": " << nilaiTerendah << " (" << namaTerendah << ")" << "\n";
 
    return 0;
}