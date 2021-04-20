# Firmata-Schnittstelle

## TODO

Was noch fehlt:
- Text auf dem Display des Handschuhs anzeigen (Sysex string data oder Port?)
- Kontrolle der LED-Streifen (Port?)

## Digitale Ports

| Nummer | Beschreibung  |
|:------ |:------------- |
| 0x90   | Taster Daumen |
| 0x91   | Taster Finger |
| 0x92   | Gesten        |

### Port 0x90 (Daumen)

| Beschreibung |  ID | Bit Mask | junXion Pin |
|:------------ | ---:|:-------- |:----------- |
| Daumen 1     |   0 | 0x01     | Digital 0   |
| Daumen 2     |   1 | 0x02     | Digital 1   |
| Daumen 3     |   2 | 0x04     | Digital 2   |
| Daumen 4     |   3 | 0x08     | Digital 3   |

### Port 0x91 (Finger)

| Beschreibung     |  ID | Bit Mask | junXion Pin |
|:---------------- | ---:|:-------- |:----------- |
| Zeigefinger 1    |   4 | 0x01     | Digital 4   |
| Mittelfinger 1   |   5 | 0x02     | Digital 5   |
| Ringfinger 1     |   6 | 0x04     | Digital 6   |
| kleiner Finger 1 |   7 | 0x08     | Digital 7   |
| Zeigefinger 2    |   8 | 0x10     | Digital 12  |
| Mittelfinger 2   |   9 | 0x20     | Digital 13  |
| Ringfinger 2     |  10 | 0x40     | Digital 14  |
| kleiner Finger 2 |  11 | 0x80     | Digital 15  |

### Port 0x92 (Gesten)

| Beschreibung     |  ID | Bit Mask | junXion Pin |
|:---------------- | ---:|:-------- |:----------- |
| Hand nach links  |   0 | 0x01     | Digital 8   |
| Hand nach rechts |   1 | 0x02     | Digital 9   |
| Hand aufwärts    |   2 | 0x01     | Digital 10  |
| Hand abwärts     |   3 | 0x02     | Digital 11  |

## Analoge Pins

| Nummer | Beschreibung             |  ID | junXion Pin |
|:------ |:------------------------ | ---:|:----------- |
| 0xE0   | Beugung Zeigefinger      |   0 | Analog 0    |
| 0xE1   | Beugung Mittelfinger     |   1 | Analog 1    |
| 0xE2   | Beugung Ringfinger       |   2 | Analog 2    |
| 0xE3   | Beugung kleiner Finger   |   3 | Analog 3    |
| 0xE4   | Beschleunigung X         |   5 | Own Input 1 |
| 0xE5   | Beschleunigung Y         |   6 | Own Input 2 |
| 0xE6   | Beschleunigung Z         |   7 | Own Input 3 |
| 0xE7   | Orientierung Pitch       |   8 | Own Input 4 |
| 0xE8   | Orientierung Roll        |   9 | Own Input 5 |
| 0xE9   | Orientierung Yaw/Heading |  10 | Own Input 6 |
| 0xEA   | Distanzsensor            |  12 | Own Input 8 |
