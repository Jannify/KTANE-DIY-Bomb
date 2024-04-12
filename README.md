# KTANE DIY Bomb

## Aufbau

### Module

0: BigButton  
1: Wires  
2: Password  
3: Simon says  
4: (Number) Memory  
5: Morse Code

### Arduino Mega Pins

| Pins      | Hardware       | Function                                                                    |
|-----------|----------------|-----------------------------------------------------------------------------|
| A0 - A5   | Wires          | Wire Input                                                                  |
| A6        | Button         | BigButton pressed                                                           |
| A7 - A10  | Buttons        | Simon Says pressed (Blue, Red, Yellow, Green)                               |
| A11 - A14 | Buttons        | NumberMemory pressed (left to right)                                        |
| A15       | Button         | Morse Send pressed                                                          |
|           |                |                                                                             |
| D2        | WS2812         | BigButton Button Color                                                      |
| D3        | WS2812         | BigButton Strip Color                                                       |
| D4        | Register Data  |                                                                             |
| D5        | Register Clock | 7Bit LEDs Memory Big Number                                                 |
| D6 - D9   | Register Clock | 7Bit LEDs Memory First to Fourth Button                                     |
| D10 - D13 | LEDs           | Memory Level Indicator (4x, bottom to top)                                  |
|           |                |                                                                             |
| D14 - D17 | LEDs           | Transistors for Simon Says Buttons (4x: Blue, Red, Yellow, Green)           |
| D18 - D19 | LEDs           | 2 Red LEDs for XX                                                           |
| D20 - D21 | I2C            |                                                                             |
|           |                |                                                                             |
| D22 - D32 | LEDs           | 11 Green "Module solved" LEDs (left to right, top to bottom, front to back) |
| D33       | Reset          | Jump wire to arduino reset pin                                              |
| D34       | LED            | White LED for Indicator                                                     |
| D35       | LED            | Yellow LED for Morse                                                        |
| D36 - D37 | Front Clock    | Front Clock (not I2C but Data, Clock)                                       |
| D38       | Buzzer         | Front Buzzer                            OLD OLD OLD OLD                     |
|           |                |                                                                             |
| D41 - D42 | Input Buttons  | Morse Switch (left + right)                                                 |
| D43       | Input Button   | Password Send                                                               |
| D44 - D53 | Input Buttons  | Password Switch Letters (left to right, top to bottom)                      |

\
I2C

- 0,96" OLED White  - Multiplex 0 - Indicator
- 2,40" OLED Yellow - Multiplex 1 - Morse
- 2,40" OLED Green  - Multiplex 2 - Password
- 0,91" OLED White  - Multiplex 3 - BigButton Text
- 1,30" OLED White  - Multiplex 4 - SerialNumber

## Serial Communication (Pi to Arduino)

| Data |      Title      | Description                                       | Length | Payload Data                                                                                                       |
|:----:|:---------------:|---------------------------------------------------|:------:|--------------------------------------------------------------------------------------------------------------------|
| 0x0  |      Reset      | Resets all configurations, powers everything down |   0    |                                                                                                                    |
| 0x1  |      Init       | Init SerialNumber screen, Indicator and BigButton |   13   | [SN: 6 Letters] + [Indicator: LED on/off + 3 Letters] + [Morse: Index] + [BigButton: ButtonColorIndex + TextIndex] |
| 0x2  |      Start      | Starts bomb with given time                       |   2    | Timer in Seconds (ushort)                                                                                          |
| 0x3  |    Set Tries    | On/Off X's LEDs                                   |   1    | Number of Xs to display                                                                                            |
| 0x4  |   Set Solved    | Updated green "Module Solved" LEDs                |   2    | Bitfield of Modules solved (5 zeros + from 10 to 0)                                                                |
| 0x5  | BigButton Strip | Sets color of BigButton Strip                     |   1    | ColorIndex (Red, Blue, Yellow, White) values                                                                       |
| 0x6  |  Password Text  | Sets Text of Password LCD                         |   5    | 5 Letters (Encoding TBD)                                                                                           |
| 0x7  |    SimonSays    | Runs SimonSays sequence (length 3-6)              |   2    | 3Bit Length + zero + 6 * (2 Bit Button index)                                                                      |
| 0x8  |     Memory      | Displays number on 7Bit segments and level        |   2    | 2bits per number: BigNumber + 4*Buttons + 3bits Level                                                              |
| 0x9  |                 |                                                   |        |                                                                                                                    |
| 0xA  |                 |                                                   |        |                                                                                                                    |
| 0xB  |                 |                                                   |        |                                                                                                                    |
| 0xC  |                 |                                                   |        |                                                                                                                    |
| 0xD  |                 |                                                   |        |                                                                                                                    |
| 0xE  |                 |                                                   |        |                                                                                                                    |
| 0xF  |                 |                                                   |        |                                                                                                                    |

## Serial Communication (Arduino to Pi)

| Data |      Title       | Description                                               | Length | Payload Data                                                |
|:----:|:----------------:|-----------------------------------------------------------|:------:|-------------------------------------------------------------|
| 0x0  |     Reserved     |                                                           |        |                                                             |
| 0x1  |  BigButton Down  | Reports how long the button was pressed and at which time |   3    | TimePressed (0=short, 1=long) + Seconds on Timer (ushort)   |
| 0x2  |     Wire Cut     | Current wire states                                       |   1    | Bitfield with which wire was cut                            |
| 0x3  | Password Changes | Password button was pressed                               |   1    | Send (no/yes) + Down(0)/Up(1) + 3 Bit Index of Letter (0-4) |
| 0x4  |    SimonSays     | Reports pressed SimonSays button                          |   1    | Index of button pressed                                     |
| 0x5  |      Memory      | Memory button was pressed                                 |   1    | Index of button pressed                                     |
| 0x6  |    Morse Code    | Morse code answer send                                    |   1    | 0=failed, 1=succeed                                         |
| 0x7  |                  |                                                           |        |                                                             |
| 0x8  |                  |                                                           |        |                                                             |
| 0x9  |                  |                                                           |        |                                                             |
| 0xA  |                  |                                                           |        |                                                             |
| 0xB  |                  |                                                           |        |                                                             |
| 0xC  |                  |                                                           |        |                                                             |
| 0xD  |                  |                                                           |        |                                                             |
| 0xE  |                  |                                                           |        |                                                             |
| 0xF  |     Time Up      | Send when the clock reached zero                          |        |                                                             |
