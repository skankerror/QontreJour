/*
 * (c) 2023 Michaël Creusy -- creusy(.)michael(@)gmail(.)com
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
 */

#ifndef QONTREJOUR_H
#define QONTREJOUR_H

enum dmxDrivers
{
  dummy,
  artnet,
  e131,
  dmxusb,
  uart,
  Count_dmxDrivers = uart +1
};

#define SUBMASTER_SLIDERS_COUNT 24

#define UNIVERSE_OUTPUT_COUNT_DEFAULT 512

#define DMX_CHANNEL_OUTPUT_TABLE_MODEL_ROWS_COUNT_DEFAULT 32
#define DMX_CHANNEL_OUTPUT_TABLE_MODEL_COLUMNS_COUNT_DEFAULT 32


//static int STATIC_UNIVERSE_COUNT = 0; // TODO this is ugly
//static float STATIC_GRAND_MASTER_VALUE = 1.0f; // this will multiply each output


#endif // QONTREJOUR_H
