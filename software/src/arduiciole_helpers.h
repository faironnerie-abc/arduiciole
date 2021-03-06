#ifndef __ARDUICIOLE_HELPERS_H__
#define __ARDUICIOLE_HELPERS_H__

#define HELPER_ANALOG_WRITE(x, d)  { analogWrite(HLED, x);  analogWrite(LLED, x);  delay(d); }
#define HELPER_DIGITAL_WRITE(x, d) { digitalWrite(HLED, x); digitalWrite(LLED, x); delay(d); }

#define HELPER_BLINK(L, D, C) for (int blinki = 0; blinki < C; blinki++) \
  { digitalWrite(L, HIGH); delay(D); digitalWrite(L, LOW); delay(D); }

#ifdef DEBUG
# define DEBUG_PRINT(m) altSoftSerial.println(m);
# define DEBUG_PRINT_VAL(m,v) altSoftSerial.print(m); altSoftSerial.println(v);
# define DEBUG_PRINT_VAL_HEX(m,v) altSoftSerial.print(m); altSoftSerial.println(v, HEX);
# define DEBUG_DIGITAL_WRITE(l,v) digitalWrite(l,v);
# define DEBUG_DELAY(d) delay(d);
#else
# define DEBUG_PRINT(m)
# define DEBUG_PRINT_VAL(m,v)
# define DEBUG_PRINT_VAL_HEX(m,v)
# define DEBUG_DIGITAL_WRITE(l,v)
# define DEBUG_DELAY(d)
#endif

#endif /* __ARDUICIOLE_HELPERS_H__ */
