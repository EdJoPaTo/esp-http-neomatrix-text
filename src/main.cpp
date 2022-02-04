#include <ESP8266WebServer.h>
#include <EspSimpleRemoteUpdate.h>
#include <FastLED_NeoMatrix.h>
#include <FastLED.h>
#include <WiFiManager.h>

WiFiManager wifiManager;
#define HOSTNAME "ESP-Matrix"
#define HOTSPOT_PASSWORD "ich will text"

EspSimpleRemoteUpdate updater;

const int HTTP_SERVER_PORT = 80;
ESP8266WebServer http_server(HTTP_SERVER_PORT);

const uint16_t WIDTH = 32;
const uint16_t HEIGHT = 8;

const int PIN_MATRIX = 13; // D7
const int PIN_ON = 5;      // D1

CRGB leds[WIDTH * HEIGHT];

//   NEO_MATRIX_TOP, NEO_MATRIX_BOTTOM, NEO_MATRIX_LEFT, NEO_MATRIX_RIGHT:
//     Position of the FIRST LED in the matrix; pick two, e.g.
//     NEO_MATRIX_TOP + NEO_MATRIX_LEFT for the top-left corner.
//   NEO_MATRIX_ROWS, NEO_MATRIX_COLUMNS: LEDs are arranged in horizontal
//     rows or in vertical columns, respectively; pick one or the other.
//   NEO_MATRIX_PROGRESSIVE, NEO_MATRIX_ZIGZAG: all rows/columns proceed
//     in the same order, or alternate lines reverse direction; pick one.
FastLED_NeoMatrix matrix = FastLED_NeoMatrix(leds, WIDTH, HEIGHT,
  NEO_MATRIX_BOTTOM  + NEO_MATRIX_RIGHT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG);

String text = "hey!";
uint16_t hue = 120; // green
uint8_t sat = 100;
uint8_t bri = 10;
boolean on = true;
int x = 0;

uint8_t hue8 = hue / 360.0 * 256.0;
uint8_t sat8 = sat / 100.0 * 255.0;

int textPixelWidth() {
  return text.length() * 6;
}

bool isTextLongerThanMatrix() {
  return textPixelWidth() > matrix.width();
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN_ON, OUTPUT);
  Serial.begin(115200);

  FastLED.addLeds<NEOPIXEL, PIN_MATRIX>(leds, WIDTH * HEIGHT);
  matrix.begin();
  matrix.setBrightness(bri * on);
  matrix.setCursor(0, 0);
  auto color = CRGB(CHSV(hue8, sat8, 255));
  matrix.setTextColor(matrix.Color(color.red, color.green, color.blue));
  matrix.setTextWrap(false);
  matrix.print(text);
  matrix.show();

  updater.enableOTA(NULL);
  // updater.enableHTTPWebUpdater(); // Would also run on Port 80 → https://github.com/plapointe6/EspSimpleRemoteUpdate/issues/1

  wifiManager.setHostname(HOSTNAME);
  wifiManager.autoConnect(HOSTNAME, HOTSPOT_PASSWORD);

  http_server.on("/", HTTP_GET, []() {
    Serial.println("Respond to /");
    http_server.send(200, "text/html", "Welcome to the http accessible Neopixel Matrix!\nCheck out the repo: https://github.com/EdJoPaTo/esp-http-neomatrix-text/\nVersion: " GIT_VERSION);
  });

  http_server.on("/hue", HTTP_GET, []() {
    http_server.send(200, "text/plain", String(hue));
  });

  http_server.on("/sat", HTTP_GET, []() {
    http_server.send(200, "text/plain", String(sat));
  });

  http_server.on("/bri", HTTP_GET, []() {
    http_server.send(200, "text/plain", String(bri));
  });

  http_server.on("/on", HTTP_GET, []() {
    http_server.send(200, "text/plain", String(on));
  });

  http_server.on("/text", HTTP_GET, []() {
    http_server.send(200, "text/plain", text);
  });

  http_server.on("/hue", HTTP_POST, []() {
    String payload = http_server.arg("plain");
    int parsed = strtol(payload.c_str(), 0, 10);
    hue = parsed % 360;
    hue8 = hue / 360.0 * 256.0;
    auto color = CRGB(CHSV(hue8, sat8, 255));
    matrix.setTextColor(matrix.Color(color.red, color.green, color.blue));
    http_server.send(200, "text/plain", String(hue));
  });

  http_server.on("/sat", HTTP_POST, []() {
    String payload = http_server.arg("plain");
    int parsed = strtol(payload.c_str(), 0, 10);
    sat = max(0, min(100, parsed));
    sat8 = sat / 100.0 * 255.0;
    auto color = CRGB(CHSV(hue8, sat8, 255));
    matrix.setTextColor(matrix.Color(color.red, color.green, color.blue));
    http_server.send(200, "text/plain", String(sat));
  });

  http_server.on("/bri", HTTP_POST, []() {
    String payload = http_server.arg("plain");
    int parsed = strtol(payload.c_str(), 0, 10);
    bri = max(0, min(255, parsed));
    matrix.setBrightness(bri * on);
    http_server.send(200, "text/plain", String(bri));
  });

  http_server.on("/on", HTTP_POST, []() {
    String payload = http_server.arg("plain");
    on = payload != "0";
    matrix.setBrightness(bri * on);
    http_server.send(200, "text/plain", String(on));
  });

  http_server.on("/text", HTTP_POST, []() {
    text = http_server.arg("plain");
    x = isTextLongerThanMatrix() ? matrix.width() : 0;
    http_server.send(200, "text/plain", text);
  });

  http_server.begin();

  Serial.println("Everything is set up.");
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  http_server.handleClient();
  digitalWrite(PIN_ON, on ? HIGH : LOW);

  matrix.fillScreen(0);
  matrix.setCursor(x, 0);
  matrix.print(text);

  if (isTextLongerThanMatrix()) {
    x -= 1;
    if (x < -textPixelWidth()) {
      x = matrix.width();
    }
  } else {
    x = 0;
  }

  matrix.show();
  delay(50);
}
