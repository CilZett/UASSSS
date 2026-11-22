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