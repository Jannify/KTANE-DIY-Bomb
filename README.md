# KTANE DIY Bomb

## Assembly

### Module

0: BigButton  
1: Wires  
2: MorseCode  
3: Memory  
 : Base  
4: SimonSays  
5: Password  

### Arduino Mega Pins

| Pins      | Hardware       | Function                                                              |
|-----------|----------------|-----------------------------------------------------------------------|
| A0  - A3  | Buttons        | Simon Says pressed (Blue, Red, Yellow, Green)                         |
| A4        | Input Buttons  | Morse Send                                                            |
| A5  - A6  | Input Buttons  | Morse Switch (left + right)                                           |
| A7  - A10 | Buttons        | NumberMemory pressed (left to right)                                  |
| A11 - A15 | Wires          | Wire Input                                                            |
|           |                |                                                                       |
| D2        | WS2812         | BigButton Button Color                                                |
| D3        | WS2812         | BigButton Strip Color                                                 |
|           |                |                                                                       |
| D4 - D13  | Input Buttons  | Password Switch Letters (left to right, top to bottom)                |
| D14       | Input Button   | Password Send                                                         |
| D15       | Button         | BigButton pressed                                                     |
|           |                |                                                                       |
| D16       | LED            | Yellow LED for Morse                                                  |
|           |                |                                                                       |
| D20 - D21 | I2C            |                                                                       |
|           |                |                                                                       |
| D41       | LED            | White LED for Indicator                                               |
| D42       | Register Data  | Data line for Memory 7-Segment                                        |
| D43 - D44 | Register Clock | 7Bit LEDs Memory Fourth to First Number                               |
| D45       | Register Clock | 7Bit LEDs Memory Big Number                                           |
| D46       | Register Clock | Memory Level LEDs (4x, bottom to top) + 2 XX + White LED Indicator    |
| D47       | Register Data  | Data line for LED Controllers                                         |
| D48       | Register Clock | Transistors for Simon Says Button LEDs (4x: Blue, Red, Yellow, Green) |
| D49 - D50 | Register Clock | Module solved LEDs (11x, left to right, top to bottom, front to back) |
| D51       | Buzzer         | Front Buzzer                                                          |
| D52 - D53 | Front Clock    | Front Clock (not I2C but Data, Clock)                                 |

### Arduino Mega I²C Multiplexer

- 0,96" OLED White  - Multiplex 0 - Indicator
- 2,40" OLED Yellow - Multiplex 1 - Morse
- 2,40" OLED Green  - Multiplex 2 - Password
- 0,91" OLED White  - Multiplex 3 - BigButton Text
- 1,30" OLED White  - Multiplex 4 - SerialNumber

## Communication

### Serial Communication (Pi to Arduino)

| Data |        Title        | Description                                | Length | Payload Data                                               |
|:----:|:-------------------:|--------------------------------------------|:------:|------------------------------------------------------------|
| 0x0  |        Reset        | Powers everything down                     |   0    |                                                            |
| 0x1  |      Init Bomb      | Init SerialNumber screen and Indicator     |   10   | [SN: 6 Letters] + [Indicator: LED + 3 Letters]             |
| 0x2  | Init static Modules | Init Morse BigButton                       |   3    | [Morse: Index] + [BigButton: ButtonColorIndex + TextIndex] |
| 0x3  |        Start        | Starts bomb with given time                |   2    | Timer in Seconds (ushort)                                  |
| 0x4  |      Set Tries      | On/Off X's LEDs                            |   1    | Number of Xs to display                                    |
| 0x5  |     Set Solved      | Updated green "Module Solved" LEDs         |   1    | Password -> BigButton (leading zero + 7Bit)                |
| 0x6  |   BigButton Strip   | Sets color of BigButton Strip              |   1    | ColorIndex (Red, Blue, Yellow, White) values               |
| 0x7  |    Password Text    | Sets Text of Password LCD                  |   5    | 5 Letters (Encoding TBD)                                   |
| 0x8  |      SimonSays      | Runs SimonSays sequence (length 3-6)       |   2    | 3Bit Length + zero + 6 * (2 Bit Button index)              |
| 0x9  |       Memory        | Displays number on 7Bit segments and level |   2    | 2bits per number: BigNumber + 4*Buttons + 4bit LevelLED    |
| 0xA  |                     |                                            |        |                                                            |
| 0xB  |                     |                                            |        |                                                            |
| 0xC  |                     |                                            |        |                                                            |
| 0xD  |                     |                                            |        |                                                            |
| 0xE  |                     |                                            |        |                                                            |
| 0xF  |                     |                                            |        |                                                            |

### Serial Communication (Arduino to Pi)

| Data |      Title       | Description                                               | Length | Payload Data                                                    |
|:----:|:----------------:|-----------------------------------------------------------|:------:|-----------------------------------------------------------------|
| 0x0  |     Reserved     |                                                           |        |                                                                 |
| 0x1  |  BigButton Down  | Reports how long the button was pressed and at which time |   3    | WasLongPressed (bool) + Seconds on Timer (ushort little endian) |
| 0x2  |     Wire Cut     | Current wire states                                       |   1    | Bitfield with which wire was cut                                |
| 0x3  | Password Changes | Password button was pressed                               |   1    | Send (no/yes) + Down(0)/Up(1) + 3 Bit Index of Letter (0-4)     |
| 0x4  |    SimonSays     | Reports pressed SimonSays button                          |   1    | Index of button pressed                                         |
| 0x5  |      Memory      | Memory button was pressed                                 |   1    | Index of button pressed                                         |
| 0x6  |    Morse Code    | Morse code answer send                                    |   1    | 0=failed, 1=succeed                                             |
| 0x7  |                  |                                                           |        |                                                                 |
| 0x8  |                  |                                                           |        |                                                                 |
| 0x9  |                  |                                                           |        |                                                                 |
| 0xA  |                  |                                                           |        |                                                                 |
| 0xB  |                  |                                                           |        |                                                                 |
| 0xC  |                  |                                                           |        |                                                                 |
| 0xD  |                  |                                                           |        |                                                                 |
| 0xE  |     Time Up      | Send when the clock reached zero                          |        |                                                                 |
| 0xF  |     Logging      | Writes a logging message to the pi                        |   X    | Message with line  ending                                       |
