#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <ctime>
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
vector<Ukur> daftarUkur;

// Penyimpanan sesi penimbangan
vector<Ukur> sesiUkur;
bool sesiBelumDiexport = false;


// ---------- FUNGSI BANTU ----------

string trim(const string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";

    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

string huruf_kecil(string s) {
    transform(s.begin(), s.end(), s.begin(),
              [](unsigned char c) {
                  return static_cast<char>(tolower(c));
              });
    return s;
}

vector<string> pecah(const string& s, char pemisah) {
    vector<string> hasil;
    string bagian;
    stringstream ss(s);

    while (getline(ss, bagian, pemisah))
        hasil.push_back(bagian);

    return hasil;
}

string hariIni() {
    time_t t = time(nullptr);
    tm* lt = localtime(&t);

    char buf[11];
    strftime(buf, sizeof(buf), "%Y-%m-%d", lt);

    return buf;
}

string stempelFile() {
    time_t t = time(nullptr);
    tm* lt = localtime(&t);

    char buf[16];
    strftime(buf, sizeof(buf), "%Y-%m-%d_%H%M", lt);

    return buf;
}

bool nikValid(const string& s) {
    return s.size() == 16 &&
           all_of(s.begin(), s.end(),
                  [](unsigned char c) {
                      return isdigit(c);
                  });
}

bool tanggalValid(const string& s) {
    if (s.size() != 10 || s[4] != '-' || s[7] != '-')
        return false;

    for (int i : {0, 1, 2, 3, 5, 6, 8, 9})
        if (!isdigit(static_cast<unsigned char>(s[i])))
            return false;

    int th = stoi(s.substr(0, 4));
    int bl = stoi(s.substr(5, 2));
    int tg = stoi(s.substr(8, 2));

    if (bl < 1 || bl > 12 || tg < 1)
        return false;

    int hari[] = {
        31, 28, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31
    };

    bool kabisat =
        (th % 4 == 0 && th % 100 != 0) || th % 400 == 0;

    int maks = hari[bl - 1] +
               ((bl == 2 && kabisat) ? 1 : 0);

    return tg <= maks;
}

int cariIndexNik(const string& nik) {
    for (size_t i = 0; i < daftarBayi.size(); i++) {
        if (daftarBayi[i].nik == nik)
            return static_cast<int>(i);
    }

    return -1;
}


// ---------- INPUT DENGAN VALIDASI ----------

string bacaBaris(const string& label) {
    cout << label << ": ";

    string s;

    if (!getline(cin, s)) {
        cout << "\nInput selesai.\n";
        exit(0);
    }

    return trim(s);
}

string inputTeks(const string& label) {
    while (true) {
        string s = bacaBaris(label);

        if (!s.empty())
            return s;

        cout << "  ! Tidak boleh kosong.\n";
    }
}

string inputNik() {
    while (true) {
        string s = bacaBaris("NIK (16 digit)");

        if (nikValid(s))
            return s;

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

            if (pos == s.size() && v > 0)
                return v;

        } catch (...) {
        }

        cout << "  ! Masukkan angka lebih dari 0.\n";
    }
}