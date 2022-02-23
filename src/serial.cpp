#include "../include/serial.h"


void SerialRead()
{
    HANDLE hSerial;
    hSerial = CreateFile(TEXT("COM3"),
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, //FILE_FLAG_OVERLAPPED,
        NULL);
    if (hSerial == INVALID_HANDLE_VALUE)
    {
        if (GetLastError() == ERROR_FILE_NOT_FOUND)
        {
            //printf("Serial port does not exist.\n");
        }
        //printf("Some other error occurred.\n");
    }
    else
    {

        DCB dcbSerialParams = { 0 };
        dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
        if (!GetCommState(hSerial, &dcbSerialParams))
        {
            //printf("Error getting state\n");
        }

        FillMemory(&dcbSerialParams, sizeof(dcbSerialParams), 0);
        dcbSerialParams.BaudRate = CBR_9600;
        dcbSerialParams.ByteSize = 8;
        dcbSerialParams.StopBits = ONESTOPBIT;
        dcbSerialParams.Parity = NOPARITY;
        if (!SetCommState(hSerial, &dcbSerialParams))
        {
           //printf("Error setting serial port state\n");
        }

        COMMTIMEOUTS timeouts = { 0 };
        timeouts.ReadIntervalTimeout = MAXDWORD;
        timeouts.ReadTotalTimeoutConstant = 20;
        timeouts.ReadTotalTimeoutMultiplier = 0;
        timeouts.WriteTotalTimeoutConstant = 0;
        timeouts.WriteTotalTimeoutMultiplier = 0;

        if (!SetCommTimeouts(hSerial, &timeouts))
        {
            //printf("Error occureed(timeError)\n");
        }

        char szBuff[32] = { 0 };
        DWORD dwBytesRead = 0;
        if (!ReadFile(hSerial, szBuff, 4, &dwBytesRead, NULL))
        {
            //printf("Error occurred. Report to user.\n");
        }


        SetCommMask(hSerial, EV_RXCHAR);
        DWORD dwCommStatus = 1;
        while (1)
        {
            WaitCommEvent(hSerial, &dwCommStatus, 0);

            if (dwCommStatus & EV_RXCHAR)
            {
                memset(szBuff, 0, sizeof(szBuff));
                int read = ReadFile(hSerial, LPVOID(szBuff), 32, &dwBytesRead, NULL);

                printf("%s", szBuff);
            }
        }

        CloseHandle(hSerial);
    }
}




