#include <iostream>
using namespace std;

int main() {
    int menu;
    int level = level - 1;

    cout << "=== Selamat Datang Di Gacoan! ===";
    cout << "Mau Menu Yang Mana?\n";
    cout << "Masukan Menu: ";
    cin >> menu;

    switch (menu) {

        case 1:
            cout << "Kamu Memilih Gacoam!\n";
            cout << "Masukan Level: ";
            cin >> level;

            switch (level) {
                case 1:
                    cout <<"Gacoan Level 0 Akan Segera Dibuat!";
                    break;
                case 2:
                    cout << "Gacoan Level 1 Akan Segera Dibuat!";
                    break;
                case 3:
                    cout << "Gacoan Level 2 Akan Segera Dibuat!";
                    break;
                case 4:
                    cout << "Gacoan Level 3 Akan Segera Dibuat!";
                    break;
                case 5:
                    cout << "Gacoan Level 4 Akan Segera Dibuat!";
                    break;
                default:
                    cout << "Level Tidak Tersedia!";
                    break;
            }
            break;

        case 2:
            cout << "Kamu Memilih Hompimpa!\n";
            cout << "Masukan Level: ";
            cin >> level;

            switch (level) {
                case 1:
                    cout << "Hompimpa Level 0 Akan Segera Dibuat!";
                    break;
                case 2:
                    cout << "Hompimpa Level 1 Akan Segera Dibuat!";
                    break;
                case 3:
                    cout << "Hompimpa Level 2 Akan Segera Dibuat!";
                    break;
                case 4:
                    cout << "Hompimpa Level 3 Akan Segera Dibuat!";
                    break;
                case 5:
                    cout << "Hompimpa Level 4 Akan Segera Dibuat!";
                    break;
                default:
                    cout << "Level Tidak Tersedia!";
                    break;

            }
            break;

        case 3:
            cout << "Kamu Memilih Suit!\n";
            cout << "Masukan Level: ";
            cin >> level;

            switch (level) {
                case 1:
                    cout << "Suit Level 0 Akan Segera Dibuat!";
                    break;
                case 2:
                    cout << "Suit Level 1 Akan Segera Dibuat!";
                    break;
                case 3:
                    cout << "Suit Level 2 Akan Segera Dibuat!";
                    break;
                case 4:
                    cout << "Suit Level 3 Akan Segera Dibuat!";
                    break;
                case 5:
                    cout << "Suit Level 4 Akan Segera Dibuat!";
                    break;
                default:
                    cout << "Level Tidak Tersedia!";
                    break;

            }
            break;

        default:
            cout << "Menu Tidak Tersedia";
            break;
        




    }
}