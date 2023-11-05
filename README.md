# Overview
here is my little project:
through this project I am trying to remotely control the opening of a valve for a hydroelectric turbine and to be able to measure the level of a water reserve

![alt text](assets/PXL_20220916_162324473.jpg "Title")
```mermaid
graph TD;
    Home-->Turbine;
    Home-->Pond;
    
```

# Master
# Turbine
i2c 
ina260 0x40
ina219 0x44 
ads1115 0x48
bmp180 0x77
ssd1306 0x3C
# Etang

## VL53L1X
Fil | Fct | broche
---|----|----
brun|GND|
bleu|5V|
orange|SCL|
voilet|SDA|