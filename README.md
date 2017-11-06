# Christmas tree garland based on ws2812b strip and arduino pro mini

# Hardware
Arduino pro mini, HC-06 bluetooth module, ws2812b strip. Right now there is now bluetooth support because my 3.3V LDO doesn't work, will fix it asap and start Android app to communicate from phone to strip.

# Software
Src was written in C lang and compile with avr-gcc. As build system i tried to use CMake with very nice template from https://github.com/mkleemann/cmake-avr, thanks @mkleemann! It was an experiment and i really like CMake for this purpose now.

Also i used wonderful lib for strip https://github.com/Mattscheibe/ws2812b, thanks @Mattscheibe ! ) It really simple and just do what it has to do )


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
make make upload_garland
```
