#include <Arduino.h>
/*
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

*/
//#define dbg

 u8  peer[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};  // MAC: MultiCast
 u8 l_MAC[] = {0xA4, 0xCF, 0x12, 0xD9, 0xC7, 0xAB};  // MAC: local MAC
 u8 r_MAC[] = {0x5C, 0xCF, 0x7F, 0x72, 0xE6, 0xE8};  // MAC: remote MAC

 #define TINY_BME280_I2C  // must be defined before: TinyBME280.h
 #define UPDATE_MILLIS       3000  // How often to cycle through "loop()"
 #define SDA 4                     // oled, bme
 #define SCL 5                     // oled, bme
 #define WiFiChannel 0             // ESPNOW requires all peers be on same channel
 #define U8X8_FONT u8g2_font_tiny5_tr // https://github.com/olikraus/u8g2/wiki/fntgrpu8g#tiny5  https://github.com/olikraus/u8g2/tree/master/tools/font
  

  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
  #include <ESPAsyncWebServer.h>
  #include <espnow.h>
  #include <LittleFS.h>
  #include <AsyncElegantOTA.h>  // over the air updates (Local ONLY)
  #include <U8g2lib.h>          // howTo:  https://github.com/olikraus/u8g2/wiki
  #include <Wire.h>
  #include <TinyBME280.h>
  #include <string>
  #include <index.h>


  #ifdef dbg
  u32 ms=millis();
  #define trace Serial.printf("%i %lums, ", __LINE__, millis()-ms);ms=millis();Serial.flush();
#else
  #define trace ;
#endif

/*
// very cool : bit manipulation macros: stored here, but not needed here
#define SET_BIT(x, pos) (x |= (1U << pos))
#define CLEAR_BIT(x, pos) (x &= (~(1U<< pos)))
#define CHECK_BIT(x, pos) (x & (1UL << pos) )
*/

  AsyncWebServer webServer(80);
  AsyncWebSocket webSock("/");
  U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8x8(U8G2_R0, U8X8_PIN_NONE, SCL, SDA); // u8g2lib.h: controller, pixels, mode, HW I2C
  tiny::BME280 bme; // Note: default I2C address 0x77

  /******  PROTOTYPES  *******/
    void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);
    void notFound(AsyncWebServerRequest *request);
    std::string valFromJson(const std::string &json, const std::string &element);
    wl_status_t wifiConnect(WiFiMode m);
    bool get_wifi_json();
    void set_wifi_json(const std::string &json);
    bool save_wifi_json();
    bool get_BME_json();
    void set_BME_json(const std::string &json);
    bool save_BME_json();
    String mac2string(const uint8_t *mac);
    void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus);
    void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len);
    float hPa2Hg(int n);
    int fixHumidity(int n);
//*******  END Prototypes  *******/

/******* Declare Constants & Globals *******/
  /*** Default WiFi IP info ***/
  IPAddress  // soft AP IP info
            ip_AP(10,10,10,77)
          , ip_STA(10,133,70,77)
          , ip_AP_STA(10,0,1,100)
          , ip_AP_GW(10,10,10,10)
          , ip_GW(10,133,70,1)
          , ip_subNet(255,255,255,0);
  /*** END Default WiFi IP info ***/

  const byte DNS_PORT  = 53;
  const char *AP_hostName = "Dueling_BME280";
  const char *ssid = "JRJAG2";
  const char *password = "D.J.Trump57";
  const char AP_SSID[] = "BME_Compare: 10.10.10.77"; // max length 32
  const char SAVE_BME_FAIL_MSG[] = "\"what\":\"index\",\"message\":\"BME Data Save FAILED\"}";

  typedef struct local_bme_data_t {
    float l_temp;
    float l_humid;
    float l_press;
    float r_temp;
    float r_humid;
    float r_press;
    u32   latency;
    float t_diff=0;
    float h_diff=0;
    float p_diff=0;
      
    std::string toStr(){    // make JSON string
      char c[200];
      int n = sprintf(c, "{\"src\":\"index\",\"l_temp\":%.1f,\"l_humid\":%.0f,\"l_press\":%.2f,\"r_temp\":%.1f,\"r_humid\":%.0f,\"r_press\":%.2f, \"t_diff\":%.2f,\"h_diff\":%.1f,\"p_diff\":%.2f,\"latency\":%u}"
      , l_temp, l_humid, l_press, r_temp, r_humid, r_press, t_diff, h_diff, p_diff, latency);
      return std::string(c, n);
    }
  } local_bme_data_t;
    local_bme_data_t local_bme;

  struct remote_bme_t{
    float r_temp;
    float r_humid;
    float r_press;
    u32   latency;
  };remote_bme_t remote_bme;

  typedef struct WifiCreds_t{
    String    ssid="80";
    String    pwd="GeorgeTheDogy";
    bool      isDHCP=false;
    IPAddress ip;
    IPAddress gw;
    IPAddress mask;
    bool      updated;

    std::string toStr(){
      char s[150];
      sprintf(s, "{\"src\":\"wifi\",\"ssid\":\"%s\",\"pwd\":\"%s\",\"isDHCP\":%s,\"ip\":\"%s\",\"gw\":\"%s\",\"mask\":\"%s\",\"updated\":%u}"
              , ssid.c_str(), pwd.c_str(), isDHCP?"true":"false", ip.toString().c_str(), gw.toString().c_str()
              , mask.toString().c_str(), updated);
      return(s);
    }
  } WifiCreds_t;
    WifiCreds_t wifi;

  u32 lastTime = 0;
  u32 latency = millis();
  u32 timerDelay = 3000;  // send readings timer
/******* END Constants & Globals *******/

void setup() {
#ifdef dbg
    Serial.begin(74880); // initial boot message is at 74880
    Serial.setDebugOutput(true);
    delay(500);
    Serial.printf("\n%i\t\t***** DEBUG Started *****\n", __LINE__);Serial.flush();
#endif

    LittleFSConfig cfg;
    cfg.setAutoFormat(false);
    LittleFS.setConfig(cfg);
#ifdef dbg
    if(!LittleFS.begin())printf("%i\tLittleFS.begin() FAILED\n",__LINE__);
#else
    LittleFS.begin();
#endif
    trace
#ifdef dbg
    if(!u8x8.begin())printf("%i\tu8x8.begin() FAILED\n",__LINE__);
#else
    u8x8.begin();
#endif
    u8x8.setPowerSave(0);
    u8x8.setFont(U8X8_FONT);
trace
#ifdef dbg
    if(!get_BME_json())printf("%i\tget_BME() FAILED\n",__LINE__);
    if(wifi_set_phy_mode(PHY_MODE_11N)==false)printf("%i\tFAILED: PHY_MODE_11N\n",__LINE__);
    yield();
    // Attempt Connect to the BME280
    if(bme.beginI2C(0x76) == false){
      printf("\n%i\tBME Begin(0x76) FAILED trying 0x77\n",__LINE__);
      if(bme.beginI2C(0x77) == false){
        printf("\n%i\tBME Begin(0x77) & BEGIN(0x76) BOTH FAILED\n",__LINE__);
      }else printf("\n%i\tBME Begin(0x77) worked!\n",__LINE__);
    }else printf("\n%i\tBME Begin(0x76) Worked!\n",__LINE__);
#else
    get_BME_json();
    wifi_set_phy_mode(PHY_MODE_11N);
    if(bme.beginI2C(0x76) == false)bme.beginI2C(0x77);
    yield();
#endif
    Wire.begin(SDA,SCL);  // I2C bmp1, bmp2, OLED

    get_wifi_json(); // get json from littlefs & load the struct(wifi)
    if(wifiConnect(WIFI_STA)!=3)wifiConnect(WIFI_AP); // start WiFi using struct(wifi)

#ifdef dbg
  printf("\n%i\t%s\t%s\tconneted: %s\n",__LINE__, WiFi.macAddress().c_str(),wifi.toStr().c_str(), WiFi.isConnected()?"yes":"no");
#endif
trace
#ifdef dbg
  if (esp_now_init() != 0)Serial.println("Error initializing ESP-NOW");
  esp_now_set_peer_channel(l_MAC, WiFiChannel);
#else
  esp_now_init();
  esp_now_set_peer_channel(r_MAC, WiFiChannel);
#endif
		esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
		esp_now_register_send_cb(OnDataSent);
		esp_now_register_recv_cb(OnDataRecv);
//    dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
//    dnsServer.start(DNS_PORT, "*", WiFi.getMode()==WIFI_STA ? WiFi.localIP() : WiFi.softAPIP());
    webServer.onNotFound(notFound);
trace
		webSock.onEvent(onWsEvent);
		webServer.addHandler(&webSock);
trace
		// Web Pages
		webServer.onNotFound(notFound);

    if(WiFi.softAPIP().isSet())webServer.on("/"     , HTTP_GET, [](AsyncWebServerRequest *request){request->send_P(200, "text/html", WIFI_HTML);});
    else{
      webServer.on("/"         , HTTP_GET, [](AsyncWebServerRequest *request){request->send_P(200, "text/html", INDEX_HTML);});
      webServer.on("/wifi"     , HTTP_GET, [](AsyncWebServerRequest *request){request->send_P(200, "text/html", WIFI_HTML);});
      // NOTE:  ip/update ==> OTA updates
    }
    ESP.wdtFeed();  // just incase...
trace
		AsyncElegantOTA.begin(&webServer);    // Start ElegantOTA
trace
		webServer.begin();
trace
    ESP.wdtFeed();
  esp_now_add_peer(r_MAC, ESP_NOW_ROLE_COMBO, 7, NULL, 0);
}

void loop() {
    char line1[32], line2[32], line3[32]; // OLED display data lines

    if ((millis() - lastTime) > timerDelay){

      local_bme.l_temp =std::round(bme.readFixedTempF()/10.0)/10.0;
      local_bme.l_humid=fixHumidity(bme.readFixedHumidity());
      local_bme.l_press=hPa2Hg(bme.readFixedPressure());

      sprintf(line1, "T %.1f P %.2f H %i",  local_bme.l_temp, local_bme.l_press, (int)local_bme.l_humid);
      sprintf(line2, "T %.1f P %.2f H %.0f",  local_bme.r_temp+local_bme.t_diff, local_bme.r_press+local_bme.p_diff, (float)local_bme.r_humid+local_bme.h_diff);
      sprintf(line3, "Remote Latency ms %u",local_bme.latency);

      latency=millis(); // signal remote to send it sensor readings==>> void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len);
      esp_now_send((uint8_t *)&r_MAC, (uint8_t *)&latency, sizeof(latency)); // send millis() to remote BME device; triggering remote to send its sensor readings

#ifdef dbg
    printf("\n%i\tRaw T: %i\tRAW H: %u\tRAW P: %u\n",__LINE__, bme.readFixedTempF(),bme.readFixedHumidity(),bme.readFixedPressure());Serial.flush();
    printf("\n%i\ttL: %.1f\thL: %.0f\tpL: %.2f\t\ttR: %.1f\thR: %.0f\tpR: %.2f\t latency: %u\n"
      ,__LINE__, local_bme.l_temp,local_bme.l_humid,local_bme.l_press,local_bme.r_temp,local_bme.r_humid,local_bme.r_press,local_bme.latency);Serial.flush();
//    printf("%i\tIP:%s\tIP:%s\tSSID:%s\tconnected:%s\n"
//      ,__LINE__, wifi.ip.toString().c_str(), WiFi.localIP().toString().c_str(), WiFi.SSID().c_str(), WiFi.isConnected()?"Yes":"NO");Serial.flush();
//    printf("\n%i\t%s\t\tisConnected: %s\tIP: %s\n", __LINE__, line1, WiFi.isConnected()?"yes":"no", WiFi.localIP().toString().c_str());Serial.flush();
    printf("%i\t%s\n", __LINE__, line1);Serial.flush();
    printf("%i\t%s\n", __LINE__, line2);Serial.flush();
    printf("%i\t%s\n", __LINE__, line3);Serial.flush();
#endif
      u8x8.clearBuffer();
      u8x8.setFlipMode(0);
      u8x8.drawStr(3,10,line1);
      u8x8.drawStr(3,19,line2);
      u8x8.drawStr(3,28,line3);
      u8x8.drawRFrame(0,0,128,32,1);
      u8x8.sendBuffer();

      webSock.textAll(local_bme.toStr().c_str()); delay(1);
      webSock.cleanupClients();
      lastTime = millis();
    } // loop delay
}  //  The LOOP

void notFound(AsyncWebServerRequest *request){request->send_P(200, "text/html", INDEX_HTML);}
  //**********************************************************************
void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
    if(type == WS_EVT_CONNECT){client->ping();webSock.cleanupClients();}
    if(type == WS_EVT_DISCONNECT){client->close();webSock.cleanupClients();}
    if(type != WS_EVT_DATA)return;
//**************
    AwsFrameInfo * info = (AwsFrameInfo*)arg;
    if(info->final && !info->index && info->len == len)if(info->opcode != WS_TEXT)return;
    data[len]=0; // null terminate the char[];  Just to make sure!
    std::string s((char*)data, len);
    std::string src = valFromJson(s, "src");
    std::string what = valFromJson(s, "what");
#ifdef dbg
    printf("\n%i src=%s\twhat: %s\nws_in: %s\n", __LINE__, src.c_str(),what.c_str(),s.c_str());
#endif
      if(src.compare("index")==0){
        if(what.compare("refresh")==0){client->text(local_bme.toStr().c_str());}
        if(what.compare("tare")==0){ // FYI: Values displayed on the web page are from the struct "local"
trace
          local_bme.t_diff = local_bme.l_temp  - local_bme.r_temp;
          local_bme.h_diff = local_bme.l_humid - local_bme.r_humid;
          local_bme.p_diff = local_bme.l_press - local_bme.r_press;

          if(!save_BME_json()){
trace
            client->text(SAVE_BME_FAIL_MSG);
          }
          else{
trace
            client->text(local_bme.toStr().c_str());
          }
        }
      }
      if(src.compare("wifi")==0){
        if(what.compare("submit")==0){
          printf("%i\twhat:submit: %s\n",__LINE__, what.c_str());
trace
          set_wifi_json(s); // <-- updated, but no reboot yet
trace
          wifi.updated = !save_wifi_json();
trace
          client->text(wifi.toStr().c_str()); // include "updated" value
        }
trace
        if(what.compare("refresh")==0){client->text(wifi.toStr().c_str());}
        if(what.compare("reboot")==0)ESP.restart();
      }
}
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len){// local gets whole struct.  remnote get r_unix_time
      memcpy(&local_bme.r_temp, incomingData, len);
      local_bme.latency = millis()-latency;
}
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus){
#ifdef dbg
  printf("\n%i\t%s\tsent to: %s\n",__LINE__,__FUNCTION__, mac2string(mac_addr).c_str());
#endif
}
String mac2string(const uint8_t *mac){
      char macStr[18];
      snprintf(macStr, 18, "%02X:%02X:%02X:%02X:%02X:%02X",
          mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
      return(macStr);
}
wl_status_t wifiConnect(WiFiMode_t m){
  WiFi.disconnect();
  WiFi.mode(m);  // WIFI_AP_STA,WIFI_AP; WIFI_STA;
    switch(m){
        case WIFI_STA:
            WiFi.enableAP(false);
            WiFi.enableSTA(true);
            delay(25);
            WiFi.hostname("Remote & Local BME280");
            if(wifi.isDHCP==false)WiFi.config(wifi.ip, wifi.gw, wifi.mask);
            WiFi.begin(wifi.ssid.c_str(), wifi.pwd.c_str(), WiFiChannel);
trace
            break;
        case WIFI_AP:
            WiFi.enableSTA(false);
            WiFi.enableAP(true);
            delay(25);
            WiFi.config(ip_AP, ip_AP_GW, ip_subNet);
            WiFi.softAPConfig(ip_AP, ip_AP_GW, ip_subNet);
            WiFi.softAP(AP_SSID, "", WiFiChannel, false, 4,1000);
            WiFi.channel(WiFiChannel);
            WiFi.begin();
          break;
        case WIFI_AP_STA:
            WiFi.enableSTA(true);
            WiFi.enableAP(true);
            delay(25);
            WiFi.hostname("Remote & Local BME280");
            if(!wifi.isDHCP)
              WiFi.config(wifi.ip, wifi.gw, wifi.mask);
            WiFi.softAPConfig(ip_AP, ip_AP_GW, ip_subNet);
            WiFi.softAP(AP_SSID, "", WiFiChannel, false, 4);
            WiFi.begin(wifi.ssid.c_str(), wifi.pwd.c_str(), WiFiChannel);
          break;
        case WIFI_OFF: return(WiFi.status());
    }
    WiFi.setOutputPower(20.5);  // range: 0dbm - 20.5dbm
    u32 startup = millis();
  #ifdef dbg
    Serial.println("\n");Serial.flush();
  #endif
    while(WiFi.status() != WL_CONNECTED){
      delay(750);
      ESP.wdtFeed();
  #ifdef dbg
      Serial.print(".");
  #endif
        if(millis() - startup >= 15000) break;
      }
  #ifdef dbg
        Serial.printf("\n%i\n",__LINE__);
  #endif
trace
  #ifdef dbg
     Serial.printf("\n%i ip: %s, gw: %s, mask: %s\n", __LINE__, WiFi.localIP().toString().c_str(), WiFi.gatewayIP().toString().c_str(), WiFi.subnetMask().toString().c_str());Serial.flush();
     Serial.printf("%i AP_ssid: \"%s\", AP_ip: %s\n", __LINE__, WiFi.softAPSSID().c_str(), WiFi.softAPIP().toString().c_str());Serial.flush();
     Serial.printf("%i MAC: %s, RSSI: %i\n", __LINE__, WiFi.macAddress().c_str(), WiFi.RSSI());Serial.flush();
     Serial.printf("%i WiFi.Mode: %hhu, WiFi.Status(): %hhu, connect took seconds: %lu\n\n", __LINE__, WiFi.getMode(), WiFi.status(), (millis() - startup)/1000);Serial.flush();

     std::string wlStatus;
    switch(WiFi.status()){       
      case  WL_NO_SHIELD:       wlStatus = "WL_NO_SHIELD";      break;   // for compatibility with WiFi Shield library
      case  WL_IDLE_STATUS:     wlStatus = "WL_IDLE_STATUS";    break;
      case  WL_NO_SSID_AVAIL:   wlStatus = "WL_NO_SSID_AVAIL";  break;
      case  WL_SCAN_COMPLETED:  wlStatus = "WL_SCAN_COMPLETED"; break;
      case  WL_CONNECTED:       wlStatus = "WL_CONNECTED";      break;
      case  WL_CONNECT_FAILED:  wlStatus = "WL_CONNECT_FAILED"; break;
      case  WL_CONNECTION_LOST: wlStatus = "WL_CONNECTION_LOST";break;
      case  WL_WRONG_PASSWORD:  wlStatus = "WL_WRONG_PASSWORD"; break;
      case  WL_DISCONNECTED:    wlStatus = "WL_DISCONNECTED";   break;
    }
    Serial.printf("\n%i\t%s\n", __LINE__,wlStatus.c_str());
    delay(100);
trace
  #endif
trace
      return(WiFi.status());
}
bool get_wifi_json(){
    fs::File f = LittleFS.open("/wifi.json", "r");
    if(f){
            f.seek(0, SeekSet); // offset, mode
            std::string s = f.readString().c_str();
            f.close();
            set_wifi_json(s);
            return(true);
    }
    return(false);
}
void set_wifi_json(const std::string& json){
    wifi.ip.fromString((const char*)valFromJson(json, "ip").c_str());
    wifi.gw.fromString((const char*)valFromJson(json, "gw").c_str());
    wifi.mask.fromString((const char*)valFromJson(json, "mask").c_str());
    wifi.isDHCP =::atoi((const char*)valFromJson(json, "isDHCP").c_str());
    wifi.ssid = valFromJson(json, "ssid").c_str();
    wifi.pwd  = valFromJson(json, "pwd").c_str();
}
bool save_wifi_json(){
    File f = LittleFS.open(F("/wifi.json"), "w");
    if(f){
      f.print(wifi.toStr().c_str());
      f.close();
#ifdef dbg
    Serial.printf("%i\t%s\t Success: wifi... %s\n", __LINE__, __FUNCTION__, wifi.toStr().c_str());Serial.flush();
#endif
          return(false);
    }
    else{
  #ifdef dbg
    Serial.printf("%i\t%s\t FAILED: wifi... %s\n", __LINE__, __FUNCTION__, wifi.toStr().c_str());Serial.flush();
  #endif
          return(true);
    }
}
bool get_BME_json(){ // true: FAILED to retrieve BME local to BME remote differential data
          File f = LittleFS.open("/bme.json", "r");
          if(f){
                          std::string s = f.readString().c_str();
                          f.close();
                          set_BME_json(s);
trace
                          return(1);
          }
trace
          return(0); // true: FAILED to retrieve BME local to BME remote differential data
}
void set_BME_json(const std::string& json){
      local_bme.t_diff  =::atoi(valFromJson(json, "t_diff").c_str());
      local_bme.h_diff  =::atoi(valFromJson(json, "h_diff").c_str());
      local_bme.p_diff  =::atoi(valFromJson(json, "p_diff").c_str());
}
bool save_BME_json(){  //true: FAILED to save BME local to BME remote differential data
    File f = LittleFS.open(F("/bme.json"), "w");
    if(f){
            f.print(local_bme.toStr().c_str());
            f.close();
trace
            return(1); // SUCCESS
    }
trace
    return(0); // FAILED
}
std::string valFromJson(const std::string &json, const std::string &element){
    size_t start, end;
    start = json.find(element);
    start = json.find(":", start)+1;
    if(json.substr(start,1) =="\"")start++;
    end  = json.find_first_of(",]}\"", start);
    return(json.substr(start, end-start));
}
void get_bme_readings(){
  local_bme.l_temp=bme.readFixedTempF()/100;
  local_bme.l_humid=bme.readFixedHumidity()/100;
  local_bme.l_press=bme.readFixedPressure();  // div-100 & convert from hPa to inHG
}
float hPa2Hg(int n){return(std::round(bme.readFixedPressure()*.02953)/100);}
int fixHumidity(int n){return(std::round(bme.readFixedHumidity()/100.0)/10);}
