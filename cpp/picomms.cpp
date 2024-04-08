#include <Windows.h>
#include <stdexcept>
#include "picomms.h"


Picomms::Picomms(int serialPortId)
{
    // Convert serialPortId to a string
    std::string portName = "\\\\.\\COM" + std::to_string(serialPortId);

    // Open the serial port
    hSerial = CreateFileA(
        portName.c_str(), 
        GENERIC_READ | GENERIC_WRITE, 
        0, 
        NULL, 
        OPEN_EXISTING, 
        0, 
        NULL);

    if (hSerial == INVALID_HANDLE_VALUE)
    {
        throw std::runtime_error("Could not open the Pico serial port");
    }

    // Set up the serial port parameters
    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hSerial, &dcbSerialParams))
    {
        CloseHandle(hSerial);
        throw std::runtime_error("Could not get the state of the Pico serial port");
    }

    dcbSerialParams.BaudRate = CBR_115200; // Set baud rate to 115200
    dcbSerialParams.ByteSize = 8; // 8 data bits
    dcbSerialParams.StopBits = ONESTOPBIT; // 1 stop bit
    dcbSerialParams.Parity = NOPARITY; // No parity
    if (!SetCommState(hSerial, &dcbSerialParams))
    {
        CloseHandle(hSerial);
        throw std::runtime_error("Could not set the state of the Pico serial port");
    }

    // Set up timeouts
    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    if (!SetCommTimeouts(hSerial, &timeouts))
    {
        CloseHandle(hSerial);
        throw std::runtime_error("Could not set the Pico serial port timeouts");
    }

    // Clear the serial port
    PurgeComm(hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
}


std::string Picomms::SendLine(std::string message)
{
    // Send the message appending '\r' to it
    message += '\r';
    DWORD bytesWritten;
    if (!WriteFile(hSerial, message.c_str(), message.size(), &bytesWritten, NULL))
    {
        throw std::runtime_error("Could not write to the Pico serial port");
    }
    if (bytesWritten != message.size())
    {
		throw std::runtime_error("Could not write all bytes to the Pico serial port");
	}

    // Receive the echo
    return ReceiveLine();
}


std::string Picomms::ReceiveLine()
{
    // Read the message one character at a time until '\n'
	std::string received;
	char buffer;
	DWORD bytesRead;
    do
    {
        if (!ReadFile(hSerial, &buffer, 1, &bytesRead, NULL))
        {
			throw std::runtime_error("Could not read from the Pico serial port");
		}
        if (bytesRead == 0)
        {
			throw std::runtime_error("Could not read from the Pico serial port");
		}
		received += buffer;
	} while (buffer != '\n');

    // Strip the '\r' and eventual '\n' at the end of the string
    while (!received.empty() && (received.back() == '\r' || received.back() == '\n')) {
        received.pop_back();
    }

	return received;
}


void Picomms::Signal()
{
	SendLine("");
}


Picomms::~Picomms()
{
	CloseHandle(hSerial);
}
