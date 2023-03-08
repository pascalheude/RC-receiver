# RC-receiver with flight controller

The receiver is based on an Arduino Pro Mini

![](images/board.png)

The flight controller is a SkyStars-rc F405HD2

![](images/SkyStars-rc%20F405HD2.png)

The ESC 4 in 1 is a LANNRC BLHELI_S 45A 60A

![](images/LANNRC%20BLHELI_S%2045A%2060A%20ESC%204in1.jpg)

The Arduino is connected to the flight controller using the Futaba SBUS.  
As the Arduino is not able to invert the serial bus, requested by the SBUS protocol,
there is a need to add an inverter between the Arduino and the flight controller..  
A 3 cells LIPO baterry is used to provide power to ESC, flight controller and motor. 
THe battery is also used to provide power to the Arduino and the NRF24L01.
The battery monitor allows the Arduino to check if power supply is enough.  
The MPU and the GPS are provision. They are used when there is no flight controller.  
An LCD screen (2 lines of 16 characters) and LEDs provide information of receiver operation mode.  
Here is the synoptic of the receiver:
![](images/synoptic.png)

For the transmitter, see [](https://github.com/pascalheude/RC-transmitter)