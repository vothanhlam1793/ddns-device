#include <Arduino.h>
#if defined(ESP8266)
#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
#include <ESP_Watchdog.h> // Thêm thư viện ESP_Watchdog
#elif defined(ESP32)
#include "WiFi.h"
#include "HTTPClient.h"
#include <esp_task_wdt.h> // Thêm thư viện esp_task_wdt
#endif

#include <EasyDDNS.h>

const char *ssid = "TP-LINK_CCEA";
const char *password = "12345678";

WiFiServer server(80);

const int ledPin = 2; // IO2 for the LED
bool ledState = false;
unsigned long previousMillis = 0;
const long interval = 500; // 1Hz frequency (500ms ON, 500ms OFF)

void setup()
{
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP()); // In địa chỉ IP
  server.begin();

  /*
    Danh sách các nhà cung cấp DDNS được hỗ trợ:
    - "duckdns"
    - "noip"
    - "dyndns"
    - "dynu"
    - "enom"
    - "all-inkl"
    - "selfhost.de"
    - "dyndns.it"
    - "strato"
    - "freemyip"
    - "afraid.org"
  */
  EasyDDNS.service("duckdns");

  /*
    Đối với nhà cung cấp DDNS nơi bạn nhận được mã thông báo:
      Sử dụng: EasyDDNS.client("domain", "token");

    Đối với nhà cung cấp DDNS nơi bạn nhận được tên người dùng và mật khẩu: (Để trống trường mật khẩu "" nếu không yêu cầu)
      Sử dụng: EasyDDNS.client("domain", "username", "password");
  */
  EasyDDNS.client("viethitech.duckdns.org", "e74ab271-4dd0-4a5a-8664-29e57cfc359f"); // Nhập tên miền DDNS và mã thông báo của bạn

  // Được thông báo khi địa chỉ IP của bạn thay đổi
  EasyDDNS.onUpdate([&](const char *oldIP, const char *newIP)
                    {
    Serial.print("EasyDDNS - IP Change Detected: ");
    Serial.println(newIP); });

// Bật watchdog
#if defined(ESP8266)
  ESPWatchdog.enable(0); // Thiết lập thời gian chờ tắt watchdog (0 = tắt watchdog)
#elif defined(ESP32)
  esp_task_wdt_init(10, true); // Thiết lập thời gian chờ tắt watchdog (10 giây)
#endif

  pinMode(ledPin, OUTPUT); // Set the LED pin as output
}

void loop()
{
  // Toggle the LED every 1 second
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    ledState = !ledState;
    digitalWrite(ledPin, ledState);
  }

  // Kiểm tra địa chỉ IP công khai mới mỗi 10 giây
  EasyDDNS.update(10000);

// Đặt lại watchdog
#if defined(ESP8266)
  ESPWatchdog.feed();
#elif defined(ESP32)
  esp_task_wdt_reset();
#endif
}