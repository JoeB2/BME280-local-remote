extern const char INDEX_HTML[] PROGMEM =  R"=====(0
<!DOCTYPE html>
<html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1" />
    <link
        rel="stylesheet"
        href="https://use.fontawesome.com/releases/v5.7.2/css/all.css"
        integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr"
        crossorigin="anonymous"
    />
    <title>Local & Remote BME280</title>
    <style>
        @viewport{width:device-width;zoom:1.0;}
        @-ms-viewport {width: device-width ;}
        html{
            font-family: Arial;
            display: inline-block;
            margin: 0px auto;
            text-align: left;
            min-width: 544px;
            min-height:777px;
            background-color: dodgerblue;
        }
        h2 {
            font-size: 4.0vw;
            text-align: center;
        }
        h3 {
            font-size: 2.8vw;
            font-weight: bold;
            text-align: center;
            background-color: rgb(255,255,255);
            color: blue;
            vertical-align: top;
            margin-top:0px;
        }
        .help {
            border: 0;
            line-height: 3.5vw;

            padding: 0 1vw;
            text-align: center;
            font-weight:bold;
            font-size: 2.2vw;

            cursor: pointer;
            background-color: rgb(255, 255, 255);
            box-shadow: inset 1vw 1vw 1vw rgba(255, 255, 255, 0.6),
                inset -.3vw -.25vw .3vw rgba(0, 0, 0, 0.6);

            border:19vw;

            margin-left:.3vw;
            margin-top: 4vw;
            padding: 1vw;

            width:67.9vw;
            margin-inline: 1vw;
        }
        .vcc {
            color: #242121;
            font-weight: bold;
            font-family: Verdana;

            text-align: center;
            vertical-align: top;
            padding-left: 1vw;
            padding-top: -1vw;
            font-size: 2.5vw;
            grid-column:5;
            grid-row:1/2;
        }
        .blink {
            animation: blinker 1.9s linear infinite;
            color: #a81a1a;
            font-weight: bold;
            font-family: Verdana;

            padding-left: 1vw;
            font-size: 2.5vw;
            grid-column:5;
            grid-row:1/2;
        }
        @keyframes blinker {
            99% {
                opacity: 0;
            }
        }
        .sensors-grid{
            display:grid;
            grid-template-columns:25vw 13vw 21vw 13vw;
            grid-template-rows:  10vw 4vw 4vw 4vw 4vw;

            box-sizing: border-box;
            width: min(100%, 75.5vw);
            min-width: 42w;
            height: 32vw;

            margin-left: 5vw;
            margin-right: 5vw;

            background-color: #ced3c7;
            justify-items: left;

            padding: 1.3vw 2.3vw;

            font-size:2.5vw;
            font-weight: bold;
        }
        .status-grid{
            display:grid;
            grid-template-columns:7vw 18vw 15vw 17vw 8vw;
            grid-template-rows: 4vw 9.5vw;

            box-sizing: border-box;
            width: min(100%, 75.5vw);
            min-width: 39vw;
            height: 16.5vw;

            margin-left: 5vw;
            margin-right: 5vw;

            background-color: #ced3c7;
            justify-items: center;

            padding: 1vw 2.8vw;

            font-size:2.5vw;
            font-weight: bold;
            text-align:center;
        }
        .time {
            grid-column: 4 / 5;
            color: blue;
            font-size:2.0vw;
        }
    </style>
  </head>
  <h3 style="margin: 1vw 5vw;width: 72.5vw;">Local & Remote BME280</h3>
  <main class="sensors-grid" style="font-size:2.4vw;">
    <div>
        <span id="bt_help" class="help" role="submit"
            onclick="f_help()" value="Help" title="Hide/Show WebSock Messages">Help
        </span>
    </div>
    <div id="clock" class="time">clock</div>
    <div>
        <i class="fas fa-thermometer-three-quarters" style="color: #ff0022; margin-left: 1.6vw;font-size: 2.3vw"></i>
        <span title="Local Temp.  BME280">Local Temp</span>
    </div>
    <div>
        <span id="l_temp" name="sensors">%T1%</span>
        <sup class="units">&deg;F</sup>
    </div>
    <div>
        <i class="fas fa-thermometer-half" style="color: #ff0022; margin-left: 1.4vw;font-size: 2.3vw"></i>
        <span title="Remote Temperature. (BME280)" >Rmt Temp</span>
    </div>
    <div>
        <span id="r_temp" name="sensors">%T2%</span>
        <sup class="units">&deg;F</sup>
    </div>
    <div>
        <i class="fas fa-tint" style="color:#00add6; margin-left: 1.2vw;font-size: 2.3vw"></i>
        <span title="Local Humidity">Local Humid</span>
    </div>
    <div>
        <span id="l_humid" name="sensors">%H1%</span>
        <sup class="units">%</sup>
    </div>
    <div>
        <i class="fas fa-tint" style="color:#00add6; margin-left: 1.2vw;font-size: 2.3vw"></i>
        <span title="Remote Humidity.">Rmt Humid</span>
    </div>
    <div>
        <span id="r_humid" name="sensors">%H2%</span>
        <sup class="units">%</sup>&nbsp&nbsp
    </div>
    <div>
        <i class="fas fa-tachometer-alt" style="color: #06b106; margin-left: .50vw; font-size:2.3vw;"></i>
        <span title="Air pressure :  NOTE: Standard is 14.7 PSI at Sea Level. (29.92 in Hg)">Local Press</span>
    </div>
    <div>
        <span id="l_press" name="sensors">%P1%</span>
        <sup class="units">Hg</sup>
    </div>
    <div>
        <i class="fas fa-tachometer-alt" style="color: #06b106; margin-left: .50vw; font-size:2.3vw;"></i>
        <span title="Air pressure :  NOTE: Standard is 14.7 PSI at Sea Level. (29.92 in Hg)">RMT Press</span>
    </div>
    <div>
        <span id="r_press" name="sensors">%P2%</span>
        <sup class="units">Hg</sup>
    </div>
    <div><i class="fas fa-thermometer-empty" style="color:#00add6; margin-left: 1.5vw;font-size: 2.3vw"></i>
        <span title="Temperature water comes out of the air.">Local DP</span>
    </div>
    <div>
        <span id="l_dp" name="sensors">%DP1%</span>
        <sup class="units">&degF</sup>&nbsp&nbsp
    </div>
    <div><i class="fas fa-thermometer-empty" style="color:#00add6; margin-left: 1.5vw;font-size: 2.3vw"></i>
        <span title="Temperature water comes out of the air.">RMT DP</span>
    </div>
    <div>
        <span id="r_dp" name="sensors">%DP2%</span>
        <sup class="units">&degF</sup>&nbsp&nbsp
    </div>
    <div>
        <svg enable-background='new 0 0 58.422 40.639'height=2vw id=Layer_1 version=1.1 viewBox='0 0 58.422 40.639'width=4vw x=0px xml:space=preserve xmlns=http://www.w3.org/2000/svg xmlns:xlink=http://www.w3.org/1999/xlink y=0px>
            <g>
                <path d='M58.203,37.754l0.007-0.004L42.09,9.935l-0.001,0.001c-0.356-0.543-0.969-0.902-1.667-0.902
                    c-0.655,0-1.231,0.32-1.595,0.808l-0.011-0.007l-0.039,0.067c-0.021,0.03-0.035,0.063-0.054,0.094L22.78,37.692l0.008,0.004
                    c-0.149,0.28-0.242,0.594-0.242,0.934c0,1.102,0.894,1.995,1.994,1.995v0.015h31.888c1.101,0,1.994-0.893,1.994-1.994
                    C58.422,38.323,58.339,38.024,58.203,37.754z'fill=#955BA5
                />
                <path d='M19.704,38.674l-0.013-0.004l13.544-23.522L25.13,1.156l-0.002,0.001C24.671,0.459,23.885,0,22.985,0
                    c-0.84,0-1.582,0.41-2.051,1.038l-0.016-0.01L20.87,1.114c-0.025,0.039-0.046,0.082-0.068,0.124L0.299,36.851l0.013,0.004
                    C0.117,37.215,0,37.62,0,38.059c0,1.412,1.147,2.565,2.565,2.565v0.015h16.989c-0.091-0.256-0.149-0.526-0.149-0.813
                    C19.405,39.407,19.518,39.019,19.704,38.674z'fill=#955BA5
                />
            </g>
        </svg>
        <span title="Functional Altitude.  Altitude you feel & engines & planes feel.">Local DA</span>
    </div>
    <div>
        <span id="l_da" name="sensors">%A1%</span>
        <sup class="units">ft</sup>&nbsp&nbsp
    </div>
    <div>
        <svg enable-background='new 0 0 58.422 40.639'height=2vw id=Layer_1 version=1.1 viewBox='0 0 58.422 40.639'width=4vw x=0px xml:space=preserve xmlns=http://www.w3.org/2000/svg xmlns:xlink=http://www.w3.org/1999/xlink y=0px>
            <g>
                <path d='M58.203,37.754l0.007-0.004L42.09,9.935l-0.001,0.001c-0.356-0.543-0.969-0.902-1.667-0.902
                    c-0.655,0-1.231,0.32-1.595,0.808l-0.011-0.007l-0.039,0.067c-0.021,0.03-0.035,0.063-0.054,0.094L22.78,37.692l0.008,0.004
                    c-0.149,0.28-0.242,0.594-0.242,0.934c0,1.102,0.894,1.995,1.994,1.995v0.015h31.888c1.101,0,1.994-0.893,1.994-1.994
                    C58.422,38.323,58.339,38.024,58.203,37.754z'fill=#955BA5
                />
                <path d='M19.704,38.674l-0.013-0.004l13.544-23.522L25.13,1.156l-0.002,0.001C24.671,0.459,23.885,0,22.985,0
                    c-0.84,0-1.582,0.41-2.051,1.038l-0.016-0.01L20.87,1.114c-0.025,0.039-0.046,0.082-0.068,0.124L0.299,36.851l0.013,0.004
                    C0.117,37.215,0,37.62,0,38.059c0,1.412,1.147,2.565,2.565,2.565v0.015h16.989c-0.091-0.256-0.149-0.526-0.149-0.813
                    C19.405,39.407,19.518,39.019,19.704,38.674z'fill=#955BA5
                />
            </g>
        </svg>
        <span title="Functional Altitude.  Altitude you feel & engines & planes feel.">RMT DA</span>
    </div>
    <div>
        <span id="r_da" name="sensors">%A2%</span>
        <sup class="units">ft</sup>&nbsp&nbsp
    </div>
  </main>
  <main>
    <!-- comment: NOTES -->
    <!-- <div id="help" hidden="true">  -->
    <div id="help" hidden class="settings-grid">
        <span class="help"
            value="Sync BME1 & BME2"
            type="button"
            onclick="f_tare()"
            style="margin-left:14vw;background: rgb(155, 219, 149)"
            title="Send diff info to controller"
            >Sync BME1 & BME2
        </span>
        <span class="help"
            value="WiFi Settings"
            type="button"
            onclick="f_redirect('wifi')"
            style="background: orange;"
            title="Web Page for setting WiFi SSID/PWD & IP"
            >WiFi Settings
        </span>
        <span class="help"
            value="Firmware"
            type="button"
            onclick="f_redirect('update')"
            style="background:rgb(249, 253, 0);color: red;"
            title="Web Page for updating firmware"
            >Firmware
        </span>
        <details><hr>JSON Inbound<hr>
            <aside id="json1" style="background-color:white;font-weight: normal; font-size: 1.5vw; word-wrap:  break-word;">{"src":"mcuMain","what":"in bound"}</aside>
            <br>JSON Outbound<hr>
            <aside id="json2" style="background-color:white;font-weight: normal; font-size: 1.5vw; word-wrap:  break-word;">{"src":"main","what":"out bound"}</aside>
            <hr>
        </details>
    </div>
   </main>
  <script language="javascript" type="text/javascript">
        window.onload = function() {setInterval( f_clock, 1000);lastWebSock = Date.now();}
        let webSock = new WebSocket("ws://" + window.location.hostname + ":80");
        webSock.onopen = function(){ 
            let json = {src:"index",what:"refresh"};
            webSock.send(JSON.stringify(json));
        };
        webSock.onmessage = function(evt) {f_webSockOnMessage(evt);};
        webSock.onerror = function(evt) {console.warn(evt);};
        webSock.onclose = function(){};
        let sleep_s = Number(30); // no WebSock inbound timeout; send refresh signal
        let lastDataRefresh = Number(1);
        let lastPageRefresh = Number(1);
        
        const ALTITUDE = Number(890);
        let j = {
              src: "index"
            , what: "refresh"
            , l_temp:  67.60
            , l_humid: 15.1
            , l_press: 29.92
            , r_temp:  31.72
            , r_humid: 17.6
            , r_press: 32.15
            , latency: 117
            , t_diff:  0
            , h_diff:  0
            , p_diff:  0
        };
//        let js = JSON.stringify(j);
function f_webSockOnMessage(evt){
            if (typeof evt.data === "string"){
                let s = evt.data;
                j = JSON.parse(s);
                if(j.src != "index" )return;
                lastWebSock = Date.now();
                document.getElementById("json1").innerHTML = s;
                let elements=document.getElementsByName("sensors");
                for (let i=0;i<elements.length;i++){
            switch (elements[i].id){
                case "src":    continue;
                case "what":   continue;
                case "t_diff": continue;
                case "h_diff": continue;
                case "p_diff": continue;
                case "l_temp":
                    document.getElementById("l_temp").innerHTML
                     = j.l_temp?j.l_temp.toFixed(1):"n/a";
                  break;
                case "r_temp":
                    document.getElementById("r_temp").innerHTML
                     = j.r_temp?(j.r_temp+j.t_diff).toFixed(1):"n/a";
                  break;
                  case "l_humid":
                    document.getElementById("l_humid").innerHTML
                     = j.l_humid?j.l_humid.toFixed(1):"n/a";
                  break;
                  case "r_humid":
                    document.getElementById("r_humid").innerHTML
                     = j.r_humid?(j.r_humid+j.h_diff).toFixed(1):"n/a";
                  break;
                  case "l_press":
                    document.getElementById("l_press").innerHTML
                     = j.l_press?j.l_press.toFixed(1):"n/a";
                  break;
                  case "r_press":
                    document.getElementById("r_press").innerHTML
                     = j.r_press?(j.r_press+j.p_diff).toFixed(1):"n/a";
                  break;
            }
        }
        document.getElementById("l_dp").innerHTML = j.l_press?f_dewPoint(j.l_temp,j.l_humid).toFixed(1):"n/a";
        document.getElementById("r_dp").innerHTML = j.r_press?f_dewPoint(j.r_temp, j.r_humid).toFixed(1):"n/a";
        document.getElementById("l_da").innerHTML = j.l_press?f_densityAlt(j.l_temp,j.l_press).toFixed(1):"n/a";
        document.getElementById("r_da").innerHTML = j.r_press?f_densityAlt(j.r_temp, j.r_press).toFixed(1):"n/a";
        }
    }
    function f2c(f){return((f-32)/1.80)}
    function c2f(c){return((c*1.80)+32)}
    function f_PressureAltitude(p,A){return((29.92-p)*1000+A)}
    function f_isaTempC(A){return(15-1.98*A/1000)}
    function f_densityAlt(t,p){  //DA = PA + (120 x (OAT – ISA)) : Density Altitude in Feet = Pressure Altitude in Feet + (120 x (OAT°C – ISA Temperature °C))
        return(f_PressureAltitude(p,ALTITUDE)+(120*(f2c(t)-f_isaTempC(ALTITUDE))));
    }
    function f_dewPoint(t,h){
        let a=17.625;
        let b=243.04;
        let tC=f2c(t);
        let alph=Math.log2(h/100)+a*tC/(b+tC);
        return(c2f((b*alph/(a-alph))));
    }
    function f_clock(){
        const clock = document.getElementById("clock");
        d = new Date();
        clock.innerHTML = d.toLocaleDateString() + "<br>"
        + d.toLocaleTimeString(
            'en-US', {
                timeZone:'America/Los_Angeles'
            , hour12:false
            , hour:'numeric'
            , minute:'numeric'
            , second:'numeric'
            }
        );
    };
    function f_redirect(where) {window.location.href = "/" + where;}
    function f_help(){
        if (document.getElementById("help").getAttribute("hidden")){
            document.getElementById("help").removeAttribute("hidden");
            document.getElementById("bt_help").innerHTML = "Un-Help";
        }else{
            document.getElementById("help").setAttribute("hidden", true);
            document.getElementById("bt_help").innerHTML = "Help";
        }
    };
    function stringToFloat(s){
        if (s.search(/^\s*(\+|\-)?\d*(\.\d*)?\s*$/)==-1)
            throw new Error("'"+s+"' is not a valid number", "'"+s+"' is not a valid number");
        return parseFloat(s);
    }
    function f_tare(){
        var strTare = '{"src":"index","what":"tare"}';
        document.getElementById("json2").innerHTML=strTare;
        webSock.send(strTare);
    }
  </script>
</html>
)=====";

  const char WIFI_HTML[] PROGMEM =  R"=====(
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta name="file" content="wifi.html">
    <meta name="author" content="Joe Belson 20210822">
    <meta name="what" content="Set wifi configuration">
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Set WiFi Configuration</title>
    <style>
      #blink {
          font-size: 15px;
          font-weight: bold;
          color: #be2d2d;
          transition: 0.5s;
      }
    </style>
  </head>
  <body style="text-align: left;" onload="f_onload()">
    <input id="bt_help" type="submit" value="Help" style="font-size: 14px;line-height: 1.5;" onclick="f_help()">
    <h2>Dual BME280</h2>
    <p>
      <hr>
      <table class="calibrate">
        <caption style="font-weight: bolder;">Set WiFi Configuration</caption>
         <thead>
          <tr>
            <th>SSID</th>
            <th>PWD</th>
          </tr>
         </thead>
         <tbody>
          <tr>
              <td><input type="text" id="ssid" name="cred" onchange="f_update()" placeholder="WiFi SSID"/></td>
              <td><input type="text" id="pwd"  name="cred" onchange="f_update()" placeholder="WiFi PWD"/></td>
              <td><input type="submit" value="Submit"  id="submit"   onclick="f_submit()"  name="dhcp"></td>
              <td><input type="submit" value="Refresh" id ="refresh" onclick="f_refresh()" name="dhcp"></td>
              <td><input type="submit" value="Restart" id ="restart" onclick="f_restart()" name="dhcp"></td>
          </tr>
          </tbody>
        </table>
        <br>
        <table class="calibrate">
          <label for="isDHCP">DHCP:</label><input type="checkbox" id="isDHCP" onclick="f_dhcp(checked)" onchange="f_update()">
          <a id="blink"></a>
          <tbody>
          <tr>
            <td>
               <input type="text" id="ip" name="dhcp"
                style="width: 7rem;" onchange="f_update()" placeholder="Fixed IP"
                minlength="7" maxlength="15" size="15"
                required pattern="^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$">
            </td>
            <td>
               <input type="text" id="gw" name="dhcp"
                style="width: 7rem;" onchange="f_update()" placeholder="Network Gateway"
                minlength="7" maxlength="15" size="15"
                required pattern="^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$">
            </td>
            <td>
              <input type="text" id="mask" name="dhcp"
                style="width: 7rem;" onchange="f_update()" placeholder="Network Mask"
                minlength="7" maxlength="15" size="15"
                required pattern="^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$">
            </td>
            <td>
              <input type="submit" name="dhcp" id="submit2"  value="Submit" onclick="f_submit()">
              <input type="submit" name="dhcp" id="refresh2" value="Refresh" onclick="f_refresh()">
              <input type="submit" name="dhcp" id="restart2" value="Restart" onclick="f_restart()">
            </td>
          </tr>
          </tbody>
        </table>
        <id id="help" style="visibility:collapse;text-align: left;"><hr>
            1) Click Submit to send new WiFi credentials
        <br>2) Click Refresh to reload the web page - discard all changes
        <br>3) Click Reboot to start the device
        <hr>
        <p id="message" style="text-align: left; visibility: collapse;">Message Area</p>
      </p>
    </body>
    <script language = "javascript" type = "text/javascript">
  
        let webSock       = new WebSocket('ws://'+window.location.hostname+':80');
        webSock.onopen = function(){ 
            let json = {src:"wifi",what:"refresh"};
            webSock.send(JSON.stringify(json));
        };
        webSock.onmessage = function(evt){f_webSockOnMessage(evt);}
        webSock.onerror   = function(evt){f_webSockOnError(evt);}

        var j = {
          src:    "",
          what:   "",
          ip:     "",
          gw:     "",
          mask:   "",
          isDHCP: "",
          ssid:   "",
          pwd:    ""
        };
        let updates = false;
        function f_webSockOnMessage(evt){
          if(updates==true)return;
          if(typeof evt.data === "string"){
            let j=JSON.parse(evt.data);
            if(j.src != "wifi")return;
            document.getElementById("message").innerHTML=JSON.stringify(j);
            for(let key in j){
              let e = document.getElementById(key);
              switch(key){
                case "src":continue;
                case "what":continue;
                case "updated":continue;
                case "isDHCP":e.checked=j[key];continue;
                default:
                  console.log(key+" :",j[key]);
                  e.value=j[key];continue;
              }
            }
          }
        }
        function f_webSockOnError(evt){}
        function f_submit(){
          for(var key in j){
            e=document.getElementById(key);
            switch(key){
              case "src":continue;
              case "what":continue;
              case "updated":continue;
              case "isDHCP":
                j[key]=e.checked?"1":"0"; continue;
              default:
                j[key]=e.value;
          }
          }
          j.src="wifi";j.what="submit";
          console.log(JSON.stringify(j));
          updates = false;
          webSock.send(JSON.stringify(j));
        }
        function f_refresh(){
          const msg='{"src":"wifi","what":"refresh"}';
          console.log(msg);
          updates = false;
          webSock.send(msg);
        }
        function f_dhcp(checked){
          const n=document.getElementsByName("dhcp");
          if(checked){
            for(var i=0;i<n.length;i++)n[i].setAttribute("hidden", "hidden");
            document.getElementById("submit").removeAttribute("hidden");
            document.getElementById("refresh").removeAttribute("hidden");
            document.getElementById("restart").removeAttribute("hidden");
          }else{
            for(var i=0;i<n.length;i++)n[i].removeAttribute("hidden");
            document.getElementById("submit").setAttribute("hidden", "hidden");
            document.getElementById("refresh").setAttribute("hidden", "hidden");
            document.getElementById("restart").setAttribute("hidden", "hidden");
          }
          updates=true;
        }
        function f_update(){
          updates = true;
          var blink = document.getElementById('blink');
          blink.innerHTML="Finish Updating then Click Submit or Refresh";
            setInterval(function() {
                blink.style.opacity =
                 (blink.style.opacity == 0 ? 1 : 0);
                }, 1000);
        }
        function f_help(){
          nxt=document.getElementById("help").style.visibility=="collapse"?"initial":"collapse";
          document.getElementById("bt_help").value=nxt=="collapse"?"Help":"un-Help";
          rray=["help","message"];
          rray.forEach(function(val){document.getElementById(val).style.visibility=nxt;});
        }
        function f_onload(){
          console.log("F_ONLOAD()");
          document.getElementById("submit").setAttribute("hidden","hidden");
          document.getElementById("refresh").setAttribute("hidden","hidden");
          document.getElementById("restart").setAttribute("hidden","hidden");
          document.getElementById("isDHCP").checked=false;
        }
        function f_restart(){webSock.send("{\"src\":\"wifi\",\"what\":\"reboot\"}");}
     </script>
  </html>
      )=====";
