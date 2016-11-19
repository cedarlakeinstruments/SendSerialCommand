#pragma once
#include <windows.h>
#include <stdio.h>

class CSerialLink
{
public:
	CSerialLink();
	BOOL Write(char);
	BOOL SetupSerialPort(char*);

private:
	BOOL	m_bPortReady;
	HANDLE	m_hCom;
	DCB		m_dcb;
	COMMTIMEOUTS m_CommTimeouts;
};
