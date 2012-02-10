/* infinity-pedal, a linux driver for the Infinity IN-USB-2 foot pedal
 * Copyright (C) 2012 Scott Squires
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

#define INFINITY_VENDOR         "05f3"
#define INFINITY_PRODUCT_PEDAL  "00ff"

int pedal_find_devnode(char **devnode);

