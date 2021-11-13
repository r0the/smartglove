# Max-Schnittstelle

## Beschreibung

Der SmartGlove schickt automatisch Nachrichten über die serielle Schnittstelle. Jede Nachricht hat eine fixe Länge.

## Analoge Werte

Analoge Werte werden mit zwei Bytes dargestellt. Das erste Byte wird mit 256 multipliziert und zum zweiten Byte addiert.

## Aufbau einer Nachricht

Eine Nachricht beginnt immer mit dem Wert 83 (S für Start). Es folgt der Typ der Nachricht und die Länge der gesamten Nachricht in Bytes. Anschliessend kommt der Inhalt der Nachricht und abschliessend der Wert 69 (E für Ende).

| Position | Bedeutung | Wert   | Hex |
|:-------- |:--------- |:------ |:--- |
| 1        | Start     | 83 (S) | $53 |
| 2        | Art       | ...    | ... |
| 3        | Länge     | ...    | ... |
| ...      | Inhalt    | ...    | ... |
| n        | Ende      | 69 (E) | $45 |

## Nachrichten-Typen

Es gibt folgende Nachrichten-Typen:

| Wert | Hex | Buchstabe | Bedeutung                                     |
|:---- |:--- |:--------- |:--------------------------------------------- |
| 73   | $49 | I         | Information (Geräte-Typ und Firmware-Version) |
| 68   | $44 | D         | Digitale Daten (Zustand der Knöpfe)           |
| 65   | $41 | A         | Analoge Daten (Sensoren)                      |
| 83   | $53 | S         | State setzen                                  |

## Nachricht Information (I)

Gleich nach dem Aufbau der Verbindung schickt der SmartGlove eine Infomrations-Nachricht mit Gerätetyp und Firmware-Version.

| Beschreibung     | Pos | Bytes | Wert                |
|:---------------- |:---:|:-----:|:------------------- |
| Beginn           |  1  |   1   | 83 (S)              |
| Art              |  2  |   1   | 73 (I)              |
| Länge            |  3  |   1   | 10                  |
| Gerätetyp        |  4  |   1   | siehe unten         |
| Firmware-Version |  5  |   1   | Hauptversionsnummer |
| Firmware-Version |  6  |   1   | Nebenversionsnummer |
| (Reserve)        |  7  |   1   |                     |
| (Reserve)        |  8  |   1   |                     |
| (Reserve)        |  9  |   1   |                     |
| Ende             | 10  |   1   | 69 (E)              |

Es gibt folgende Gerätetypen:

| Beschreibung | Wert   |
|:------------ |:------ |
| SmartGlove   | 71 (G) |
| SmartBall    | 66 (B) |


## Nachricht Digital (D)

In regelmässigen Abständen schickt der SmartGlove die Zustände der Druckknöpfe sowie die Information zu erkannten Gesten.

| Beschreibung     | Pos | Bytes | Wert   | junXion Pin |
|:---------------- |:---:|:-----:|:------ |:----------- |
| Beginn           |  1  |   1   | 83 (S) |             |
| Art              |  2  |   1   | 68 (D) |             |
| Länge            |  3  |   1   | 20     |             |
| Daumen 1         |  4  |   1   | 0/1    | Digital 0   |
| Daumen 2         |  5  |   1   | 0/1    | Digital 1   |
| Daumen 3         |  6  |   1   | 0/1    | Digital 2   |
| Daumen 4         |  7  |   1   | 0/1    | Digital 3   |
| Zeigefinger 1    |  8  |   1   | 0/1    | Digital 4   |
| Mittelfinger 1   |  9  |   1   | 0/1    | Digital 5   |
| Ringfinger 1     | 10  |   1   | 0/1    | Digital 6   |
| kleiner Finger 1 | 11  |   1   | 0/1    | Digital 7   |
| Hand nach links  | 12  |   1   | 0/1    | Digital 8   |
| Hand nach rechts | 13  |   1   | 0/1    | Digital 9   |
| Hand aufwärts    | 14  |   1   | 0/1    | Digital 10  |
| Hand abwärts     | 15  |   1   | 0/1    | Digital 11  |
| Zeigefinger 2    | 16  |   1   | 0/1    | Digital 12  |
| Mittelfinger 2   | 17  |   1   | 0/1    | Digital 13  |
| Ringfinger 2     | 18  |   1   | 0/1    | Digital 14  |
| kleiner Finger 2 | 19  |   1   | 0/1    | Digital 15  |
| Ende             | 20  |   1   | 69 (E) |             |

## Nachricht Analog (A)

In regelmässigen Abständen schickt der SmartGlove die aktuellen Werte der Sensoren:

| Beschreibung             | Pos | Bytes | Wert   | junXion Pin |
|:------------------------ |:---:|:-----:|:------ |:----------- |
| Beginn                   |  1  |   1   | 83 (S) |             |
| Art                      |  2  |   1   | 65 (A) |             |
| Länge                    |  3  |   1   | 26     |             |
| Distanzsensor            |  4  |   2   | analog | Own Input 1 |
| Beschleunigung X         |  6  |   2   | analog | Own Input 2 |
| Beschleunigung Y         |  8  |   2   | analog | Own Input 3 |
| Beschleunigung Z         | 10  |   2   | analog | Own Input 4 |
| Orientierung Pitch       | 12  |   2   | analog | Own Input 5 |
| Orientierung Roll        | 14  |   2   | analog | Own Input 6 |
| Orientierung Yaw/Heading | 16  |   2   | analog | Own Input 7 |
| Beugung Zeigefinger      | 18  |   2   | analog | Analog 0    |
| Beugung Mittelfinger     | 20  |   2   | analog | Analog 1    |
| Beugung Ringfinger       | 22  |   2   | analog | Analog 2    |
| Beugung kleiner Finger   | 24  |   2   | analog | Analog 3    |
| Ende                     | 26  |   1   | 69 (E) |             |

## Nachricht State (S)

Mit dieser Nachricht kann dem Smartglove einen Wert (Zahl zwischen 0 und 255) übermittelt werden, welcher auf dem Display angezeigt werden soll.

| Beschreibung | Pos | Bytes | Wert    |
|:------------ |:---:|:-----:|:------- |
| Beginn       |  1  |   1   | 83 (S)  |
| Art          |  2  |   1   | 83 (S)  |
| Länge        |  3  |   1   | 5       |
| Wert         |  4  |   1   | 0 - 255 |
| Ende         |  5  |   1   | 69 (E)  |
