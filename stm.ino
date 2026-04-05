#include <Wire.h>
#include <U8g2lib.h>
#include <DHT.h>

#define PIR_PIN PB0
#define LED_PIN PB1
#define DHTPIN PA0
#define DHTTYPE DHT11

#define IN1 PB10
#define IN2 PB11

DHT dht(DHTPIN, DHTTYPE);

// Vertical OLED display
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R1);

// Thresholds
float maxTemp = 33.0;
float minHum = 30.0;

void setup() {
  pinMode(PIR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  digitalWrite(LED_PIN, LOW);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  Serial1.begin(9600);

  dht.begin();
  u8g2.begin();
}

void loop() {
  int motion = digitalRead(PIR_PIN);

  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_5x8_tr);

  if (motion == HIGH) {

    digitalWrite(LED_PIN, HIGH);

    u8g2.drawStr(0, 10, "Motion: YES");
    u8g2.drawStr(0, 20, "Light : ON");

    Serial1.println("Motion Detected");
    Serial1.println("Light ON");

    if (!isnan(temp) && !isnan(hum)) {

      char tempStr[20];
      char humStr[20];
      char tempValue[10];
      char humValue[10];

      dtostrf(temp, 4, 1, tempValue);
      dtostrf(hum, 4, 1, humValue);

      sprintf(tempStr, "Temp:%sC", tempValue);
      sprintf(humStr, "Hum :%s%%", humValue);

      u8g2.drawStr(0, 35, tempStr);
      u8g2.drawStr(0, 45, humStr);

      Serial1.print("Temperature: ");
      Serial1.println(temp);

      Serial1.print("Humidity: ");
      Serial1.println(hum);

      // Fan ON if temperature high OR humidity low
      if (temp > maxTemp || hum < minHum) {

        // Motor ON
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);

        u8g2.drawStr(0, 58, "Fan : ON");

        Serial1.println("Fan ON");
      } 
      else {

        // Motor OFF
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);

        u8g2.drawStr(0, 58, "Fan : OFF");

        Serial1.println("Fan OFF");
      }

    } 
    else {
      u8g2.drawStr(0, 35, "DHT11 Error");

      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);

      Serial1.println("DHT11 Error");
    }

  } 
  else {

    digitalWrite(LED_PIN, LOW);

    // Motor OFF when no motion
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);

    u8g2.drawStr(0, 10, "Motion: NO");
    u8g2.drawStr(0, 20, "Light : OFF");
    u8g2.drawStr(0, 35, "Fan   : OFF");

    Serial1.println("No Motion");
    Serial1.println("Light OFF");
    Serial1.println("Fan OFF");
  }

  u8g2.sendBuffer();

  delay(1000);
}
