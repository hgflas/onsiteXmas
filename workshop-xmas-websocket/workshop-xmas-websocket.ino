#include <WiFi.h>
#include <Adafruit_NeoPixel.h>
#include <ESPAsyncWebServer.h>

/* Put your SSID & Password */
const char* ssid = "";  // Enter SSID here
const char* password = "!";  //Enter Password here

/* Put IP Address details */
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

/* LED Configuration */
#define LED_PIN     14  // Define pin for LED strip
#define LED_COUNT   12  // Number of LEDs
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);


bool ledStatus = false;  // LED status (on/off)
bool breathing = false;

AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);
  strip.begin();
  strip.setBrightness(50);
  strip.show(); // Initialize all pixels to 'off'

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.printf("WiFi Failed!\n");
  }
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  delay(100);
  
  //Define HTTP Endpoints
  server.on("/", HTTP_GET, handle_OnConnect);
  server.on("/led/on", HTTP_GET, handle_led_on);
  server.on("/led/off", HTTP_GET, handle_led_off); 
  server.on("/led/color", HTTP_GET, handle_set_color);
  server.on("/led/brightness", HTTP_GET, handle_set_brightness);
  server.on("/led/breathingMode/on", HTTP_GET, handle_breathingMode_on);
  server.on("/led/breathingMode/off", HTTP_GET, handle_breathingMode_off); 
  server.on("/led/customMode/on", HTTP_GET, handle_custom_mode_on); 
  server.on("/led/customMode/off", HTTP_GET, handle_custom_mode_off); 
  server.onNotFound(handle_NotFound);
  
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  //server.handleClient();
    if (breathing) {
    breathing_mode();
  }
}

void handle_OnConnect(AsyncWebServerRequest *request){
  Serial.println("Client connected");
  request->send(200, "text/html", SendHTML(ledStatus));
}


void handle_custom_mode_on(AsyncWebServerRequest *request) {
  // TO BE DONE
}


void handle_custom_mode_off(AsyncWebServerRequest *request) {
  // TO BE DONE
}



void handle_breathingMode_on(AsyncWebServerRequest *request) {
  if(ledStatus == true){
    Serial.println("Breathing LED Mode started");
    breathing = true; // Start breathing animation
    request->send(200, "text/html", "Breathing LED Mode started");
  }else{
     Serial.println("LEDs are off, first turn LED ons"); 
     request->send(403, "text/plain", "Error: LED is currently off. Turn on the LED first.");
  }
}

void handle_breathingMode_off(AsyncWebServerRequest *request) {
  Serial.println("Breathing LED Mode stopped");
  breathing = false; // Stop breathing animation
  strip.fill(strip.Color(226, 0, 116)); 
  strip.show();
  request->send(200, "text/html", "Breathing LED Mode stopped");
}

void handle_led_on(AsyncWebServerRequest *request){
  ledStatus = true;
  Serial.println("LED Status: ON");
  strip.fill(strip.Color(226, 0, 116)); // White color
  strip.show();
  request->send(200, "text/html", SendHTML(ledStatus));
}

void handle_led_off(AsyncWebServerRequest *request) {
  ledStatus = false;
  Serial.println("LED Status: OFF");
  strip.fill(strip.Color(0, 0, 0)); // Off
  strip.show();
  request->send(200, "text/html", SendHTML(ledStatus));
}

void handle_set_brightness(AsyncWebServerRequest *request){
  if (request->hasArg("brightness")) {
    int brightness = request->arg("brightness").toInt();
    Serial.println("Brightness received: " + String(brightness)); // Debugging log
    strip.setBrightness(brightness);
    strip.show();  // Apply the brightness change to the LED strip
    request->send(200, "text/plain", "Brightness updated");
  } else {
    request->send(400, "text/plain", "Brightness level not provided");
  }
}

void handle_set_color(AsyncWebServerRequest *request) {
  if(ledStatus == true){
    // Get the RGB value from the URL
    String r = request->arg("r"); 
    String g = request->arg("g"); 
    String b = request->arg("b"); 
    Serial.println("RGB received: " + r + "," + g + "," + b); // Debugging log
  
    setColor(strip.Color(r.toInt(), g.toInt(), b.toInt()));
    request->send(200, "text/html", SendHTML(true)); // Send back the updated HTML
  }else{
     Serial.println("LEDs are off, first turn LED ons"); 
     request->send(403, "text/plain", "Error: LED is currently off. Turn on the LED first.");
  }
}

void handle_NotFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void setColor(uint32_t color) {
  for (int x = 0; x < LED_COUNT; x++) {
     strip.setPixelColor(x, color);
  }
  strip.show();
}

void breathing_mode(){
  static int brightness = 0;         // Brightness level
  static int fadeAmount = 5;         // Step size for fading in and out
  
  // RGB color modulation for smooth breathing effect
  int red = (226 * brightness) / 255;   // Red component based on brightness
  int green = (0 * brightness) / 255;   // Green component stays 0
  int blue = (116 * brightness) / 255;  // Blue component based on brightness

  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, strip.Color(red, green, blue));  // Set color with adjusted brightness
  }

  strip.show();
  
  // Adjust brightness value
  brightness += fadeAmount;

  // Reverse direction when brightness reaches the limits
  if (brightness <= 0 || brightness >= 255) {
    fadeAmount = -fadeAmount;  // Reverse direction
  }

  delay(100);  // Slow down the animation for a smooth breathing effect
}

String SendHTML(bool ledStat) {
  String html = "<!DOCTYPE html>\n"
                 "<html lang=\"en\">\n"
                 "<head>\n"
                 "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
                 "  <title>Onsite Xmas</title>\n"
                  "  <style>\n"
                 "    /* General styling */\n"
                 "    html, body {\n"
                 "      font-family: Arial, sans-serif;\n"
                 "      display: flex;\n"
                 "      justify-content: center;\n"
                 "      align-items: center;\n"
                 "      min-height: 100vh;\n"
                 "      margin: 0;\n"
                 "      background-color: #000;\n"
                 "      color: #fff;\n"
                 "    }\n"
                 "    /* Main container for the app */\n"
                 "    .app-container {\n"
                 "      width: 100%;\n"
                 "      max-width: 100vw;\n"
                 "      min-height: 100vh;\n"
                 "      box-sizing: border-box;\n"
                 "      padding: 20px;\n"
                 "      background-color: #121212;\n"
                 "      display: flex;\n"
                 "      flex-direction: column;\n"
                 "      align-items: center;\n"
                 "      justify-content: center;\n"
                 "    }\n"
                 "    /* Header styling */\n"
                 "    .header {\n"
                 "      background-color: #ff00ff;\n"
                 "      padding: 20px;\n"
                 "      width: 100%;\n"
                 "      display: flex;\n"
                 "      align-items: center;\n"
                 "      justify-content: space-between;\n"
                 "      box-sizing: border-box;\n"
                 "    }\n"
                 "    .header h1 {\n"
                 "      font-size: 1.8rem;\n"
                 "      margin: 0;\n"
                 "    }\n"
                 "    /* Power toggle styling */\n"
                 "    .header-toggle, .mode-toggle {\n"
                 "      display: flex;\n"
                 "      align-items: center;\n"
                 "      margin-top: 20px;\n"
                 "    }\n"
                 "    .header-toggle label {\n"
                 "      font-size: 1.5rem;\n"
                 "      margin-right: 15px;\n"
                 "    }\n"
                 "    .header-toggle input[type=\"checkbox\"], .mode-toggle input[type=\"checkbox\"] {\n"
                 "      width: 50px;\n"
                 "      height: 25px;\n"
                 "      -webkit-appearance: none;\n"
                 "      appearance: none;\n"
                 "      background-color: #ccc;\n"
                 "      border-radius: 25px;\n"
                 "      position: relative;\n"
                 "      outline: none;\n"
                 "      cursor: pointer;\n"
                 "      transition: background-color 0.3s ease;\n"
                 "    }\n"
                 "    .header-toggle input[type=\"checkbox\"]:checked, .mode-toggle input[type=\"checkbox\"]:checked {\n"
                 "      background-color: #2ecc71;\n"
                 "    }\n"
                 "    .header-toggle input[type=\"checkbox\"]::before, .mode-toggle input[type=\"checkbox\"]::before {\n"
                 "      content: \"\";\n"
                 "      position: absolute;\n"
                 "      top: 2px;\n"
                 "      left: 2px;\n"
                 "      width: 21px;\n"
                 "      height: 21px;\n"
                 "      background-color: #fff;\n"
                 "      border-radius: 50%;\n"
                 "      transition: left 0.3s ease;\n"
                 "    }\n"
                 "    .header-toggle input[type=\"checkbox\"]:checked::before, .mode-toggle input[type=\"checkbox\"]:checked::before {\n"
                 "      left: 27px;\n"
                 "    }\n"
                 "    /* Slider container */\n"
                 "    .slider-container {\n"
                 "      display: flex;\n"
                 "      align-items: center;\n"
                 "      width: 100%;\n"
                 "      max-width: 500px;\n"
                 "      margin: 20px 0;\n"
                 "      gap: 10px;\n"
                 "    }\n"
                 "    .slider-container input[type=\"range\"] {\n"
                 "      -webkit-appearance: none;\n"
                 "      appearance: none;\n"
                 "      width: 100%;\n"
                 "      background-color: #ff00ff;\n"
                 "      height: 6px;\n"
                 "      border-radius: 3px;\n"
                 "      outline: none;\n"
                 "    }\n"
                 "    .slider-container input[type=\"range\"]::-webkit-slider-thumb {\n"
                 "      -webkit-appearance: none;\n"
                 "      appearance: none;\n"
                 "      width: 20px;\n"
                 "      height: 20px;\n"
                 "      background-color: #fff;\n"
                 "      border-radius: 50%;\n"
                 "      cursor: pointer;\n"
                 "    }\n"
                 "    /* Content area */\n"
                 "    .content {\n"
                 "      width: 100%;\n"
                 "      max-width: 500px;\n"
                 "      padding: 20px;\n"
                 "      background-color: #121212;\n"
                 "      box-sizing: border-box;\n"
                 "    }\n"
                 "    .control-row {\n"
                 "      display: flex;\n"
                 "      justify-content: space-between;\n"
                 "      align-items: center;\n"
                 "      margin: 10px 0;\n"
                 "    }\n"
                 "    .control-row label {\n"
                 "      font-size: 1.2rem;\n"
                 "    }\n"
                 "    /* Power status */\n"
                 "    .status-on {\n"
                 "      color: #2ecc71;\n"
                 "      font-weight: bold;\n"
                 "    }\n"
                 "    .color-buttons {\n"
                 "      margin-top: 20%;\n"
                 "      display: flex;\n"
                 "      flex-wrap: wrap;\n"
                 "      gap: 10px;\n"
                 "      justify-content: center;\n"
                 "    }\n"
                 "    .color-button {\n"
                 "      padding: 10px;\n"
                 "      border: none;\n"
                 "      border-radius: 5px;\n"
                 "      cursor: pointer;\n"
                 "      color: white;\n"
                 "      font-weight: bold;\n"
                 "      width: 120px;\n"
                 "      text-align: center;\n"
                 "    }\n"
                 "    /* Intensity styling */\n"
                 "    .intensity-value {\n"
                 "      background-color: #ff00ff;\n"
                 "      color: white;\n"
                 "      border-radius: 10px;\n"
                 "      padding: 5px 10px;\n"
                 "      font-weight: bold;\n"
                 "    }\n"
                 "    /* Color picker */\n"
                 "    .color-preview {\n"
                 "      width: 40px;\n"
                 "      height: 40px;\n"
                 "      border-radius: 5px;\n"
                 "      background-color: #ff00ff;\n"
                 "      border: 2px solid #333;\n"
                 "      cursor: pointer;\n"
                 "    }\n"
                 "    /* Toast message */\n"
                 "    #toast {\n"
                 "      visibility: hidden;\n"
                 "      min-width: 300px;\n"
                 "      background-color: #333;\n"
                 "      color: #fff;\n"
                 "      text-align: center;\n"
                 "      padding: 15px;\n"
                 "      border-radius: 5px;\n"
                 "      position: fixed;\n"
                 "      z-index: 1;\n"
                 "      left: 50%;\n"
                 "      bottom: 40px;\n"
                 "      font-size: 16px;\n"
                 "      opacity: 0;\n"
                 "      transition: opacity 0.5s, visibility 0.5s;\n"
                 "      transform: translateX(-50%);\n"
                 "    }\n"
                "    .modes-container {\n"
                "      width: 100%;\n"
                "      max-width: 500px;\n"
                "      padding: 20px;\n"
                "      background-color: #121212;\n"
                "      box-sizing: border-box;\n"
                "      margin-top: 20px;\n"
                "      text-align: center;\n"
                "    }\n"
                "    .mode-toggle {\n"
                "      display: flex;\n"
                "      justify-content: space-between;\n"
                "      align-items: center;\n"
                "      margin: 10px 0;\n"
                "    }\n"
                 "  </style>\n"
                 "  <link rel=\"stylesheet\" href=\"https://cdn.jsdelivr.net/npm/@simonwep/pickr/dist/themes/classic.min.css\" />\n"
                 "</head>\n"
                 "<body>\n"
                 "  <div class=\"app-container\">\n"
                 "    <div class=\"header\">\n"
                 "      <h1>Onsite Xmas</h1>\n"
                 "    </div>\n"
                 "      <div class=\"header-toggle\">\n"
                 "        <label for=\"power-toggle\">Power:</label>\n"
                 "        <input type=\"checkbox\" id=\"power-toggle\" " + String(ledStat ? "checked" : "") + ">\n"
                 "      </div>\n"
                 "    <div class=\"slider-container\">\n"
                 "      <span>-</span>\n"
                 "      <input type=\"range\" id=\"intensity-slider\" min=\"0\" max=\"100\" value=\"75\">\n"
                 "      <span>+</span>\n"
                 "    </div>\n"
                 "    <div class=\"content\">\n"
                 "      <div class=\"control-row\">\n"
                 "        <label>Power:</label>\n"
                 "        <span id=\"power-status\" class=\"status-" + String(ledStat ? "on" : "off") + "\">" + String(ledStat ? "on" : "off") + "</span>\n"
                 "      </div>\n"
                 "      <div class=\"control-row\">\n"
                 "        <label>Intensity:</label>\n"
                 "        <span id=\"intensity-value\" class=\"intensity-value\">75</span>\n"
                 "      </div>\n"
                 "      <div class=\"control-row\">\n"
                 "        <label>Color:</label>\n"
                 "        <div class=\"color-preview\" id=\"color-preview\"></div>\n"
                 "      </div>\n"
                "    <div class=\"modes-container\">\n"
                "      <h2>Modes</h2>\n"
                "      <div class=\"mode-toggle\">\n"
                "        <label for=\"breathing-mode\">Breathing Mode</label>\n"
                "        <input type=\"checkbox\" id=\"breathing-mode\" onclick=\"toggleBreathingMode()\">\n"
                "      </div>\n"
                "      <div class=\"mode-toggle\">\n"
                "        <label for=\"custom-mode\">Custom Mode</label>\n"
                "        <input type=\"checkbox\" id=\"custom-mode\" onclick=\"toggleCustomMode()\">\n"
                "     </div>\n"
                " <div class=\"color-buttons\">\n"
                "   <button class=\"color-button\" style=\"background-color: #ff0000;\" onclick=\"setColor('255,0,0')\">Red</button>\n"
                "   <button class=\"color-button\" style=\"background-color: #0000ff;\" onclick=\"setColor('0,0,255')\">Blue</button>\n"
                "   <button class=\"color-button\" style=\"background-color: #00ff00;\" onclick=\"setColor('0,255,0')\">Green</button>\n"
                "   <button class=\"color-button\" style=\"background-color: #ff00ff;\" onclick=\"setColor('226,0,116')\">Magenta</button>\n"
                "   <button class=\"color-button\" style=\"background-color: #ffa500;\" onclick=\"setColor('255,165,0')\">Orange</button>\n"
                "   <button class=\"color-button\" style=\"background-color: #40e0d0;\" onclick=\"setColor('64,224,208')\">Turquoise</button>\n"
                " </div>\n"
                 "    </div>\n"
                 "  </div>\n"
                 "  <div id=\"toast\">Toast Message</div>\n"
                 "  <script src=\"https://cdn.jsdelivr.net/npm/@simonwep/pickr/dist/pickr.min.js\"></script>\n"
                 "  <script>\n"
                "  const setColor = (rgb) => {\n"
                "   const [r, g, b] = rgb.split(',');\n"
                "   fetch(`/led/color?r=${r}&g=${g}&b=${b}`)\n"
                "      .then(response => {\n"
                "        if (response.ok) {\n"
                "          showToast('Color set successfully!');\n"
                "        } else {\n"
                "          showToast('Error set Color!');\n"
                "        }\n"
                "      })\n"
                "      .catch(error => showToast(`Error: ${error.message}`, true));\n"
                "  };\n"

              "    function toggleBreathingMode() {\n"
              "      const isChecked = document.getElementById('breathing-mode').checked;\n"
              "      fetch(`/led/breathingMode/${isChecked ? 'on' : 'off'}`)\n"
              "        .then(response => {\n"
              "          if (response.ok) {\n"
              "            showToast('Breathing Mode set successfully!');\n"
              "          } else {\n"
                "          showToast('Error set Breathing Mode!');\n"
                "        }\n"
              "        })\n"
              "        .catch(error => showToast(`Error: ${error.message}`, true));\n"
              "    }\n"
              "    function toggleCustomMode() {\n"
              "      const isChecked = document.getElementById('custom-mode').checked;\n"
              "      fetch(`/led/customMode/${isChecked ? 'on' : 'off'}`)\n"
              "        .then(response => {\n"
              "          if (response.ok) {\n"
              "            showToast('Custom Mode set successfully!');\n"
              "          }else {\n"
                "          showToast('Error set Custom Mode!');\n"
                "        }\n"
              "        })\n"
              "        .catch(error => showToast(`Error: ${error.message}`, true));\n"
              "    }\n"

                 "    const showToast = (message, isError = false) => {\n"
                 "      const toast = document.getElementById('toast');\n"
                 "      toast.style.backgroundColor = isError ? '#e74c3c' : '#2ecc71';\n"
                 "      toast.innerText = message;\n"
                 "      toast.style.visibility = 'visible';\n"
                 "      toast.style.opacity = '1';\n"
                 "      setTimeout(() => {\n"
                 "        toast.style.visibility = 'hidden';\n"
                 "        toast.style.opacity = '0';\n"
                 "      }, 3000);\n"
                 "    };\n"
                 "    const powerToggle = document.getElementById('power-toggle');\n"
                 "    const powerStatus = document.getElementById('power-status');\n"
                 "    powerToggle.addEventListener('change', () => {\n"
                 "      const action = powerToggle.checked ? 'on' : 'off';\n"
                 "      fetch(`/led/${action}`)\n"
                 "        .then(response => {\n"
                 "          if (response.status == 200) {\n"
                 "            showToast(`LED turned ${action}`);\n"
                 "          } else {\n"
                 "            showToast('Failed to turn LED off.', true);\n"
                 "          }\n"
                 "        })\n"
                 "        .catch(error => console.error('Error:', error));\n"
                 "    });\n"
                 "    const intensitySlider = document.getElementById('intensity-slider');\n"
                 "    const intensityValue = document.getElementById('intensity-value');\n"
                 "    intensitySlider.addEventListener('input', () => {\n"
                 "      const brightness = intensitySlider.value;\n"
                 "      intensityValue.innerText = brightness;\n"
                 "  fetch(`/led/brightness?brightness=${brightness}`)\n"
                 "    .then(response => response.text())\n"
                 "    .catch(error => console.error(`Error setting brightness:`, error));\n"
                 "}); \n"
                 "    const pickr = Pickr.create({\n"
                 "      el: '#color-preview',\n"
                 "      theme: 'classic',\n"
                 "      default: '#ff00ff',\n"
                 "      swatches: ['#ff00ff', '#e91e63', '#00ff00', '#3498db', '#f39c12', '#d35400'],\n"
                 "      components: { preview: true, opacity: false, hue: false, interaction: { hex: false, rgba: true, input: true, save: true } }\n"
                 "    });\n"
                "    pickr.on('save', (color, instance) => {\n"
                "      const hexColor = color.toHEXA().toString();\n"
                "      const rgba = color.toRGBA();\n"
                "      fetch(`/led/color?r=${rgba[0]}&g=${rgba[1]}&b=${rgba[2]}`)\n"
                              
                "        .then(response => {\n"
                "          if (response.status !== 200) {\n"
                "            return response.text().then(errorMsg => showToast('Color set failed, turn LEDs on!', true));\n"
                "          }\n"
                "          showToast('Color set successfully!', false);\n"
                "        })\n"
                "        .catch(error => showToast(`Error: ${error.message}`, true));\n"
                "      showToast(`Color set to ${hexColor}`);\n"
                "      pickr.hide();\n"
                "    });\n"
                "  </script>\n"
                "</body>\n"
                "</html>";

  return html;
}


