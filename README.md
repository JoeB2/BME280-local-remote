# BME280-remote-local

NOTE:   .pio/libdeps/AsyncElegantOTA/AsyncElegantOTA.h contains an #error directive: comment this line in order to compile the project

  Summary:
      This esp8266 firmware is intenmded to provide support for two BME280 sensors.  One local and one remote.
      This is the source code for the LOCAL.
      The remote device uses esp_now to send its readings to the local device.  The local device sends its
      most recent millis() to the remote device and the remote device returns that same time along with its
      sensor readings.  local then subtracts the original millis() from the current millis() inferring tx/rx latency.

  Compile & Deploy:
      The firmware explicitly uses the MAC address of the remote and the local for esp_now commuication.
      Determine the mac addresses for the local and the remote deevice and upate l_MAC & r_MAC respectively.

  #define dbg will enable several printf() activities throughout the code in an effort to aid in determnining
  errant activity with the code.

  Activity:
    1) Update the struct local_bme with bme280 readings
    2) Remote 8266 device's bme280 readings used to update remote's "remote_bme" struct
    3) This local 8266, using espNOW: send current millis() to the remote 8266 device
    4) The remote 8266 device, upon revieing the espNOW message replies, using espNOW, with its remote_bme struct
    5) The local 8266 device periodically updates its oled, waits for http requests, and send websock json messages
    6) "Tare" button on INDEX_HTML page's "help" calcutales the diff between current local and remote BME readings
    7) The http request ip/wifi provides an html web page for updating wifi connection properties.(SSID, PWD, DHCP(Y/N), etc)
    8) WiFi credentials are loaded and kept in struct wifi.  However non voloital storage, littlefs, maintains wifi.json after each web update
    9) http root request returns a web page with all sensor values, local & remote.  Help will reveal an opportunity for
      several buttons: wifi, tare, 
    10) http root request, followed by "Help" and "Tare" will send a json message to the local MCU.
    11) Tare: The local MCU subtracts the remote's sensor values (temnp, humidity, pressure) from the current local values and
        stores the results in the struct local_bme.  Ultimatly storing a json string representation of local_bme in littlefs as bme.json
    12) All displayed remote sensor readings (html, oled) are the result of remote reading + "tidfii, h_diff, p_diff" initialized with zero
        until the first "Tare" activity

  Notes:
    SDA, SCL for the oled and BME280 are set with the #define SDA and SCL
    loop() cycle frequency is set with #define UPDATE_MILLIS
    u8x8 font is set with: #define U8X8_FONT  // fonts: 


