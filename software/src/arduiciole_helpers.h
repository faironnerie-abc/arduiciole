#ifndef __ARDUICIOLE_HELPERS_H__
#define __ARDUICIOLE_HELPERS_H__

#define HELPER_ANALOG_WRITE(x, d)  { analogWrite(HLED, x);  analogWrite(LLED, x);  delay(d); }
#define HELPER_DIGITAL_WRITE(x, d) { digitalWrite(HLED, x); digitalWrite(LLED, x); delay(d); }

#define HELPER_BLINK(L, D, C) for (int blinki = 0; blinki < C; blinki++) \
  { digitalWrite(L, HIGH); delay(D); digitalWrite(L, LOW); delay(D); }

#endif /* __ARDUICIOLE_HELPERS_H__ */
