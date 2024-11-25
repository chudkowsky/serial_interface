//
// Created by mateu on 25/11/2024.
//

#include <iostream>
#include <windows.h>
#include "config.h"
using namespace std;

const DWORD baudRates[] = {
    CBR_110, CBR_300, CBR_600, CBR_1200, CBR_2400,
    CBR_4800, CBR_9600, CBR_14400, CBR_19200,
    CBR_38400, CBR_56000, CBR_57600, CBR_115200,
    CBR_128000, CBR_256000
};
const BYTE parityOptions[] = {EVENPARITY, MARKPARITY, NOPARITY, ODDPARITY};

void get_comm_timeout(HANDLE hCommDev, COMMTIMEOUTS CommTimeouts) {
    BOOL if_correct = GetCommTimeouts(hCommDev, &CommTimeouts);
    if (!if_correct) {
        std::cout << "Blad podczas pobierania ustawien czasow komunikacji.\n";
        return;
    }

    std::cout << "------------ Czas oczekiwania komunikacji ------------\n";
    std::cout << " Odstep pomiedzy odczytami       : " << CommTimeouts.ReadIntervalTimeout << " ms\n";
    std::cout << " Czas calkowity odczytu (staly)  : " << CommTimeouts.ReadTotalTimeoutConstant << " ms\n";
    std::cout << " Mnoznik czasu odczytu           : " << CommTimeouts.ReadTotalTimeoutMultiplier << " ms\n";
    std::cout << " Czas calkowity zapisu (staly)   : " << CommTimeouts.WriteTotalTimeoutConstant << " ms\n";
    std::cout << " Mnoznik czasu zapisu            : " << CommTimeouts.WriteTotalTimeoutMultiplier << " ms\n";
    std::cout << "------------------------------------------------------\n";
}



void get_comm_state(HANDLE hCommDev, DCB dcb) {
    BOOL if_correct = GetCommState(hCommDev, &dcb);
    if (!if_correct) {
        std::cout << "Blad podczas pobierania stanu portu szeregowego.\n";
        return;
    }

    std::cout << "-------------- Stan portu szeregowego ---------------\n";
    std::cout << " Predkosc transmisji (BaudRate) : " << dcb.BaudRate << "\n";
    std::cout << " Liczba bitow danych (ByteSize) : " << static_cast<int>(dcb.ByteSize) << " bity\n";
    std::cout << " Parzystosc (fParity)           : " << (dcb.fParity ? "Wlaczona" : "Wylaczona") << "\n";
    std::cout << " Kontrola DTR (fDtrControl)     : " << (dcb.fDtrControl ? "Wlaczona" : "Wylaczona") << "\n";
    std::cout << " Bity stopu (StopBits)          : ";

    switch (dcb.StopBits) {
        case ONESTOPBIT:
            std::cout << "1 bit stopu\n";
        break;
        case ONE5STOPBITS:
            std::cout << "1.5 bitu stopu\n";
        break;
        case TWOSTOPBITS:
            std::cout << "2 bity stopu\n";
        break;
        default:
            std::cout << "Nieznana wartosc\n";
        break;
    }
    std::cout << "-----------------------------------------------------\n";
}

bool setTimeouts(HANDLE hCommDev) {
    COMMTIMEOUTS timeouts = {};
    timeouts.ReadIntervalTimeout = 0;
    timeouts.ReadTotalTimeoutConstant = 1000;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 0;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    if (!SetCommTimeouts(hCommDev, &timeouts)) {
        cout << "Błąd ustawiania timeout'ów!" << endl;
        CloseHandle(hCommDev);
        return 1;
    }
    return 0;
}

void setBaudRate(DCB &dcb) {
    int baudRateChoice = 0;
    while (true) {
        cout << "Wybierz wartosc parametru BaudRate (numer opcji):\n"
           << "-----------------------------------------------\n"
           << " [ 1] - CBR_110       [ 6] - CBR_4800      [11] - CBR_56000\n"
           << " [ 2] - CBR_300       [ 7] - CBR_9600      [12] - CBR_57600\n"
           << " [ 3] - CBR_600       [ 8] - CBR_14400     [13] - CBR_115200\n"
           << " [ 4] - CBR_1200      [ 9] - CBR_19200     [14] - CBR_128000\n"
           << " [ 5] - CBR_2400     [10] - CBR_38400     [15] - CBR_256000\n"
           << "-----------------------------------------------\n"
           << "Podaj numer opcji: ";
        cin >> baudRateChoice;

        if (baudRateChoice >= 1 && baudRateChoice <= 15) {
            dcb.BaudRate = baudRates[baudRateChoice - 1];
            break;
        } else {
            cout << "Niepoprawna wartosc! Wybierz ponownie.\n";
        }
    }
}

void setParity(DCB &dcb) {
    int parityChoice = 0;
    while (true) {
        cout << "Wybierz wartosc parametru fParity (numer opcji):\n"
          << "-----------------------------------------------\n"
          << " [ 1] - EVENPARITY   (Parzystosc)\n"
          << " [ 2] - MARKPARITY   (Znakowana parzystosc)\n"
          << " [ 3] - NOPARITY     (Bez parzystosci)\n"
          << " [ 4] - ODDPARITY    (Nieparzystosc)\n"
          << "-----------------------------------------------\n"
          << "Podaj numer opcji: ";
        cin >> parityChoice;
        if (parityChoice >= 1 && parityChoice <= 4) {
            dcb.Parity = parityOptions[parityChoice - 1];
            break;
        } else {
            cout << "Niepoprawna wartosc! Wybierz ponownie.\n";
        }
    }
}

void setByteSize(DCB &dcb) {
    int byteSize = 0;

    while (true) {
        cout << "Wybierz wartosc parametru ByteSize (liczba bitow danych):\n"
             << "-----------------------------------------------\n"
             << " [ 5] - 5 bity\n"
             << " [ 6] - 6 bitow\n"
             << " [ 7] - 7 bitow\n"
             << " [ 8] - 8 bitow\n"
             << "-----------------------------------------------\n"
             << "Podaj liczbe: ";
        cin >> byteSize;

        if (byteSize == 5 || byteSize == 6 || byteSize == 7 || byteSize == 8) {
            dcb.ByteSize = byteSize;
            return;
        }

        cout << "Niepoprawna wartosc! Wybierz ponownie.\n";
    }
}


void setStopBits(DCB &dcb) {
    int stopBits = 0;

    while (true) {
        cout << "Wybierz wartosc parametru StopBits (liczba bitow stopu):\n"
             << "-----------------------------------------------\n"
             << " [ 1] - 1 bit stopu\n"
             << " [ 2] - 1.5 bitu stopu\n"
             << " [ 3] - 2 bity stopu\n"
             << "-----------------------------------------------\n"
             << "Podaj numer opcji: ";
        cin >> stopBits;

        if (stopBits == 1) {
            dcb.StopBits = ONESTOPBIT;
            return;
        } else if (stopBits == 2) {
            dcb.StopBits = ONE5STOPBITS;
            return;
        } else if (stopBits == 3) {
            dcb.StopBits = TWOSTOPBITS;
            return;
        }

        cout << "Niepoprawna wartosc! Wybierz ponownie.\n";
    }
}


void setupTransmision(HANDLE hCommDev,DCB &dcb) {
    setBaudRate(dcb);
    setParity(dcb);
    setByteSize(dcb);
    setStopBits(dcb);
    SetCommState(hCommDev, &dcb);
    get_comm_state(hCommDev,dcb);
}
