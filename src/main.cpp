#include <Arduino.h>

const short trig_pin = 3;
const short echo_pin = 2;
const short buzzer_pin = 7;

unsigned long last_serial_time = 0;
unsigned long last_buzzer_time = 0;
unsigned long last_sensor_time = 0;
long distance = 0;
int fr = 0;

bool isSound = false;

void setup()
{
  Serial.begin(9600);
  pinMode(trig_pin, OUTPUT);
  pinMode(echo_pin, INPUT);
  pinMode(buzzer_pin, OUTPUT);
}

long getDistance()
{
  digitalWrite(trig_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(trig_pin, HIGH);
  delayMicroseconds(10); // Строго 10 мкс для HC-SR04
  digitalWrite(trig_pin, LOW);

  // pulseIn чекає відгуку (таймаут 30000 мкс = ~5 метрів)
  long duration = pulseIn(echo_pin, HIGH, 30000);

  if (duration == 0)
    return -1; // Сигнал не повернувся (помилка)

  return duration * 0.034 / 2; // Перерахунок у см
}

void loop()
{
  if (millis() - last_sensor_time >= 100)
  {
    last_sensor_time = millis();
    distance = getDistance();
    fr = 10 + rand() % 2000;
  }

  if (millis() - last_serial_time >= 1000)
  {
    last_serial_time = millis();
    if (distance <= 0)
    {
      Serial.println("Error read sensor hc-sr04");
    }
    else
    {
      Serial.print("Distance: ");
      Serial.print(distance);
      Serial.println(" cm");
      Serial.print("Frequency: ");
      Serial.print(fr);
      Serial.println(" Hz");
    }
  }

  if (distance > 0 && distance < 50)
  {
    if (distance <= 5)
    {
      tone(buzzer_pin, fr);
    }
    else
    {
      int interval = map(distance, 5, 50, 40, 800);

      if (millis() - last_buzzer_time >= interval)
      {
        last_buzzer_time = millis();
        isSound = !isSound;

        if (isSound)
        {
          tone(buzzer_pin, fr);
        }
        else
        {
          noTone(buzzer_pin);
        }
      }
    }
  }
  else
  {
    noTone(buzzer_pin);
  }
}
