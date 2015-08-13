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
