#include <Adafruit_NeoMatrix.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

WiFiManager wifiManager;
#define HOSTNAME "ESP-Matrix"
#define HOTSPOT_PASSWORD "ich will text"

const int HTTP_SERVER_PORT = 80;
ESP8266WebServer http_server(HTTP_SERVER_PORT);

const int PIN_MATRIX = 13; // D7
const int PIN_ON = 5;      // D1

// MATRIX DECLARATION:
// Parameter 1 = width of NeoPixel matrix
// Parameter 2 = height of matrix
// Parameter 3 = pin number (most are valid)
// Parameter 4 = matrix layout flags, add together as needed:
//   NEO_MATRIX_TOP, NEO_MATRIX_BOTTOM, NEO_MATRIX_LEFT, NEO_MATRIX_RIGHT:
//     Position of the FIRST LED in the matrix; pick two, e.g.
//     NEO_MATRIX_TOP + NEO_MATRIX_LEFT for the top-left corner.
//   NEO_MATRIX_ROWS, NEO_MATRIX_COLUMNS: LEDs are arranged in horizontal
//     rows or in vertical columns, respectively; pick one or the other.
//   NEO_MATRIX_PROGRESSIVE, NEO_MATRIX_ZIGZAG: all rows/columns proceed
//     in the same order, or alternate lines reverse direction; pick one.
//   See example below for these values in action.
// Parameter 5 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(32, 8, PIN_MATRIX,
  NEO_MATRIX_BOTTOM  + NEO_MATRIX_RIGHT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);

String text = "hey!";
uint16_t hue = 120; // green
uint8_t sat = 100;
uint8_t bri = 10;
boolean on = true;
int x = 0;

int textPixelWidth() {
  return text.length() * 6;
}

bool isTextLongerThanMatrix() {
  return textPixelWidth() > matrix.width();
}

// Defined below
uint16_t ColorHSV(uint16_t hue, uint8_t sat, uint8_t val);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN_ON, OUTPUT);
  Serial.begin(115200);

  matrix.begin();
  matrix.setBrightness(bri * on);
  matrix.setCursor(0, 0);
  matrix.setTextColor(ColorHSV(hue * 182, sat * 2.55, 255));
  matrix.setTextWrap(false);
  matrix.print(text);
  matrix.show();

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
    matrix.setTextColor(ColorHSV(hue * 182, sat * 2.55, 255));
    http_server.send(200, "text/plain", String(hue));
  });

  http_server.on("/sat", HTTP_POST, []() {
    String payload = http_server.arg("plain");
    int parsed = strtol(payload.c_str(), 0, 10);
    sat = max(0, min(100, parsed));
    matrix.setTextColor(ColorHSV(hue * 182, sat * 2.55, 255));
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

uint16_t ColorHSV(uint16_t hue, uint8_t sat, uint8_t val) {
  uint8_t r, g, b, r2, g2, b2;

  // Remap 0-65535 to 0-1529. Pure red is CENTERED on the 64K rollover;
  // 0 is not the start of pure red, but the midpoint...a few values above
  // zero and a few below 65536 all yield pure red (similarly, 32768 is the
  // midpoint, not start, of pure cyan). The 8-bit RGB hexcone (256 values
  // each for red, green, blue) really only allows for 1530 distinct hues
  // (not 1536, more on that below), but the full unsigned 16-bit type was
  // chosen for hue so that one's code can easily handle a contiguous color
  // wheel by allowing hue to roll over in either direction.
  hue = (hue * 1530L + 32768) / 65536;

  // Convert hue to R,G,B (nested ifs faster than divide+mod+switch):
  if (hue < 510) {         // Red to Green-1
    b = 0;
    if (hue < 255) {       //   Red to Yellow-1
      r = 255;
      g = hue;             //     g = 0 to 254
    } else {               //   Yellow to Green-1
      r = 510 - hue;       //     r = 255 to 1
      g = 255;
    }
  } else if (hue < 1020) { // Green to Blue-1
    r = 0;
    if (hue < 765) {       //   Green to Cyan-1
      g = 255;
      b = hue - 510;       //     b = 0 to 254
    } else {               //   Cyan to Blue-1
      g = 1020 - hue;      //     g = 255 to 1
      b = 255;
    }
  } else if (hue < 1530) { // Blue to Red-1
    g = 0;
    if (hue < 1275) {      //   Blue to Magenta-1
      r = hue - 1020;      //     r = 0 to 254
      b = 255;
    } else {               //   Magenta to Red-1
      r = 255;
      b = 1530 - hue;      //     b = 255 to 1
    }
  } else {                 // Last 0.5 Red (quicker than % operator)
    r = 255;
    g = b = 0;
  }

  // Apply saturation and value to R,G,B
  uint32_t v1 = 1 + val;  // 1 to 256; allows >>8 instead of /255
  uint16_t s1 = 1 + sat;  // 1 to 256; same reason
  uint8_t s2 = 255 - sat; // 255 to 0

  r2 = ((((r * s1) >> 8) + s2) * v1) >> 8;
  g2 = ((((g * s1) >> 8) + s2) * v1) >> 8;
  b2 = ((((b * s1) >> 8) + s2) * v1) >> 8;
  return matrix.Color(r2, g2, b2);
}
