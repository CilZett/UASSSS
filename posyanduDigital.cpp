#include <cctype>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <algorithm>  
#include <fstream>    
#include <iostream>   
#include <sstream>   
#include <string>     
#include <vector>    

using namespace std;

// ---------- STRUCT & DATA ----------
struct Bayi {
    string nik, nama, tglLahir, ibu, ayah;
};

struct Ukur {
    string nik, tanggal;
    double berat, tinggi;
};

const string FILE_DATA = "data_posyandu.csv";


vector<Bayi> daftarBayi;
vector<Ukur> daftarUkur;  // PENYIMPANAN PERMANEN: semua riwayat timbang (di file)

// true kalau ada perubahan di memori yang gagal disimpan ke file.
// Selama true, muatUlang() tidak boleh menimpa memori dengan isi file.
bool belumTersimpan = false;

// ---------- FUNGSI BANTU ----------
string trim(const string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

string huruf_kecil(string s) {
    transform(s.begin(), s.end(), s.begin(),
              [](unsigned char c) { return static_cast<char>(tolower(c)); });
    return s;
}

string hariIni() {
    time_t t = time(nullptr);
    tm* lt = localtime(&t);
    char buf[11];
    strftime(buf, sizeof(buf), "%Y-%m-%d", lt);
    return buf;
}

bool nikValid(const string& s) {
    return s.size() == 16 && all_of(s.begin(), s.end(),
                                    [](unsigned char c) { return isdigit(c); });
}

bool tanggalValid(const string& s) {
    if (s.size() != 10 || s[4] != '-' || s[7] != '-') return false;
    for (int i : {0, 1, 2, 3, 5, 6, 8, 9})
        if (!isdigit(static_cast<unsigned char>(s[i]))) return false;
    int th = stoi(s.substr(0, 4));
    int bl = stoi(s.substr(5, 2));
    int tg = stoi(s.substr(8, 2));
    if (bl < 1 || bl > 12 || tg < 1) return false;
    int hari[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    bool kabisat = (th % 4 == 0 && th % 100 != 0) || th % 400 == 0;
    int maks = hari[bl - 1] + ((bl == 2 && kabisat) ? 1 : 0);
    return tg <= maks;
}

int cariIndexNik(const string& nik) {
    for (size_t i = 0; i < daftarBayi.size(); i++)
        if (daftarBayi[i].nik == nik) return static_cast<int>(i);
    return -1;
}

// ---------- INPUT DENGAN VALIDASI ----------
string bacaBaris(const string& label) {
    cout << label << ": ";
    string s;
    if (!getline(cin, s)) {  // input habis (Ctrl+D / Ctrl+Z)
        cout << "\nInput selesai.\n";
        exit(0);
    }
    return trim(s);
}

string inputTeks(const string& label) {
    while (true) {
        string s = bacaBaris(label);
        if (!s.empty()) return s;
        cout << "  ! Tidak boleh kosong.\n";
    }
}

string inputNik() {
    while (true) {
        string s = bacaBaris("NIK (16 digit)");
        if (nikValid(s)) return s;
        cout << "  ! NIK harus 16 digit angka.\n";
    }
}

string inputTanggalLahir() {
    while (true) {
        string s = bacaBaris("Tanggal lahir (YYYY-MM-DD)");
        if (!tanggalValid(s))
            cout << "  ! Format/tanggal tidak valid, contoh: 2025-03-14\n";
        else if (s > hariIni())
            cout << "  ! Tanggal lahir tidak boleh di masa depan.\n";
        else
            return s;
    }
}

double inputAngka(const string& label) {
    while (true) {
        string s = bacaBaris(label);
        replace(s.begin(), s.end(), ',', '.');
        try {
            size_t pos = 0;
            double v = stod(s, &pos);
            if (pos == s.size() && v > 0) return v;
        } catch (...) {
        }
        cout << "  ! Masukkan angka lebih dari 0.\n";
    }
}

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
    if (!f) return false;
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

void simpanDanLapor(const string& pesanSukses) {
    while (!tulisCsv(FILE_DATA, daftarUkur, true)) {
        cout << "GAGAL menyimpan ke " << FILE_DATA
             << ". Kalau file sedang dibuka di Excel, tutup dulu.\n";
        string ulang = bacaBaris("Coba simpan lagi? (y/n)");
        if (huruf_kecil(ulang) != "y") {
            belumTersimpan = true;
            cout << "(Perubahan baru ada di memori, belum tersimpan permanen.)\n";
            return;
        }
    }
    belumTersimpan = false;
    cout << pesanSukses << "\n";
}

// Membaca data_posyandu.csv. Mengembalikan false kalau file belum ada.
void bacaFileData() {
    int barisDilewati = 0;
    ifstream f(FILE_DATA, ios::binary);
    if (!f) return;
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
        if (!nikValid(nik)) { barisDilewati++; continue; };
        if (cariIndexNik(nik) == -1)
            daftarBayi.push_back({nik, trim(k[1]), trim(k[2]), trim(k[3]), trim(k[4])});
        if (k.size() >= 8 && !trim(k[5]).empty()) {
            string b = trim(k[6]), t = trim(k[7]);
            replace(b.begin(), b.end(), ',', '.');
            replace(t.begin(), t.end(), ',', '.');
            try {
                daftarUkur.push_back({nik, trim(k[5]), stod(b), stod(t)});
            } catch (...) { 
                barisDilewati++; 
            }
        }
    }
    if (barisDilewati > 0)
    cout << "PERINGATAN: " << barisDilewati << " baris di " << FILE_DATA
         << " tidak terbaca dan akan HILANG saat menyimpan!\n"
         << "Cek file (pakai Notepad) sebelum melanjutkan.\n";
}

// Baca ulang file supaya edit manual di CSV ikut terbaca.
// Dilewati kalau ada perubahan di memori yang belum berhasil disimpan.
void muatUlang() {
    if (belumTersimpan) return;
    daftarBayi.clear();
    daftarUkur.clear();
    bacaFileData();
}

// ---------- FITUR ----------
void tambahBayi() {
    muatUlang();
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
    muatUlang();
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
    muatUlang();
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
    muatUlang();
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
    muatUlang();
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
    muatUlang();
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
    simpanDanLapor("Data dihapus.");
}

// Membuat laporan dari penyimpanan, bisa untuk bulan berapa saja
void exportLaporanBulan() {
    muatUlang();
    cout << "\n--- Export Laporan per Bulan ---\n";
    int bulan = 0;
    while (true) {
        string sb = bacaBaris("Bulan (1-12)");
        try {
            size_t pos = 0;
            int v = stoi(sb, &pos);
            if (pos == sb.size() && v >= 1 && v <= 12) { bulan = v; break; }
        } catch (...) {
        }
        cout << "  ! Masukkan angka 1 sampai 12.\n";
    }
    string tahun = hariIni().substr(0, 4);
    while (true) {
        string st = bacaBaris("Tahun (kosongkan = " + tahun + ")");
        if (st.empty()) break;
        if (st.size() == 4 && all_of(st.begin(), st.end(),
                                     [](unsigned char c) { return isdigit(c); })) {
            tahun = st;
            break;
        }
        cout << "  ! Tahun harus 4 digit, contoh: 2026\n";
    }
    string awalan = tahun + "-" + (bulan < 10 ? "0" : "") + to_string(bulan);
    
    vector<Ukur> terpilih;
    for (const auto& u : daftarUkur)
        if (u.tanggal.rfind(awalan, 0) == 0) terpilih.push_back(u);
    if (terpilih.empty()) {
        cout << "Tidak ada penimbangan pada " << awalan << ".\n";
        return;
    }
    sort(terpilih.begin(), terpilih.end(),
         [](const Ukur& x, const Ukur& y) { return x.tanggal < y.tanggal; });

    cout << "Penimbangan pada " << awalan << " (" << terpilih.size() << " data):\n";
    cout << fixed << setprecision(1);
    for (const auto& u : terpilih) {
        int idx = cariIndexNik(u.nik);
        string namaBayi = (idx == -1) ? u.nik : daftarBayi[idx].nama;
        cout << "  " << u.tanggal << "  " << left << setw(20) << namaBayi
             << right << setw(7) << u.berat << " kg" << setw(8) << u.tinggi
             << " cm\n";
    }
    string nama = "laporan_" + awalan + ".csv";
    if (tulisCsv(nama, terpilih, false))
        cout << "Laporan diekspor ke " << nama << ". Bisa dibuka di Excel.\n";
    else
        cout << "GAGAL menulis " << nama
             << ". Kalau file sedang dibuka di Excel, tutup dulu lalu ulangi.\n";
}
// ---------- MENU UTAMA ----------
int main() {
    bacaFileData();
    while (true) {
        cout << "\n=== POSYANDU DIGITAL ===\n"
             << "1. Tambah data bayi (sekaligus catat berat & tinggi)\n"
             << "2. Catat pengukuran bulanan\n"
             << "3. Cari bayi\n"
             << "4. Lihat riwayat pengukuran\n"
             << "5. Laporan ringkas\n"
             << "6. Export laporan per bulan (untuk puskesmas)\n"
             << "7. Hapus data bayi\n"
             << "0. Keluar\n";
        string pilihan = bacaBaris("Pilih menu");
        if (pilihan == "0") {
            cout << "Sampai jumpa.\n";
            break;
        } 
        else if (pilihan == "1") tambahBayi();
        else if (pilihan == "2") catatPengukuran();
        else if (pilihan == "3") cariBayi();
        else if (pilihan == "4") riwayat();
        else if (pilihan == "5") laporan();
        else if (pilihan == "6") exportLaporanBulan();
        else if (pilihan == "7") hapusBayi();
        else cout << "Pilihan tidak valid.\n";
    }
    return 0;
}
