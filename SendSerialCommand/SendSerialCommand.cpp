// 
// This program sends a single byte to a serial port
// specified on the command line
//
// Creation date: April, 2016
// Copyright Cedar Lake Instruments
// 
//

#include "SendSerialCommand.h"

static const int COMM_FAIL = -2;

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		printf("Usage:\n SendSerialCommand port value\n");
		return 0;
	}

	char cmd[1];
	cmd[0] = atoi(argv[2]);
	
	CSerialLink serial;
	if (serial.SetupSerialPort(argv[1]))
	{
		serial.Write(cmd[0]);
	}
	else
	{
		printf("Could not connect to %s\n", argv[1]);
	}
    return 0;

}

CSerialLink::CSerialLink()
{
	m_hCom = INVALID_HANDLE_VALUE;
}

BOOL CSerialLink::Write(char text)
{
	BOOL result = false;
	DWORD written = 0;
	if (m_bPortReady)
	{
		result = WriteFile(m_hCom, &text, 1, &written, NULL);
		if (result == false)
		{
			printf("Error: %d\n", GetLastError());
		}
	}
	return result;
}

//
// Initialize for 9,600 bps, 8N1
// Connects to port passed in as parameter
//
BOOL CSerialLink::SetupSerialPort(char* port)
{
	if (strlen(port) > 5)
	{
		printf("Port name too long\n");
		return false;
	}
	if (m_hCom != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hCom);
	}
	m_bPortReady = FALSE;
	char portname[] = { '\\','\\','.','\\',0,0,0,0,0,0 };
	strcpy_s(&portname[4], 5, port);
	m_hCom = CreateFile("\\\\.\\COM5", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ,  NULL,  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,  NULL);

	if (m_hCom == INVALID_HANDLE_VALUE)
	{
		printf("Error: %d\n", GetLastError());
		m_bPortReady = FALSE;
		return FALSE;
	} // end if

	  // set buffer sizes
	if (!SetupComm(m_hCom, 80, 80))
	{
		return FALSE;
	}

	// Set parameters
	if (!GetCommState(m_hCom, &m_dcb))
	{
		return FALSE;
	}

	m_dcb.BaudRate = 9600;
	m_dcb.ByteSize = 8;
	m_dcb.Parity = NOPARITY;
	m_dcb.StopBits = ONESTOPBIT;
	// Keep going if errors encountered
	m_dcb.fAbortOnError = FALSE;
	m_dcb.fOutxCtsFlow = FALSE;
	m_dcb.fOutxDsrFlow = FALSE;

	if (!SetCommState(m_hCom, &m_dcb))
	{
		return FALSE;
	}

	// Set timeouts
	if (!GetCommTimeouts(m_hCom, &m_CommTimeouts))
	{
	}
	m_CommTimeouts.ReadIntervalTimeout = 0;
	m_CommTimeouts.ReadTotalTimeoutConstant = 50;
	m_CommTimeouts.ReadTotalTimeoutMultiplier = 0;
	m_CommTimeouts.WriteTotalTimeoutConstant = 0;
	m_CommTimeouts.WriteTotalTimeoutMultiplier = 0;
	if (!SetCommTimeouts(m_hCom, &m_CommTimeouts))
	{
		return FALSE;
	}

	m_bPortReady = TRUE;
	return TRUE;
}

