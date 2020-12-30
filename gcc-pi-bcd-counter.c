
/*
 * gcc-pi-bcd-counter.c
 *
 * Demonstrates how to write data to the GPIO pins of a MCP23017 I2C I/O port
 * expander.  
 *
 * Outputs a BCD count on using the I/O port expander that can be used to to
 * display a decimal counter by connecting the outputs to a pair of 7-segment
 * LED displays using via a couple of 74LS47 BCD decoders or similar.
 *
 * Note: Do NOT use this code to drive LEDs directly from the MCP23017 as the 
 * total output current could easily exceed the maximum current rating for the
 * device.  If you want to drive multiple LEDs at the same time you need to use
 * a display driver or a transistor to switch the current.
 *
 * Compile with 'gcc -o gcc-pi-bcd-counter gcc-pi-bcd-counter.c'
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * 20 Jul 12   0.1   - Initial version - MEJT
 * 31 Jul 12   0.2   - Adopted the same register naming convention as used on
 *                     the product datasheet - MEJT
 *  1 Aug 12   0.3   - Added a seperate defination for the output port and data
 *                     direction register to allow the code to be easily altered
 *                     to use either port A or port B - MEJT
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>

#define DEVICE    "/dev/i2c-1"
#define ADDRESS   0x20

#define IODIRA    0x00
#define IODIRB    0x01
#define IPOLA     0x02
#define IPOLB     0x03
#define GPINTENA  0x04
#define GPINTENB  0x05
#define DEFVALA   0x06
#define DEFVALB   0x07
#define INTCONA   0x08
#define INTCONB   0x09
#define IOCON     0x0A
#define GPPUA     0x0C
#define GPPUB     0x0D
#define INTFA     0x0E
#define INTFB     0x0F
#define INTCAPA   0x10
#define INTCAPB   0x11
#define GPIOA     0x12
#define GPIOB     0x13
#define OLATA     0x14
#define OLATB     0x15

#define IODIRx    IODIRA  // Active data direction register.
#define GPIOx     GPIOA  // Active GPIO register.

#define DELAY     200000  // Delay between iterations.
#define LIMIT     1  // Number of time to display count.
#define MAX       99  // Upper limit of the counter.

void dumpbin(unsigned char byte) {

/*
 * Shift each bit and use a logical AND to mask the value, outputing the byte 
 * as two seperate nibbles.
 */
   int count;
   for(count = 7; count >= 4; count--) putchar('0' + ((byte >> count) & 1));
   putchar(' ');
   for(count = 3; count >= 0; count--) putchar('0' + ((byte >> count) & 1));
   putchar(' ');
}

int main(int argc, char **argv)
{
   int count = 0; // Counter.
   int loop = 0; // Counter.
   int bytes = 0; // Number of bytes in buffer.
   unsigned char data; // Data.

   int fd;  // File descrition.
   unsigned char buf[4];  // Buffer for data being read or written to the device.

/*
 * Open device for reading and writing - return an error on failure.
 */
	if ((fd = open(DEVICE, O_RDWR)) < 0) {
		printf("Failed to open device\n");
		exit(1);
	}
	if (ioctl(fd, I2C_SLAVE, ADDRESS) < 0) {
		printf("Unable to access device\n");
		exit(1);
	}

/*
 * Configure every GPIO pin on the selected port as an output.
 */
   bytes = 2;
   buf[0] = IODIRx;  // Adddress of active port's data direction register.
   buf[1] = 0x00;  // Set all the active port's GPIO pins to be outputs.
   if ((write(fd, buf, bytes)) != bytes) {
      printf("Error writing data\n");
      exit(1);
   }

/*
 * Output a BCD count to the selected port.
 */
   buf[0] = GPIOx;  // Adddress of the active GPIO port.
   for (loop = 0; loop < LIMIT; loop++) {
      for (count = 0; count < MAX+1; count++) {
         data = (((count/10) % 10) << 4) | (count % 10) ;  // Convert to BCD. 
         buf[1] = data ;  // Put data into buffer.
         if ((write(fd, buf, bytes)) != bytes) {  // Write data to the active port.
            printf("Error writing data\n");
            exit(1);
         }
         dumpbin (data);
         printf("%4.2u\n", count);
         usleep(DELAY);  // Slow things down a bit
      }
   }

/*
 * Reset all the GPIO pins on the selected port.
 */
   buf[0] = IODIRx;  // Adddress of active port's data direction register.
   buf[1] = 0xFF;  // Reset all the GPIO pins on the active port to be inputs.
   if ((write(fd, buf, bytes)) != bytes) {
      printf("Error writing data\n");
      exit(1);
   }
   close (fd);

	return 0;
}

