/*  
 *	Arduino-DWM1000 - Arduino library to use Decawave's DWM1000 module.
 *	Copyright (C) 2018  Michele Biondi <michelebiondi01@gmail.com>, Andrea Salvatori <andrea.salvatori92@gmail.com>
 *
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <Arduino.h>

class DWM1000Device {
public:
    DWM1000Device(uint8_t ss, uint8_t irq, uint8_t rst, uint16_t shortAddress);
    DWM1000Device(uint8_t rst, uint8_t irq, uint8_t ss, uint16_t shortAddress, uint16_t panId);
    DWM1000Device(uint8_t rst, uint8_t irq, uint8_t ss, uint16_t shortAddress, uint16_t panId, char eui[]);
    ~DWM1000Device();

    void select();
}