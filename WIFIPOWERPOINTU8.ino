#include <U8g2lib.h>
#include <Wire.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include "Assets.h"

U8G2_ST7567_ENH_DG128064I_F_HW_I2C u8g2(U8G2_R2);

// Default Wi-Fi credentials
const char* ssid = "Samir_2.4G";
const char* password = "soutigrao";

// Wi-Fi credentials for Mode 2
const char* ssid_mode2 = "PROVASP2024";
const char* password_mode2 = "@ProvaSP#24";

const char* ssid_mode3 = "LOL13";
const char* password_mode3 = "12345678";

WiFiUDP udp;
int localPort = 1337;  // UDP port

const int Right = 26;
const int Left = 25;
const int Volt = 27;

const char* tip[] = {"UwU", "OwO", ":3", ">:3", "DDDDD", "XD", "i lost my bf", "i got bf", "Nya", "Nyan", "highs", "Kys now!" , "Ecs dee", "sv_cheats", "Monster!!", "Heard", "ROBUX??", "Boykisser", "kiss me.", ">XD"};

char packetBuffer[255];

IPAddress clientIP;
bool clientKnown = false;

void drawmenu() {
  u8g2.drawXBM(0, 0, 128, 64, smenu);  // Draw the current frame (adjust dimensions if needed)
  u8g2.sendBuffer();             // Transfer the buffer to the display
}


void drawstatus(const uint8_t* bitmap) {
    if (bitmap != nullptr) {
        u8g2.setDrawColor(0);
        u8g2.drawBox(104, 10, 24, 46);
        u8g2.setDrawColor(1);
        u8g2.drawXBM(104, 10, 24, 46, bitmap);
        u8g2.sendBuffer();
    } else {
        Serial.println("Wrong bitmap type");
    }
}

void drawsectors(int cub) {
  if(cub == 0) {
    u8g2.setDrawColor(1);
    u8g2.drawBox(3, 44, 10, 10);
    u8g2.drawBox(14, 42, 10, 10);
    u8g2.drawBox(25, 40, 10, 10);
  }
  else {
    u8g2.setDrawColor(0);
    if(cub == 1) {
      u8g2.drawBox(3, 44, 10, 10);
      u8g2.sendBuffer();
      u8g2.setDrawColor(1);
      u8g2.drawXBM(3, 44, 10, 10, sleft);
    }
    if(cub == 2) {
      u8g2.drawBox(14, 42, 10, 10);
      u8g2.sendBuffer();
      u8g2.setDrawColor(1);
      u8g2.drawXBM(14, 42, 10, 10, sright);
    }
    if(cub == 3) {
      u8g2.drawBox(25, 40, 10, 10);
      u8g2.sendBuffer();
      u8g2.setDrawColor(1);
      u8g2.drawXBM(25, 40, 10, 10, svolt);
    }
  }
  u8g2.sendBuffer();  
  delay(100);
  u8g2.setDrawColor(1);
  u8g2.drawBox(3, 44, 10, 10);
  u8g2.drawBox(14, 42, 10, 10);
  u8g2.drawBox(25, 40, 10, 10);
  u8g2.sendBuffer();

}

void setup(void) {
  Serial.begin(115200);
  u8g2.setI2CAddress(0x3F * 2);  // Set the I2C address
  u8g2.begin();                  // Initialize the display
  u8g2.setBitmapMode(true);
  u8g2.clearBuffer();            // Clear the internal buffer
  u8g2.setFont(u8g2_font_u8glib_4_tf);  // Set the font

  pinMode(Right, INPUT_PULLUP);
  pinMode(Left, INPUT_PULLUP);
  pinMode(Volt, INPUT_PULLUP);
  drawmenu();
  drawstatus(sthinking);

  if (digitalRead(Left) == LOW) {
    Serial.println("Mode 2 Enabled");
    ssid = ssid_mode2;          // Switch to Mode 2 Wi-Fi credentials
    password = password_mode2;  // Switch to Mode 2 password
  }

  if (digitalRead(Right) == LOW) {
    Serial.println("Mode 3 Enabled");
    ssid = ssid_mode3;          // Switch to Mode 3 Wi-Fi credentials
    password = password_mode3;  // Switch to Mode 3 password
  }

  if (digitalRead(Right) == LOW || digitalRead(Left) == LOW) {

    u8g2.drawStr(0, 4, "Changing..");  // Write text to the display buffer
    u8g2.sendBuffer();  // Transfer the buffer to the display
    delay(500);
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to Wi-Fi...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi!");

  udp.begin(localPort);

    u8g2.drawStr(0, 4, "Connected");  // Write text to the display buffer
    u8g2.sendBuffer();  // Transfer the buffer to the display

  delay(1000);

  // Convert IP address to String
  String ipAddress = WiFi.localIP().toString();



  u8g2.clearBuffer();

  drawmenu();

  String ipString = "IP : " + ipAddress;
  u8g2.drawStr(0, 4, ipString.c_str());  // Write text to the display buffer

  String portString = "Port : " + String(localPort);
  u8g2.drawStr(0, 8, portString.c_str());  // Write text to the display buffer

  u8g2.sendBuffer();  // Transfer the buffer to the display


  drawstatus(shappy);

  u8g2.drawXBM(46, 15, 50, 32, swindow);
  u8g2.drawStr(48, 23, "Did you know");
  int randomIndex = random(0, 20); // Generate random index between 0 and 2
  u8g2.drawStr(50, 27, tip[randomIndex]); // Use 'tip' instead of 'char'
  u8g2.sendBuffer();  // Transfer the buffer to the display

  delay(1000);
  drawstatus(sidle);

  drawsectors(0);


}




// Modified sendUDP function
void sendUDP(const char* message) {
    // Check for any incoming packets first
    int packetSize = udp.parsePacket();
    if (packetSize) {
        clientIP = udp.remoteIP();
        clientKnown = true;
        Serial.printf("New client detected: %s:%d\n", clientIP.toString().c_str(), 1337);
        drawstatus(sthinking);
        delay(100);
        drawstatus(sidle);
    }

    // Only send if we know the client
    if (clientKnown) {
        udp.beginPacket(clientIP, 1337);
        udp.write((uint8_t*)message, strlen(message));
        udp.endPacket();
        Serial.printf("Sent '%s' to %s:%d\n", message, clientIP.toString().c_str(), 1337);
    } else {
        Serial.println("No client known yet, waiting...");
    }
}

void loop(void) {
  if (digitalRead(Left) == LOW) {
    drawsectors(1);
    sendUDP("l");
  }

  if (digitalRead(Right) == LOW) {
    drawsectors(2);
    sendUDP("r");
  }

  if (digitalRead(Volt) == LOW) {
    drawsectors(3);
  }

  if (random(1, 50) == 1) {
    // Clear the area before drawing the new box and text
    u8g2.setDrawColor(0);  // Set the draw color to black (clear)
    u8g2.drawBox(48, 23, 43, 23);  // Clear the previous box area
    
    u8g2.setDrawColor(1);  // Set the draw color to white (for text)
    int randomIndex = random(0, 20);  // Generate random index between 0 and 19
    u8g2.drawStr(50, 27, tip[randomIndex]);  // Draw the new string
    
    u8g2.sendBuffer();  // Send the buffer once after all drawing operations
  }


  delay(100);
}
  