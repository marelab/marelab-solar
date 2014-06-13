/*
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Name        : PCA9685.cpp
 * Author      : Georgi Todorov
 * Version     :
 * Created on  : Dec 9, 2012
 *
 * Copyright © 2012 Georgi Todorov  <terahz@geodar.com>
 */

#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <stdio.h>      /* Standard I/O functions */
#include <fcntl.h>
#include <syslog.h>		/* Syslog functionallity */
#include <inttypes.h>
#include <errno.h>
#include <math.h>

#include "PCA9685.h"

void PCA9685::setName(string hardwarename){
	chipname = hardwarename;
}

string PCA9685::getName(){
	return chipname;
}


//! Constructor takes bus and address arguments
/*!
 \param bus the bus to use in /dev/i2c-%d.
 \param address the device address on bus
 */
PCA9685::PCA9685() {
	//reset();
	//setPWMFreq(1000);
}

PCA9685::PCA9685(string bus, int address) {
	i2c = new I2C(bus,address);
	//reset();
	//setPWMFreq(1000);
}

PCA9685::~PCA9685() {
	delete i2c;
}
//! Sets PCA9685 mode to 00
void PCA9685::reset() {

		i2c->write_byte(MODE1, 0x00); //Normal mode
		i2c->write_byte(MODE2, 0x04); //totem pole

}
//! Set the frequency of PWM
/*!
 \param freq desired frequency. 40Hz to 1000Hz using internal 25MHz oscillator.
 */
void PCA9685::setPWMFreq(int freq) {

		uint8_t prescale_val = (CLOCK_FREQ / 4096 / freq)  - 1;
		i2c->write_byte(MODE1, 0x10); //sleep
		i2c->write_byte(PRE_SCALE, prescale_val); // multiplyer for PWM frequency
		i2c->write_byte(MODE1, 0x80); //restart
		i2c->write_byte(MODE2, 0x04); //totem pole (default)
}

//! PWM a single channel
/*!
 \param led channel to set PWM value for
 \param value 0-4095 value for PWM
 */
void PCA9685::setPWM(uint8_t led, int value) {
	setPWM(led, 0, value);
}
//! PWM a single channel with custom on time
/*!
 \param led channel to set PWM value for
 \param on_value 0-4095 value to turn on the pulse
 \param off_value 0-4095 value to turn off the pulse
 */
void PCA9685::setPWM(uint8_t led, int on_value, int off_value) {
		i2c->write_byte(LED0_ON_L + LED_MULTIPLYER * (led - 1), on_value & 0xFF);
		i2c->write_byte(LED0_ON_H + LED_MULTIPLYER * (led - 1), on_value >> 8);
		i2c->write_byte(LED0_OFF_L + LED_MULTIPLYER * (led - 1), off_value & 0xFF);
		i2c->write_byte(LED0_OFF_H + LED_MULTIPLYER * (led - 1), off_value >> 8);
}

int PCA9685::getPWM(uint8_t led){
	int ledval = 0;
	ledval = i2c->read_byte(LED0_OFF_H + LED_MULTIPLYER * (led-1));
	ledval = ledval & 0xf;
	ledval <<= 8;
	ledval += i2c->read_byte(LED0_OFF_L + LED_MULTIPLYER * (led-1));
	return ledval;
}

string PCA9685::getHARDWARE_HTMLConfig(void) {
	string output;
	output = output + "  <tr>";
	output = output + "    <td>&nbsp;</td>";
	output = output + "  </tr>";
	output = output + "  <tr>";
	output =
			output
					+ "    <td><table width='100%' border='0' cellpadding='4' cellspacing='4'>";
	output = output + "      <tr>";
	output = output + "        <td bgcolor='#0099CC'>PCA9685</td>";
	output =
			output
					+ "        <td colspan='2' align='right' bgcolor='#0099CC'><select name='AdapterSelect2' id='AdapterSelect2'>";
	output = output + "          <option value='LedDimmer'>LedDimmer</option>";
	output = output + "          <option value='Osmosis'>Osmosis</option>";
	output = output + "        </select></td>";
	output =
			output
					+ "        <td align='right' bgcolor='#0099CC'><img src='img/del.png' width='32' height='32' alt='ghg' /></td>";
	output = output + "      </tr>";
	output = output + "      <tr>";
	output = output + "        <td width='27%' bgcolor='#0099CC'>Adr:";
	output = output + "          <label for='ID_I2CADRESS'></label>";
	output =
			output
					+ "          <input name='ID_I2CADRESS2' type='text' id='ID_I2CADRESS' size='3' maxlength='3' /></td>";
	output = output + "        <td colspan='2' bgcolor='#0099CC'><p>Freq.:";
	output =
			output
					+ "          <input name='ID_FREQUENZ2' type='text' id='ID_FREQUENZ2' size='5' maxlength='5' />";
	output = output + "        </p></td>";
	output =
			output
					+ "        <td width='20%' align='right' bgcolor='#0099CC'>&nbsp;</td>";
	output = output + "      </tr>";
	output = output + "      <tr>";
	output = output + "        <td bgcolor='#006699'>CHANNEL 01</td>";
	output =
			output
					+ "        <td bgcolor='#006699'><input name='CH2' type='text' id='CH2' size='2' maxlength='2' /></td>";
	output = output + "        <td bgcolor='#006699'>CHANNEL 09</td>";
	output =
			output
					+ "        <td align='left' bgcolor='#006699'><input name='CH10' type='text' id='CH10' size='2' maxlength='2' /></td>";
	output = output + "      </tr>";
	output = output + "      <tr>";
	output = output + "        <td bgcolor='#006699'>CHANNEL 02</td>";
	output =
			output
					+ "        <td bgcolor='#006699'><input name='CH3' type='text' id='CH3' size='2' maxlength='2' /></td>";
	output = output + "        <td bgcolor='#006699'>CHANNEL 10</td>";
	output =
			output
					+ "        <td align='left' bgcolor='#006699'><input name='CH11' type='text' id='CH11' size='2' maxlength='2' /></td>";
	output = output + "      </tr>";
	output = output + "      <tr>";
	output = output + "        <td bgcolor='#006699'>CHANNEL 03</td>";
	output =
			output
					+ "        <td bgcolor='#006699'><input name='CH4' type='text' id='CH4' size='2' maxlength='2' /></td>";
	output = output + "        <td bgcolor='#006699'>CHANNEL 11</td>";
	output =
			output
					+ "        <td align='left' bgcolor='#006699'><input name='CH12' type='text' id='CH12' size='2' maxlength='2' /></td>";
	output = output + "      </tr>";
	output = output + "      <tr>";
	output = output + "        <td bgcolor='#006699'>CHANNEL 04</td>";
	output =
			output
					+ "        <td bgcolor='#006699'><input name='CH5' type='text' id='CH5' size='2' maxlength='2' /></td>";
	output = output + "        <td bgcolor='#006699'>CHANNEL 12</td>";
	output =
			output
					+ "        <td align='left' bgcolor='#006699'><input name='CH13' type='text' id='CH13' size='2' maxlength='2' /></td>";
	output = output + "      </tr>";
	output = output + "      <tr>";
	output = output + "        <td bgcolor='#006699'>CHANNEL 05</td>";
	output =
			output
					+ "        <td bgcolor='#006699'><input name='CH6' type='text' id='CH6' size='2' maxlength='2' /></td>";
	output = output + "        <td bgcolor='#006699'>CHANNEL 13</td>";
	output =
			output
					+ "        <td align='left' bgcolor='#006699'><input name='CH14' type='text' id='CH14' size='2' maxlength='2' /></td>";
	output = output + "      </tr>";
	output = output + "      <tr>";
	output = output + "        <td bgcolor='#006699'>CHANNEL 06</td>";
	output =
			output
					+ "        <td bgcolor='#006699'><input name='CH7' type='text' id='CH7' size='2' maxlength='2' /></td>";
	output = output + "        <td bgcolor='#006699'>CHANNEL 14</td>";
	output =
			output
					+ "        <td align='left' bgcolor='#006699'><input name='CH15' type='text' id='CH15' size='2' maxlength='2' /></td>";
	output = output + "      </tr>";
	output = output + "      <tr>";
	output = output + "        <td bgcolor='#006699'>CHANNEL 07</td>";
	output =
			output
					+ "        <td bgcolor='#006699'><input name='CH8' type='text' id='CH8' size='2' maxlength='2' /></td>";
	output = output + "        <td bgcolor='#006699'>CHANNEL 15</td>";
	output =
			output
					+ "        <td align='left' bgcolor='#006699'><input name='CH16' type='text' id='CH16' size='2' maxlength='2' /></td>";
	output = output + "      </tr>";
	output = output + "      <tr>";
	output = output + "        <td bgcolor='#006699'>CHANNEL 08</td>";
	output =
			output
					+ "        <td width='26%' bgcolor='#006699'><input name='CH9' type='text' id='CH9' size='2' maxlength='2' /></td>";
	output = output
			+ "        <td width='27%' bgcolor='#006699'>CHANNEL 16</td>";
	output =
			output
					+ "        <td align='left' bgcolor='#006699'><input name='CH17' type='text' id='CH17' size='2' maxlength='2' /></td>";
	output = output + "      </tr>";
	output = output + "    </table></td>";
	output = output + "  </tr>";
	output = output + "  <tr>";
	output = output + "    <td>&nbsp;</td>";
	output = output + "  </tr>";

	return output;
}

void PCA9685::DimChannel(int channel, int value,I2C i2c){
		cout << "DIMM from PCA9685" << endl;
}
