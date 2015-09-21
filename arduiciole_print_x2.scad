/**
 * Copyright (c) 2015 Guilhelm Savin. All rights reserved.
 *
 * This file is part of the ARDUICIOLE project.
 *
 * ARDUICIOLE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ARDUICIOLE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ARDUICIOLE.  If not, see <http://www.gnu.org/licenses/>.
 */
include <arduiciole_parts.scad>;

translate([0, -socle_h / 2 - 5, 0]) {
  translate([-socle_w/2-5, 0, 0])
  part2();

  translate([socle_w/2+5, 0, 0])
  part1();
}

translate([0, socle_h / 2 + 5, 0]) {
  translate([-socle_w/2-5, 0, 0])
  part2();

  translate([socle_w/2+5, 0, 0])
  part1();
}
