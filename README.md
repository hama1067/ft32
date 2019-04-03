# ft32

<p align="center"><img width="75%" src="./demo.png"></p>

Fischertechnik TXT Controller replacement based on ESP32 microcontroller with a fancy web programmable HMI. The broswer IDE is rest upon [cody++](https://github.com/josephpal/cody-pp) version 1.0 (once ESP IDE). 

If you want to checkout the current cody++ web IDE version 1.0, please visit the demo [page](http://hit-karlsruhe.de/hit%2Dinfo/FT32/codypp/#/). The latest version is avaiable [here](http://hit-karlsruhe.de/hit%2Dinfo/FT32/codypp/#/).

## Installation

Beside the hardware setup we also have to install several libraries to work with the arduino ide. After that we can go on with preparing the ESP32 so that Codypp will work out of the box from the internal storage.

### Hardware and Arduino IDE setup

Before you can start, you have to install some dependencies und you need additional hardware. Beside the ESP32 microcontroller as the main component, you will need:

- [OLED](http://www.dx.com/p/0-96-128x64-i2c-interface-blue-color-oled-display-module-board-for-arduino-419232) Display
- [Motor](https://www.pololu.com/product/2137) driver

Please make sure you have connected all components correctly as it is mentioned on this [documentation page](http://hit-karlsruhe.de/hit-info/info-ws17/Fischertechnik/0404Portbelegungsplan.html).

Now you have to flash the ESP32 with the C++ software. Therefore you have to open the ft32.ino file from our github repository in the Arduino IDE (make sure you have setup up your [Arduino Environment](https://github.com/espressif/arduino-esp32) correctly), compile the source code and after that you have o flash the microcontroller.

Make sure you have installed the following arduino libraries via the internal arduino packet manager:

- Adafruit SSD1306 (latest version, tested with 1.2.9)
- Adafruit-GFX-Library (latest version, tested with 1.3.4)
- SparkFun SX1509 (latest version, tested with 2.0.1)

Now you can go on with writing the neccessary web files into the flash memory (called SPIFFS) of the ESP32. It might be difficult, but we have created some scripts to make it as easy as possible.

### Writing to SPIFFS Memory in Linux

Notification: Tested under Linux Mint 18 - 64bit OS.
Download the makeSPIFFS directory from our repository and open a terminal in this directory on your local machine. 
```javascript
$ cd makeSPIFFS
$ sudo chmod +x ./makeSpiffs.sh ./mkspiffs
$ ./makeSpiffs.sh
```
Make sure the ESP32 is connected correctly to your computer and no Arduino console is opened anymore. By default, the ESP32 should be connected to port /dev/ttyUSB0. For more information, please visit this documentation [page](http://hit-karlsruhe.de/hit-info/info-ws17/Fischertechnik-Software-HMI/0410Bedienungsanleitung.html).

<p align="center"><img width="55%" src="./makeSpiffs.png"></p>

### Writing to SPIFFS Memory in Windows

Is quite the same, but in windows, you have to install [python](https://www.python.org/downloads/) first. After that, you can execute the script. Make sure the ESP32 is connected correctly to your computer and no Arduino console is opened anymore. By default, the ESP32 should be connected to port COM4. Otherwise you have to change it in the script. For more information, please visit this documentation [page](http://hit-karlsruhe.de/hit-info/info-ws17/Fischertechnik-Software-HMI/0410Bedienungsanleitung.html).

That's it. Hope you enjoy our project. If you have questions or ideas, feel free to contact us.


## Web documentation

For further information to this project visit our official documentation [webpage](http://hit-karlsruhe.de/hit-info/info-ws17/Fischertechnik/).

<p align="center"><img width="35%" src="./FT32-robot.jpg"></p>

## FT32 Robots and Cody++ in use

Video dokumentation (in german): 
- [FT32-Codypp Pre-relase Video Dez. 2017](https://youtu.be/Vb2udfsyRjI?list=PLPAfNH8UB9_noRanNCsG2tC-wpglnxcxw)

German pubils testing our Robots: 
- [Hector-Seminar-Video](https://www.youtube.com/watch?v=EfkDjtZDj-Y&list=PLPAfNH8UB9_noRanNCsG2tC-wpglnxcxw&index=2)
- [Drawing a flower with Codypp and FT32](https://www.youtube.com/watch?v=YJIpydJqEwY&list=PLPAfNH8UB9_noRanNCsG2tC-wpglnxcxw&index=8)

