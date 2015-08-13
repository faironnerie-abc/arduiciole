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
#ifdef ENERGIA
  #include "Energia.h"
#else
  #include "Arduino.h"
#endif

#define HLED 3
#define LLED 5
#define CLED 13

#define A_WRITE(x, d) { analogWrite(HLED, x);  analogWrite(LLED, x);  delay(d); }
#define D_WRITE(x, d) { digitalWrite(HLED, x); digitalWrite(LLED, x); delay(d); }

void setup()
{
  pinMode(CLED, OUTPUT);

  pinMode(HLED, OUTPUT);
  pinMode(LLED, OUTPUT);
}

void loop()
{
  for (int i=0; i<=255; i++)
    A_WRITE(i, 1);

  D_WRITE(HIGH, 1000);

  for (int i=255; i>=0; i--)
    A_WRITE(i, 2);

  D_WRITE(LOW, 3000);
}
