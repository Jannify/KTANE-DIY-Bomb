# KTANE DIY Bomb

## Assembly

### Module

0: BigButton  
1: Wires  
2: Password  
3: SimonSays  
 : Base
4: Memory  
5: Morse Code

### Arduino Mega Pins

#### !Currently not updated in code!

| Pins      | Hardware       | Function                                                              |
|-----------|----------------|-----------------------------------------------------------------------|
| A0  - A4  | Wires          | Wire Input                                                            |
| A5  - A6  | Input Buttons  | Morse Switch (left + right)                                           |
| A7        | Input Buttons  | Morse Send                                                            |
| A8  - A11 | Buttons        | Simon Says pressed (Blue, Red, Yellow, Green)                         |
| A12 - A15 | Buttons        | NumberMemory pressed (left to right)                                  |
|           |                |                                                                       |
| D2        | WS2812         | BigButton Button Color                                                |
| D3        | WS2812         | BigButton Strip Color                                                 |
|           |                |                                                                       |
| D20 - D21 | I2C            |                                                                       |
|           |                |                                                                       |
| D30       | LED            | Yellow LED for Morse                                                  |
|           |                |                                                                       |
|           |                |                                                                       |
| D43 - D44 | Register Clock | 7Bit LEDs Memory Fourth to First Number                               |
| D45       | Register Clock | 7Bit LEDs Memory Big Number                                           |
| D46       | Register Clock | Memory Level LEDs (4x, bottom to top) + 2 XX + White LED Indicator    |
| D47       | Register Data  |                                                                       |
| D48       | Register Clock | Transistors for Simon Says Button LEDs (4x: Blue, Red, Yellow, Green) |
| D49       | Buzzer         | Front Buzzer                                                          |
| D50 - D51 | Front Clock    | Front Clock (not I2C but Data, Clock)                                 |
| D52 - D53 | Register Clock | Module solved LEDs (11x, left to right, top to bottom, front to back) |

|               | Button         | BigButton pressed                                                     |
| D43           | Input Button   | Password Send                                                         |
| D44 - D53     | Input Buttons  | Password Switch Letters (left to right, top to bottom)                |
| D40       | Reset          | Jump wire to arduino reset pin           MAYBE NOT NEEDED?            |

### Arduino Mega I²C Multiplexer

- 0,96" OLED White  - Multiplex 0 - Indicator
- 2,40" OLED Yellow - Multiplex 1 - Morse
- 2,40" OLED Green  - Multiplex 2 - Password
- 0,91" OLED White  - Multiplex 3 - BigButton Text
- 1,30" OLED White  - Multiplex 4 - SerialNumber

## Communication

### Serial Communication (Pi to Arduino)

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

### Serial Communication (Arduino to Pi)

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
