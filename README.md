# 🦇 Ultrasonic Parktronic System in C++ with Arduino Mega 2560

Проєкт **Ультразвукового парктроніка** на базі мікроконтролера **Arduino Mega 2560**, дальноміра **HC-SR04** та звукового п'єзовипромінювача.

Пристрій постійно вимірює відстань до перешкоди та попереджає водія переривчастим звуковим сигналом: чим ближче перешкода — тим частіше пищить бузер. На відстані менше 5 см звук стає безперервним.

---

## 📌 Особливості проєкту

* **Динамічна частота сигналів:** Автоматичний розрахунок затримки між піками за допомогою функції `map()`.
* **Зона критичної небезпеки:** Автоматичний перехід на суцільний звук при наближенні $< 5\text{ см}$.
* **Захист від зависання:** Застосування таймауту для `pulseIn()`, що запобігає блокуванню програми при втраті ехо-імпульсу.
* **Моніторинг реального часу:** Вивід відстані у `Serial Monitor` кожні 500 мс.

---

## 🛠 Компоненти та обладнання

| Компонент | Кількість | Примітка |
|---|---|---|
| **Мікроконтролер** | 1 | Arduino Mega 2560 / Uno |
| **Ультразвуковий датчик** | 1 | HC-SR04 |
| **П'єзовипромінювач (Buzzer)** | 1 | Пасивний / активний 5V |
| **Макетна плата та дроти** | 1 | Breadboard та DuPont-дроти |

---

## 🔌 Схема підключення (Wiring)

| Модуль / Компонент | Вивід компонента | Пін Arduino Mega 2560 |
|---|---|---|
| **HC-SR04** | VCC | **5V** |
| **HC-SR04** | GND | **GND** |
| **HC-SR04** | Trig | **Digital Pin 3** |
| **HC-SR04** | Echo | **Digital Pin 2** |
| **Buzzer** | `+` (сигнал) | **Digital Pin 7** |
| **Buzzer** | `-` | **GND** |

---

## 💻 Код проєкту (`src/main.cpp`)

```cpp
#include <Arduino.h>

const short TRIG_PIN = 3;
const short ECHO_PIN = 2;
const short BUZZER_PIN = 7;

unsigned long lastSerialTime = 0;
unsigned long lastBuzzerTime = 0;
bool isSoundOn = false;

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
}

long getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return -1;
  
  return duration * 0.034 / 2;
}

void loop() {
  long distance = getDistance();

  if (millis() - lastSerialTime >= 500) {
    lastSerialTime = millis();
    Serial.print("Дистанція: ");
    if (distance <= 0) {
      Serial.println("Помилка / Поза зоною!");
    } else {
      Serial.print(distance);
      Serial.println(" см");
    }
  }

  if (distance > 0 && distance < 50) {
    if (distance <= 5) {
      tone(BUZZER_PIN, 1000);
    } else {
      int beepInterval = map(distance, 5, 50, 60, 600);

      if (millis() - lastBuzzerTime >= beepInterval) {
        lastBuzzerTime = millis();
        isSoundOn = !isSoundOn;
        if (isSoundOn) {
          tone(BUZZER_PIN, 1000);
        } else {
          noTone(BUZZER_PIN);
        }
      }
    }
  } else {
    noTone(BUZZER_PIN);
  }
}
