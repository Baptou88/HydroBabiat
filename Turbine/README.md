# Pins

| 1  | 2   | 3   | 4   | 5   | 6  | 7  | 8   | 9   | 10   | 11 | 12 | 13 | 14  | 15 | 16   | 17   | 18 | 19 | 20  |
|----|-----|-----|-----|-----|----|----|-----|-----|------|----|----|----|-----|----|------|------|----|----|-----|
| 5V | 3v3 | GND | FCF | FCO | PG | ENC| ENC |     | Tacky|    | SDA Jaune|    | RST |    |      |      | SW | DT | CLK |
|    |     |     |     |     |    |    |     |     |      |    |    |    |     |    | MotR | MotL |    |    |     |

18/08/24 - tachy deplacer de pin 21 à pin 3

# Address I2C

-

SSD1306 GND - VCC - SCL - SDA


[sol motor controllers](https://www.solomotorcontrollers.com/)

Tachy 
Noir -> Vert -> Signal (breadbord 10)
bleu -> bleu -> -
brun -> violet -> +

## probleme tachy:
essayer detachinterrupt
essayer de repacer par millis
essayer mutex

[RPM] 230.20   [dt] 260638
[RPM] 230.20   [dt] 260638
[RPM] 22329.74   [dt] 2687
[RPM] 22329.74   [dt] 2687
[RPM] 22329.74   [dt] 2687
[RPM] 22329.74   [dt] 2687
[RPM] 232.43   [dt] 258140
[RPM] 232.43   [dt] 258140
[RPM] 232.43   [dt] 258140
[RPM] 230.09   [dt] 260773
[RPM] 230.09   [dt] 260773
[RPM] 230.09   [dt] 260773
[RPM] 230.09   [dt] 260773
[RPM] 22388.06   [dt] 2680


## pref
print pref :
CpuFreq 20
ouvertureMax 8000
moteurKp 2.00
moteurKi 0.00
moteurKd 0.00
ledNotif 0
voltage_coefA 0.01
voltage_base -1151.00
current_coefA 0.00
current_base -20608.00
MaxIMot 8000.00
maxSpeed 400
tackyDebounce 5000