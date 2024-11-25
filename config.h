//
// Created by mateu on 25/11/2024.
//

#ifndef CONFIG_H
#define CONFIG_H
void get_comm_timeout(HANDLE hCommDev,COMMTIMEOUTS CommTimeouts);
void get_comm_state(HANDLE hCommDev,DCB dcb);
bool setTimeouts(HANDLE hCommDev);
void setupTransmision(HANDLE hCommDev,DCB &dcb);
#endif //CONFIG_H
