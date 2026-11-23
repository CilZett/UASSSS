#include <algorithm>  
#include <fstream>    
#include <iostream>   
#include <sstream>   
#include <string>     
#include <vector>    
using namespace std;

// ---------- PENYIMPANAN FILE (satu file CSV) ----------
// Format data_posyandu.csv: satu baris = satu pengukuran, data bayi diulang
// di tiap barisnya. Bayi yang belum pernah ditimbang punya satu baris dengan
// kolom pengukuran kosong. Pemisah kolom ';' dan desimal ',' (format Excel
// Indonesia), jadi bisa langsung di-double-click. NIK ditulis ="..." supaya
// Excel tidak mengubahnya jadi 1,23E+15.
// Kolom: nik;nama;tgl_lahir;nama_ibu;nama_ayah;tgl_ukur;berat_kg;tinggi_cm


// Fungsi agar teks yang dimasukan berformat Kutip
string kutip(const string& s) {
    string hasil = "\"";
    for (char c : s) {
        if (c == '"') hasil += "\"\"";
        else hasil += c;
    }
    return hasil + "\"";
}

// Fungsi mengubah titik jadi koma agar excel bisa membaca
string angkaCsv(double v) {
    ostringstream os;
    os << v;
    string s = os.str();
    replace(s.begin(), s.end(), '.', ',');
    return s;
}

// Pecah satu baris CSV menjadi kolom (mengerti tanda kutip).
vector<string> pecahCsv(const string& baris, char pemisah) {
    vector<string> kolom;
    string sel;
    bool dalamKutip = false;
    for (size_t i = 0; i < baris.size(); i++) {
        char c = baris[i];
        if (dalamKutip) {
            if (c == '"' && i + 1 < baris.size() && baris[i + 1] == '"') {
                sel += '"';
                i++;
            } else if (c == '"') {
                dalamKutip = false;
            } else {
                sel += c;
            }
        } else if (c == '"') {
            dalamKutip = true;
        } else if (c == pemisah) {
            kolom.push_back(sel);
            sel.clear();
        } else {
            sel += c;
        }
    }
    kolom.push_back(sel);
    return kolom;
}

// Menulis CSV ke namaFile dari dataUkur. Bayi yang tidak punya penimbangan di
// dataUkur hanya ikut ditulis kalau sertakanTanpaUkur = true (arsip lengkap).
// Mengembalikan false kalau gagal menulis (misalnya file sedang dibuka di Excel).
bool tulisCsv(const string& namaFile, const vector<Ukur>& dataUkur,
              bool sertakanTanpaUkur) {
    ofstream f(namaFile, ios::binary | ios::trunc);
    f << "\xEF\xBB\xBF" << "sep=;\r\n";  // BOM UTF-8 + petunjuk pemisah untuk Excel
    f << "nik;nama;tgl_lahir;nama_ibu;nama_ayah;tgl_ukur;berat_kg;tinggi_cm\r\n";
    for (const auto& b : daftarBayi) {
        string depan = "=\"" + b.nik + "\";" + kutip(b.nama) + ";" + b.tglLahir +
                       ";" + kutip(b.ibu) + ";" + kutip(b.ayah);
        vector<Ukur> milik;
        for (const auto& u : dataUkur)
            if (u.nik == b.nik) milik.push_back(u);
        sort(milik.begin(), milik.end(),
             [](const Ukur& x, const Ukur& y) { return x.tanggal < y.tanggal; });
        if (milik.empty() && sertakanTanpaUkur) f << depan << ";;;\r\n";
        for (const auto& u : milik)
            f << depan << ";" << u.tanggal << ";" << angkaCsv(u.berat) << ";"
              << angkaCsv(u.tinggi) << "\r\n";
    }
    f.close();
    return !f.fail();
}

// Simpan arsip lengkap (semua bayi + seluruh riwayat penimbangan).
bool simpanData() { return tulisCsv(FILE_DATA, daftarUkur, true); }

void simpanDanLapor(const string& pesanSukses) {
    if (simpanData())
        cout << pesanSukses << "\n";
    else
        cout << "GAGAL menyimpan ke " << FILE_DATA
             << ". Kalau file sedang dibuka di Excel, tutup dulu lalu ulangi.\n"
             << "(Perubahan tadi belum tersimpan permanen.)\n";
}

// Membaca data_posyandu.csv. Mengembalikan false kalau file belum ada.
bool bacaFileData() {
    ifstream f(FILE_DATA, ios::binary);
    if (!f) return false;
    string baris;
    char pemisah = ';';
    bool header = true;
    while (getline(f, baris)) {
        if (baris.size() >= 3 && (unsigned char)baris[0] == 0xEF &&
            (unsigned char)baris[1] == 0xBB && (unsigned char)baris[2] == 0xBF)
            baris.erase(0, 3);  // buang BOM
        baris = trim(baris);
        if (baris.empty() || baris.rfind("sep=", 0) == 0) continue;
        if (header) {  // baris judul kolom; dari sini pemisahnya ditentukan
            header = false;
            pemisah = (baris.find(';') != string::npos) ? ';' : ',';
            continue;
        }
        auto k = pecahCsv(baris, pemisah);
        if (k.size() < 5) continue;
        string nik = trim(k[0]);
        if (!nik.empty() && nik[0] == '=') nik.erase(0, 1);  // buang tanda =
        if (!nikValid(nik)) continue;
        if (cariIndexNik(nik) == -1)
            daftarBayi.push_back({nik, trim(k[1]), trim(k[2]), trim(k[3]), trim(k[4])});
        if (k.size() >= 8 && !trim(k[5]).empty()) {
            string b = trim(k[6]), t = trim(k[7]);
            replace(b.begin(), b.end(), ',', '.');
            replace(t.begin(), t.end(), ',', '.');
            try {
                daftarUkur.push_back({nik, trim(k[5]), stod(b), stod(t)});
            } catch (...) {
            }
        }
    }
    return true;
}

void muatData() {
    bacaFileData();
}


// Catat satu penimbangan ke KEDUA penyimpanan: arsip permanen dan sesi.
void catatTimbang(const Ukur& u) {
    daftarUkur.push_back(u);
    sesiUkur.push_back(u);
    sesiBelumDiexport = true;
}


// ---------- FITUR ----------
void tambahBayi() {
    cout << "\n--- Tambah Data Bayi ---\n";
    string nik = inputNik();
    if (cariIndexNik(nik) != -1) {
        cout << "NIK sudah terdaftar.\n";
        return;
    }
    Bayi b;
    b.nik = nik;
    b.nama = inputTeks("Nama bayi");
    b.tglLahir = inputTanggalLahir();
    b.ibu = inputTeks("Nama ibu");
    b.ayah = inputTeks("Nama ayah");
    cout << "Penimbangan hari ini (" << hariIni() << ") untuk " << b.nama << ":\n";
    Ukur u;
    u.nik = nik;
    u.tanggal = hariIni();
    u.berat = inputAngka("Berat (kg)");
    u.tinggi = inputAngka("Tinggi (cm)");
    daftarBayi.push_back(b);
    catatTimbang(u);
    simpanDanLapor("Data bayi dan pengukuran tersimpan.");
}

void catatPengukuran() {
    cout << "\n--- Catat Pengukuran ---\n";
    string nik = inputNik();
    int idx = cariIndexNik(nik);
    if (idx == -1) {
        cout << "NIK belum terdaftar. Tambah data bayi dulu.\n";
        return;
    }
    cout << "Bayi: " << daftarBayi[idx].nama << "\n";
    Ukur u;
    u.nik = nik;
    u.tanggal = hariIni();
    u.berat = inputAngka("Berat (kg)");
    u.tinggi = inputAngka("Tinggi (cm)");
    catatTimbang(u);
    simpanDanLapor("Pengukuran tersimpan.");
}

void cariBayi() {
    cout << "\n--- Cari Bayi ---\n";
    string kata = huruf_kecil(bacaBaris("Ketik nama atau NIK"));
    int ditemukan = 0;
    for (const auto& b : daftarBayi) {
        if (huruf_kecil(b.nama).find(kata) != string::npos ||
            b.nik.find(kata) != string::npos) {
            cout << b.nik << " | " << b.nama << " | lahir " << b.tglLahir
                 << " | ibu: " << b.ibu << " | ayah: " << b.ayah << "\n";
            ditemukan++;
        }
    }
    if (ditemukan == 0) cout << "Tidak ditemukan.\n";
}

void riwayat() {
    cout << "\n--- Riwayat Pengukuran ---\n";
    string nik = inputNik();
    int idx = cariIndexNik(nik);
    if (idx == -1) {
        cout << "NIK belum terdaftar.\n";
        return;
    }
    vector<Ukur> hasil;
    for (const auto& u : daftarUkur)
        if (u.nik == nik) hasil.push_back(u);
    if (hasil.empty()) {
        cout << "Belum ada data pengukuran untuk " << daftarBayi[idx].nama << ".\n";
        return;
    }
    sort(hasil.begin(), hasil.end(),
         [](const Ukur& a, const Ukur& b) { return a.tanggal < b.tanggal; });
    cout << "Bayi: " << daftarBayi[idx].nama << " | sudah ditimbang "
         << hasil.size() << " kali\n";
    cout << left << setw(5) << "Ke-" << setw(12) << "Tanggal" << right << setw(12)
         << "Berat (kg)" << setw(14) << "Tinggi (cm)" << "\n";
    cout << fixed << setprecision(1);
    int no = 1;
    for (const auto& u : hasil)
        cout << left << setw(5) << no++ << setw(12) << u.tanggal << right << setw(12)
             << u.berat << setw(14) << u.tinggi << "\n";
}

void laporan() {
    cout << "\n--- Laporan Ringkas ---\n";
    cout << "Jumlah bayi terdaftar  : " << daftarBayi.size() << "\n";
    cout << "Jumlah data pengukuran : " << daftarUkur.size() << "\n";
    if (daftarUkur.empty()) {
        cout << "Rata-rata berat badan  : -\n";
        return;
    }
    double total = 0;
    for (const auto& u : daftarUkur) total += u.berat;
    cout << fixed << setprecision(1);
    cout << "Rata-rata berat badan  : " << total / daftarUkur.size() << " kg\n";
}

void hapusBayi() {
    cout << "\n--- Hapus Data Bayi ---\n";
    string nik = inputNik();
    int idx = cariIndexNik(nik);
    if (idx == -1) {
        cout << "NIK tidak ditemukan.\n";
        return;
    }
    string yakin = bacaBaris("Hapus " + daftarBayi[idx].nama +
                             " beserta seluruh riwayatnya? (y/n)");
    if (huruf_kecil(yakin) != "y") {
        cout << "Dibatalkan.\n";
        return;
    }
    daftarBayi.erase(daftarBayi.begin() + idx);
    daftarUkur.erase(remove_if(daftarUkur.begin(), daftarUkur.end(),
                               [&](const Ukur& u) { return u.nik == nik; }),
                     daftarUkur.end());
    sesiUkur.erase(remove_if(sesiUkur.begin(), sesiUkur.end(),
                             [&](const Ukur& u) { return u.nik == nik; }),
                   sesiUkur.end());
    if (sesiUkur.empty()) sesiBelumDiexport = false;
    simpanDanLapor("Data dihapus.");
}

// Membuat laporan dari penyimpanan SESI: hanya penimbangan yang dicatat sejak
// program dibuka. Arsip permanen (data_posyandu.csv) tidak diubah sama sekali.
void exportLaporan() {
    cout << "\n--- Export Laporan Sesi Ini ---\n";
    if (sesiUkur.empty()) {
        cout << "Belum ada penimbangan di sesi ini. Catat dulu lewat menu 1 atau 2.\n";
        return;
    }
    cout << "Penimbangan di sesi ini (" << sesiUkur.size() << " data):\n";
    cout << fixed << setprecision(1);
    for (const auto& u : sesiUkur) {
        int idx = cariIndexNik(u.nik);
        string namaBayi = (idx == -1) ? u.nik : daftarBayi[idx].nama;
        cout << "  " << left << setw(20) << namaBayi << right << setw(7) << u.berat
             << " kg" << setw(8) << u.tinggi << " cm\n";
    }
    string nama = "laporan_" + stempelFile() + ".csv";
    if (tulisCsv(nama, sesiUkur, false)) {
        sesiBelumDiexport = false;
        cout << "Laporan diekspor ke " << nama << ". Bisa dibuka di Excel.\n";
    } else {
        cout << "GAGAL menulis " << nama
             << ". Kalau file sedang dibuka di Excel, tutup dulu lalu ulangi.\n";
    }
}

// ---------- MENU UTAMA ----------
int main() {
    muatData();
    while (true) {
        cout << "\n=== POSYANDU DIGITAL ===\n"
             << "1. Tambah data bayi (sekaligus catat berat & tinggi)\n"
             << "2. Catat pengukuran bulanan\n"
             << "3. Cari bayi\n"
             << "4. Lihat riwayat pengukuran\n"
             << "5. Laporan ringkas\n"
             << "6. Export laporan sesi ini (untuk puskesmas)\n"
             << "7. Hapus data bayi\n"
             << "0. Keluar\n";
        string pilihan = bacaBaris("Pilih menu");
        if (pilihan == "0") {
            if (sesiBelumDiexport) {
                string yakin = bacaBaris(
                    "Ada " + to_string(sesiUkur.size()) +
                    " penimbangan sesi ini yang belum diexport jadi laporan. "
                    "Tetap keluar? (y/n)");
                if (huruf_kecil(yakin) != "y") continue;
            }
            cout << "Sampai jumpa.\n";
            break;
        } 
        else if (pilihan == "1") tambahBayi();
        else if (pilihan == "2") catatPengukuran();
        else if (pilihan == "3") cariBayi();
        else if (pilihan == "4") riwayat();
        else if (pilihan == "5") laporan();
        else if (pilihan == "6") exportLaporan();
        else if (pilihan == "7") hapusBayi();
        else cout << "Pilihan tidak valid.\n";
    }
    return 0;
}
