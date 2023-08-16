#include <WiFi.h>
#include <HTTPClient.h>
#include <DHTesp.h>
#include <LiquidCrystal_I2C.h>

#define I2C_ADDR    0x27
#define LCD_COLUMNS 20
#define LCD_LINES   4

const char* ssid = "Wokwi-GUEST";
const char* password = "";

const int DHT_PIN = 13;
const int pinoLed = 4;

DHTesp dhtSensor;

LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_LINES);

void setup() {
  Serial.begin(115200);
  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  pinMode(pinoLed, OUTPUT);
  lcd.init();
  lcd.backlight();
  Serial.println("Connected");

  lcd.setCursor(0, 2);
  lcd.print("Online");
}

void loop() {
  TempAndHumidity  data = dhtSensor.getTempAndHumidity();
  float temperatura = data.temperature;
  float umidade = data.humidity;
  String url = "https://api.thingspeak.com/update?api_key=9937W6VQQ7DU0130&field1=" + String(temperatura) + "&field2=" + String(umidade);

  HTTPClient http;

  http.begin(url);
  int httpCode = http.GET();
  http.end();

  Serial.println("Temp: " + String(temperatura, 1) + "°C");
  Serial.println("Humidity: " + String(umidade, 1) + "%");
  Serial.println("---");
  Serial.println("HTTP Code: " + String(httpCode));

  lcd.setCursor(0, 0);
  lcd.print("  Temp: " + String(temperatura, 1) + "\xDF" + "C  ");
  lcd.setCursor(0, 1);
  lcd.print(" Humidity: " + String(umidade, 1) + "% ");

  if (httpCode >= 200 && httpCode <= 299) {
    String payload = http.getString();
    Serial.println(payload);
  } else {
    Serial.println("Error: " + String(httpCode));
  }

  digitalWrite(pinoLed, (temperatura > 35.0 || umidade > 70.0) ? HIGH : LOW);

  delay(5000);
}