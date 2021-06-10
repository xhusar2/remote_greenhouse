# MI-IOT_semestral_work

**Short task introduction**

Remotely controled Microgreens greenhouse

In this project I will use Arduino Uno in combination with ESP8266 wifi module to control little greenhouse through internet.
Greenhouse will contain number of sensors such as temperature and humidity meter or water level meter.

User will have full control over devices such as light, fan or water pump through website.


Website is deployed on Raspberry Pi 4 web server, which controls and communicates with Arduino Uno. Communication is done through custom Virtuino API which uses ESP8266 as sender/reciever of information (sensor values or control commands)


**Diagram**

![](photo/IOT_diagram.jpg?raw=true)

**Implementation steps:**

**Arduino setup**

For greenhouse controll I used arduino with combination of ESP8266 module.
Module is responsible for wifi communication with outside world (webserver).
Arduino is connected to relay which controls number of devices:
- 2 meter long LED grow light strip
- 12V water pump
- DHT22 sensor for temperature and humidity
- 12V 12cm PC fan


**Arduino script**

I used Virutino API (https://virtuino.com/index.php/virtuino/documentation/command-format) to communicate with webserver through ESP8266 module.
This API allowes users to define custom commands which operate on virtual shared memory.
Both arduino and webserver have access to this virtual array and can change its values.
For example I had to create command which sent state of LED light to the webserver and vice versa. This is done through http requests on esp8266 IP address. For example http://192.168.1.150:8000/&V13=1 command will change the value of the 13th cell in 
virtual array to value 1. Also we can send requests to get values with similar command using http://192.168.1.150:8000/&V13=?. I used this feature to get values from arduino sensors.


**Webserver**

I used python web framework Flash to create webserver. This webserver communicates with ESP8266 through above mentioned http requests and controls greenhouse. Website shows basic information and status of all devices and sensors inside greenhouse.
User can in real-time watch how temperature, water level and humidity change inside greenhouse. Also user can directly control LED light, FAN, and water pump.
Webserver is deployed on Raspberry Pi4 which provides convenient and easy access and availability of website.


![](photo/101475753_770547376814142_5626731590999605248_n.png?raw=true)

**End product**

![](photo/101638921_863141467496363_5220154151215824896_n.jpg?raw=true)

**Summary**

Project was great fun and I learned a lot. I did not have any experience prior to this project with microcontrollers and webservers/websites. I always wanted to try this project and this course gave me opportunity to realize it.
I think that this project was successful, everything works as intended. 

However, I have couple of improvements for the future developement, which I did not implement yet.
Greenhouse should be able to automaticaly control itself. This would be easily done through python scripts implemented in Raspberry Pi.
Scripts would periodically get values of sensors and based on those values, it would controll watering system, fans and lights.




