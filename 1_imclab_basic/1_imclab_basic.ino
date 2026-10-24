/*
  Modul 1 - iMCLab Basic
  Baca suhu T1/T2, kontrol heater Q1/Q2 dan LED lewat Serial Monitor.
  Protokol serial ala TCLab, jadi script Python (Modul 2) bisa memakai sketch ini.

  Perintah (kirim dari Serial Monitor, 115200 baud, "Newline"):
    T1 | T2        -> baca suhu (C)
    Q1 50 | Q2 50   -> set heater 0-100 persen
    LED 100         -> set LED 0-100 persen
    VER             -> versi firmware
    X               -> matikan semua heater

  PENTING: cek pin di bawah dengan manual/skematik kit iMCLab kamu.
*/

const int pinT1   = 34;   // sensor suhu 1 (ADC)
const int pinT2   = 35;   // sensor suhu 2 (ADC)
const int pinQ1   = 32;   // heater 1 (PWM)
const int pinQ2   = 33;   // heater 2 (PWM)
const int pinLED1 = 26;   // LED indikator
const int pinLED2 = 27;

const float TEMP_MAX = 80.0;   // batas aman, heater mati otomatis di atas ini

const int PWM_FREQ = 5000;
const int PWM_RES  = 8;        // 0-255
const int CH_Q1 = 0, CH_Q2 = 1, CH_LED1 = 2;

float readTemp(int pin) {
  long sum = 0;
  for (int i = 0; i < 20; i++) sum += analogRead(pin);
  float raw  = sum / 20.0;
  float volt = raw / 4095.0 * 3.3;
  return volt * 100.0;         // asumsi sensor LM35: 10 mV per C. Sesuaikan jika sensor berbeda.
}

void setPct(int ch, int pct) {
  pct = constrain(pct, 0, 100);
  ledcWrite(ch, pct * 255 / 100);
}

void allOff() {
  setPct(CH_Q1, 0);
  setPct(CH_Q2, 0);
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);

  ledcSetup(CH_Q1, PWM_FREQ, PWM_RES);   ledcAttachPin(pinQ1, CH_Q1);
  ledcSetup(CH_Q2, PWM_FREQ, PWM_RES);   ledcAttachPin(pinQ2, CH_Q2);
  ledcSetup(CH_LED1, PWM_FREQ, PWM_RES); ledcAttachPin(pinLED1, CH_LED1);
  pinMode(pinLED2, OUTPUT);

  allOff();
  Serial.println("iMCLab siap. Ketik VER, T1, T2, Q1 50, LED 100, X");
}

void loop() {
  // pengaman suhu
  float t1 = readTemp(pinT1), t2 = readTemp(pinT2);
  if (t1 > TEMP_MAX || t2 > TEMP_MAX) {
    allOff();
  }

  if (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    line.trim();
    line.toUpperCase();

    if (line == "VER") {
      Serial.println("iMCLab Basic 1.0");
    } else if (line == "T1") {
      Serial.println(t1, 2);
    } else if (line == "T2") {
      Serial.println(t2, 2);
    } else if (line == "X") {
      allOff();
      Serial.println("Stop");
    } else if (line.startsWith("Q1 ")) {
      int v = line.substring(3).toInt();
      setPct(CH_Q1, v);
      Serial.println(constrain(v, 0, 100));
    } else if (line.startsWith("Q2 ")) {
      int v = line.substring(3).toInt();
      setPct(CH_Q2, v);
      Serial.println(constrain(v, 0, 100));
    } else if (line.startsWith("LED ")) {
      int v = line.substring(4).toInt();
      setPct(CH_LED1, v);
      Serial.println(constrain(v, 0, 100));
    } else if (line.length() > 0) {
      Serial.println("Perintah tidak dikenal");
    }
  }
}
