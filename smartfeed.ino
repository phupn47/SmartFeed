#define BLYNK_TEMPLATE_ID "TMPL63N9mbxsn"
#define BLYNK_TEMPLATE_NAME "Smart Feed"
#define BLYNK_AUTH_TOKEN "BftVqPAFvgSBUjWzQzVuFuonX6dSlftQ"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <ESP32Servo.h>
#include <TimeLib.h>
#include <WidgetRTC.h>
#include <HTTPClient.h>

char ssid[] = "Sorn_2.4G"; // SSID
char pass[] = "0811775208"; // WiFi Password

String SHEET_URL = "https://script.google.com/macros/s/AKfycbyUqvV3BGZA3Rg3XE3-mEDeyqTTq1NVj3jAyZU-x5YwPNHlWhhpJ5dveOSf1dV_gn5dIA/exec";
int feedCount = 1; // Start count at 1

const int btnPin = 12;
bool lastButtonState = HIGH;
bool btnState = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

Servo myServo;
const int servoPin = 14;
WidgetRTC rtc;
BlynkTimer timer;

// value for feed automation
bool isAutoFeed = false;
int feedHour = -1;
int feedMinute = -1;

// Collect last feed
int lastFeedDay = -1;
int lastFeedHour = -1;
int lastFeedMinute = -1;

BLYNK_WRITE(V0) { // Button for feed
  int value = param.asInt();
  if (value == 1) {
    feed("กดปุ่มให้อาหารเรียบร้อยแล้ว");
    logToGoogleSheet("กดปุ่มผ่าน Blynk");
    Blynk.virtualWrite(V0, 0); // reset button
  }
}

BLYNK_WRITE(V5) { // Time input for app
  time_t t = param[0].asLong();  // get timestamp from time input

  if (t > 0) {
    feedHour = hour(t);
    feedMinute = minute(t);
    Serial.printf("ตั้งเวลาให้อาหาร: %02d:%02d\n", feedHour, feedMinute);
  } else {
    Serial.println("Time input invalid");
  }
}

BLYNK_WRITE(V6) { // Switch for Automation mode
  isAutoFeed = param.asInt() == 1;
  Serial.println(isAutoFeed ? "เปิดระบบตั้งเวลาให้อาหาร" : "ปิดระบบตั้งเวลา");
}

void checkAutoFeed() { // check current time is equal to timeset
  int currH = hour();
  int currM = minute();
  int currD = day();

  Serial.printf("[check] ตอนนี้ %02d:%02d, ตั้งไว้ %02d:%02d, Auto: %d, ล่าสุด %02d:%02d/%d\n",
                currH, currM, feedHour, feedMinute, isAutoFeed, lastFeedHour, lastFeedMinute, lastFeedDay); // check time for feeding, automation mode
  if (!isAutoFeed || feedHour < 0 || feedMinute < 0) return;

  // if current time is equal to timeset
  if (currH == feedHour && currM == feedMinute) {
    if (currD != lastFeedDay || currH != lastFeedHour || currM != lastFeedMinute) {
      feed("ให้อาหารอัตโนมัติเรียบร้อยแล้ว");
      logToGoogleSheet("ตั้งเวลาผ่าน Blynk");
      lastFeedDay = currD;
      lastFeedHour = currH;
      lastFeedMinute = currM;
    }
  }
}

void feed(String eventMessage) { // feed button + alert +  show message

  myServo.write(180);
  delay(500);
  myServo.write(0);

  int h = hour();
  int min = minute();
  int d = day();
  int m = month();
  int y = year();

  lastFeedHour = h;
  lastFeedMinute = min;
  lastFeedDay = d;

  // set current time
  String timeStr = (h < 10 ? "0" : "") + String(h) + ":" + (min < 10 ? "0" : "") + String(min);
  String message;

  // show last feed message
  if (d == day() && m == month() && y == year()) {
    message = "ให้อาหารล่าสุดวันนี้ เวลา " + timeStr;
  } else {
    message = "ให้อาหารล่าสุดเมื่อ " + String(d) + "/" + String(m) + "/" + String(y) + " เวลา " + timeStr;
  }

  Serial.println(message);
  Blynk.virtualWrite(V1, message); // show message on app

  Blynk.logEvent("feed_notify", eventMessage); // notify
}

void logToGoogleSheet(String method) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(SHEET_URL);
    http.addHeader("Content-Type", "application/json");

    String dateStr = String(day()) + "/" + String(month()) + "/" + String(year());
    String timeStr = String(hour()) + ":" + (minute() < 10 ? "0" : "") + String(minute());

    String payload = "{";
    payload += "\"method\":\"" + method + "\",";
    payload += "\"date\":\"" + dateStr + "\",";
    payload += "\"time\":\"" + timeStr + "\"";
    payload += "}";

    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
      Serial.println("Google Sheet logged");
    } else {
      Serial.printf("Failed to log: %s\n", http.errorToString(httpResponseCode).c_str());
    }
    http.end();
  }
}

void setup() {
  Serial.begin(115200);
  myServo.attach(servoPin);
  myServo.write(0); // origin

  pinMode(btnPin, INPUT_PULLUP); // setting button pin

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  rtc.begin();

  setSyncInterval(10 * 60);

  Blynk.syncVirtual(V5); // time input
  Blynk.syncVirtual(V6); // auto switch

  timer.setInterval(5000L, checkAutoFeed);  // check time for automation feed every min
}

void loop() {
  Blynk.run();
  timer.run();

  int reading = digitalRead(btnPin);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading == LOW && btnState == HIGH) {
      Serial.println("Button Pressed!");
      feed("กดปุ่มให้อาหารเรียบร้อยแล้ว");
      logToGoogleSheet("กดปุ่มที่เครื่อง");
    }

    btnState = reading;
  }

  lastButtonState = reading;
}