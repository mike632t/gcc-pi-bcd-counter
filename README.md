## gcc-pi-bcd-counter - Drive a 7 segment BCD didplay.

Written in C. 

Demonstrates how to write data to the GPIO pins of a MCP23017 I2C I/O port
expander.  

Outputs a BCD count on using the I/O port expander that can be used to to
display a decimal counter by connecting the outputs to a pair of 7-segment
LED displays using via a couple of 74LS47 BCD decoders or similar.

Note: Do NOT use this code to drive LEDs directly from the MCP23017 as the 
total output current could easily exceed the maximum current rating for the
device.  If you want to drive multiple LEDs at the same time you need to use
a display driver or a transistor to switch the current.

