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

  Turns of the matrix. Keep in mind that the matrix is still consuming power due to the resistors.

  Pin 5 (D1) is set to follow this value.
  This way you can for example make use of relais to completly power down the matrix.

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
