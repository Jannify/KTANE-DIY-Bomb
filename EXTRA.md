# EXTRA

## Notes

- A11 & A15 no pullup

## TODO

- Glue Buzzer Resistor and LED legs
- Morse LED driver
- Wire PasswordModule
- Generate own font for BigButton Display

## Links

https://blogs.iad.zhdk.ch/interactive-prototyping-methods-fs13/arduino-transistor-als-schalter/index.html
https://www.mikrocontroller.net/articles/Basiswiderstand#Beispiele
https://www.digikey.com/en/resources/conversion-calculators/conversion-calculator-led-series-resistor
https://www.elektronik-kompendium.de/sites/praxis/bauteil_uln2003a.htm

## LED-Resistor-Data

green 2,45 fV   => 130 Ohm bei 5V & 30 Ohm bei 3V
white 2,7 fV    => 115 Ohm bei 5V & 15 Ohm bei 3V
red 1,83 fV     => 160 Ohm bei 5V & 60 Ohm bei 3V
yellow 1,9 fV   => 155 Ohm bei 5V & 55 Ohm bei 3V
blue 2,63 fV    => 120 Ohm bei 5V & 20 Ohm bei 3V

1x4 Green : Memory
1x4 0 Ohm: Simon Says
1x7 1x4 Green Solved
1x3 Base (white, 2x red)

~~3x 160 Ohm for 2xRed + Yellow~~
11x 130 Ohm Ohm for Green
 5x 120 Ohm for 4x blue + 1x white

## iptables for Pi

iptables -A INPUT -i eth0 -p tcp --dport 80 -j ACCEPT
iptables -A INPUT -i eth0 -p tcp --dport 8080 -j ACCEPT
iptables -A PREROUTING -t nat -i eth0 -p tcp --dport 80 -j REDIRECT --to-port 8080
