# ESP HTTP NeoMatrix Text

Control a Neopixel Matrix over HTTP.

This is basically an adapted version of the [Text Neopixel Matrix controllable via MQTT](https://github.com/EdJoPaTo/esp-mqtt-neomatrix-text) I am using for myself.
But having a seperated MQTT Server running might not be suitable for everyone.

# HTTP Routes

You can GET and POST each of these routes to view the current value or set a new one.

- `/hue`

  Hue from 0° to 360°

  ![Hue Scale](https://upload.wikimedia.org/wikipedia/commons/a/ad/HueScale.svg)

- `/sat`

  Saturation from 0 to 100

- `/bri`

  Brightness from 0 to 252

  This value is normalized so 1 is the lowest brightness possible for green (hue 120).
  Also as the idea of this matrix is to display text the high brightness values are not as interesting (personally I only use 0-100)

- `/on`

  Can be either `1` or `0`

  Turns off the matrix.
  Keep in mind that the matrix is still consuming power due to the resistors.
  See [power consumption](#power-consumption)

  Pin 5 (D1) is set to follow this value.
  This way you can for example make use of relais to completly power down the matrix.

  Or simply unplug the matrix when you do not use it and it will not consume any power that way.

- `/text`

  Can be a string of your wishes without line breaks.
  If the text ist longer than the matrix is wide the text is scrolled through.

# Install the code on the ESP

Use the Arduino IDE to compile and install the code onto your ESP.

I tested this with an ESP8266 / NodeMCU.

Please check out another guide on how to program your ESP with the Arduino IDE.

# Wiring

Power the LED Matrix via its own Power Connections (+-) with 5V.

Connect the Matrix Input Connection to the ESP 8266
- 5V (probably red) to Vin
- GND (probably white) to GND
- Data (probably green) to GPIO 13 / D7

This way the ESP is powered via the Matrix and can be programmed while connected to the matrix.

Warning: If you power the matrix indirectly through the ESP make sure to keep the brightness low.
High brightness results in higher power consuption which the ESP can not handle.

Yes, im sure.
I accidentially tried.
Can't recommend the smell.

# Power consumption

The simplest way to power the matrix with 5V is via a USB power supply like the ones from mobile devices/phones.
As this matrix is meant for text most pixels will not be used and also the brightness does not need to be high.
While the matrix might consume > 1A with all pixels lit this is not the case for text.
Using a 1A power supply is definitly enough.

Some measurements
- ~1.4W while off
- ~1.5W with the default brightness / text,
- ~7.9W with white max brightness text "12345" (=5V 1.6A)
- ~9W with max brightness all led rainbow (not from this program)

# Connect it to your WiFi

When the ESP has no connection to a WiFi it creates a hotspot.
The default name is `ESP-Matrix` with the default password `ich will text` (german for 'I want text').
When programming the ESP you can change these values in the first few lines in the program code.

When connecting to the Hotspot an interface will allow you to specify the name and password of your local WiFi.

Once you did you can close the configuration website and the ESP will connect your WiFi.
It is then available with the hostname `ESP-Matrix`.

For more information about this process see [the documentation of the WiFiManager Library](https://github.com/tzapu/WiFiManager/tree/development#how-it-works)

As a little indicator the onboard LED will show the current connection status.
After powering on the ESP the LED will also turn on.
It will turn off when the ESP has successfully connected to WiFi and is ready to receive HTTP Requests.

Sometimes the ESP does not connect to your WiFi (onboard LED stays on) and opens its configuration portal.
Simply reset the ESP (RST button) to retry.
