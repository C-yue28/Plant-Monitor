<h1>Plant Monitor</h1> 


This smart, fully automated plant monitor system monitors soil moisture, atmospheric temperature/pressure, and humidity to determine when to water the plant, and then uses a small pump to water the plant. This is meant to be a project where I learn how to design a PCB, and it is also my first project with HackClub so I am getting used to this type of work. I plan to build upon this project in the future and create an automated gardening system. 

The PCB in this project uses an RP2040 microcontroller. The power can either be supplied through USB connection or battery; when the USB is plugged in it charges the battery and takes priority in powering the RP2040 as well. The main external modules are: BME280 for temperature/humidity/pressure sensing, SSD1315 OLED display to display some basic information to user such as soil moisture/time till next watering, Maker Soil Moisture Sensor, 1S LiPo battery for power, and 4.5V DC water pump for a cheap way to water the plant. I also found some cheap tubing for the water pump as well. Below are pictures of the components:



