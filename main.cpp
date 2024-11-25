#include <iostream>
#include <windows.h>
#include "config.h"
using namespace std;

HANDLE hCommDev;
const int BUFF_SIZE = 1024;

bool writeSerialPort(void *buffer, unsigned long numberOfBytesToWrite) {
    unsigned long numberOfBytesWritten = 0;
    if (WriteFile(hCommDev, buffer, numberOfBytesToWrite, &numberOfBytesWritten, nullptr) == 0) {
        cout << "Błąd wysyłania danych!" << endl;
        return false;
    }
    return true;
}

bool readSerialPort(void *buffer, unsigned long numberOfBytesToRead) {
    unsigned long numberOfBytesRead = 0;
    if (ReadFile(hCommDev, buffer, numberOfBytesToRead, &numberOfBytesRead, 0)) {
        return true;
    }
    return false;
}


void sendAndReceiveFile(FILE *src, FILE *dst, const long size, int frame, char *buffIn, char *buffOut) {
    for (int i = 0; i < frame; i++) {
        fread(buffOut, BUFF_SIZE, 1, src);
        writeSerialPort(buffOut, BUFF_SIZE);
        if (readSerialPort(buffIn, BUFF_SIZE) == 0) {
            cout << "Błąd w odbiorze danych!" << endl;
        }
        fwrite(buffIn, BUFF_SIZE, 1, dst);
        cout << i << "/" << frame << endl;
    }
    cout << frame << "/" << frame << endl;
    int remainingBytes = size % BUFF_SIZE;
    if (remainingBytes > 0) {
        fread(buffOut, remainingBytes, 1, src);
        writeSerialPort(buffOut, remainingBytes);
        readSerialPort(buffIn, remainingBytes);
        fwrite(buffIn, remainingBytes, 1, dst);
    }
}

void sendText() {
    string text;
    cout << "Podaj teskt do wyslania!" << endl;
    cin.ignore();
    getline(cin, text);
    int BUFF_SIZEText = text.size() + 1;
    if (BUFF_SIZEText > BUFF_SIZE) {
        cout << "Too big text!" << endl;
    }
    char *buffIn = new char[BUFF_SIZEText];
    strcpy(buffIn, text.c_str());
    if (writeSerialPort(buffIn, BUFF_SIZEText))
        cout << "Pomyslnie wyslano tekst!" << endl;
    else
        cout << "Blad w wyslaniu tekstu!" << endl;
    char *buffOut = new char[BUFF_SIZEText];
    if (readSerialPort(buffOut, BUFF_SIZEText) == 0)
        cout << "Blad w odbieraniu tekstu!" << endl;
    string result(buffOut);
    cout << "Odebrano tekst o tresci: " << result << endl;
    delete buffIn;
    delete buffOut;
}

void sendFile() {
    string pathOut;
    string pathIn;
    cout << "Podaj lokalizacje pliku, ktory chcesz skopiowac" << endl;
    cin.ignore();
    getline(cin, pathOut);
    cout << "Podaj lokalizacje docelowa pliku" << endl;
    getline(cin, pathIn);
    FILE *src = fopen(pathOut.c_str(), "rb");
    FILE *dst = fopen(pathIn.c_str(), "wb");
    fseek(src, 0,SEEK_END);
    const long size = ftell(src);
    int frame = size / BUFF_SIZE;
    fseek(src, 0,SEEK_SET);
    char *buffIn = new char[BUFF_SIZE];
    char *buffOut = new char[BUFF_SIZE];
    sendAndReceiveFile(src, dst, size, frame, buffIn, buffOut);
    fclose(src);
    fclose(dst);
    delete buffIn;
    delete buffOut;
}

void sendTextOrFile() {
    int choice = 0;

    while (true) {
        cout << "------------------- Wysylanie danych -------------------\n";
        cout << "Wybierz tryb wyslania:\n";
        cout << " [1] - Wyslanie lancucha znakow (tekst)\n";
        cout << " [2] - Wyslanie tresci pliku\n";
        cout << "-------------------------------------------------------\n";
        cout << "Podaj swoj wybor (1 lub 2): ";
        cin >> choice;

        if (choice == 1) {
            cout << "\nWybrano: Wysylanie lancucha znakow.\n";
            sendText();
            break;
        } else if (choice == 2) {
            cout << "\nWybrano: Wysylanie tresci pliku.\n";
            sendFile();
            break;
        } else {
            cout << "Niepoprawna wartosc! Wybierz ponownie.\n\n";
            // Clear the input stream if the user entered invalid data
            cin.clear();
            cin.ignore();
        }
    }
}



int main() {
    string portName;
    cout << "Podaj nazwe portu:" << endl;
    cin >> portName;
    hCommDev = CreateFile(portName.c_str(),
                          GENERIC_READ | GENERIC_WRITE,
                          0,
                          NULL,
                          OPEN_EXISTING,
                          0,
                          NULL);
    if (hCommDev == INVALID_HANDLE_VALUE) {
        cout << "Nie udalo sie otworzyc wybranego portu!" << endl << endl;
        return 1;
    }

    DCB dcb;
    dcb.DCBlength = sizeof(dcb);
    char changeSettings;
    std::cout << "Czy chcesz zmienic ustawienia portu? \n y/n?" << std::endl;
    std::cin >> changeSettings;
    if (changeSettings == 'y') {
        setupTransmision(hCommDev,dcb);
    }
    if (setTimeouts(hCommDev))
        return 1;
    sendTextOrFile();
    CloseHandle(hCommDev);
    return 0;
}
