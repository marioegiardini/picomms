#pragma once
#include <Windows.h>
#include <string>


class Picomms
{
public:
    // Constructor: opens the USB communications with the Pico 
    // using the virtual serial port <serialPortId>.
    Picomms(int serialPortId);

    // Sends a message to the Pico appending '\r', such as
    // required by the Python input() statement, and returns the echo.
    std::string SendLine(std::string message);

    // Receives a line terminated by '\r' from the Pico, 
    // such as returned by the Python print() statement,
    // stripping any eventual '\r' and '\n' at the end.
    std::string ReceiveLine();

    // Sends an empty string to the Pico. This can be read
    // by the Pico using the input() statement
    // as a signal to do something, such as reading the sensors.
    void Signal();

    // Destructor: closes the USB communications with the Pico.
    ~Picomms();

private:
    // Handle to the serial port.
    HANDLE hSerial;
};
