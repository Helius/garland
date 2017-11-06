# Christmas tree garland based on ws2812b strip and arduino pro mini

# Hardware
Arduino pro mini (Atmega168, 5V 16MHz), HC-06 bluetooth module, ws2812b strip on 100 led.

# Software
Src was written in C lang and compile with avr-gcc. As build system i tried to use CMake with very nice template from https://github.com/mkleemann/cmake-avr, thanks @mkleemann! It was an experiment and i really like CMake for this purpose now.

Also i used wonderful lib for strip https://github.com/Mattscheibe/ws2812b, thanks @Mattscheibe ! ) It really simple and just do what it has to do )

# Serial port command

Receive command byte by byte from serial port (baudrate is 115200), i use bluetooth to send command from android app.
```
8b[cmd] nx8b[value] (1-4)

#define C_BRIGHT   0x0 // set strip brightnes, value 8b[value 0..255]
#define C_MODE     0x1 // change mode, value 8b[mode] (0 - rainbow run, 1 - bt, 2 - random color run)
#define C_SPEED    0x2 // value 8b[loop delay x2 ms]
#define C_SET_LED  0x3 // set color of led by index, value 8b[index] 8b[R] 8b[G] 8b[B]
#define C_SET_ALL  0x4 // set all led color, value 8b[R] 8b[G] 8b[B]
#define C_WIDTH    0x5 // set how many led to use (from 0), value 8b[width]
```
# Install

After clone

``` bash
cd garland
mkdir build
cd build
cmake ../
# build
make
# program to arduino pro mini compatible board
make upload_garland
```
