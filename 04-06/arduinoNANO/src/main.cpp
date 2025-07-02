#include <ArduinoJson.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <U8g2lib.h>
#include <MemoryFree.h>
// #include <SPI.h>
U8G2_ST7920_128X64_1_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* CS=*/ 10, /* reset=*/ 8);

#define RX_PIN 5 
#define TX_PIN 6 
#define Addr 0x44
SoftwareSerial mySerial(RX_PIN, TX_PIN); 
StaticJsonDocument<50> doc;
  String nhietdo;
  String doamKK;
  String nhietdoP;
  String doamKKP;

const byte BUFFER_SIZE = 255; 
unsigned long page1DisplayTime = 7000;
unsigned long page2DisplayTime = 3000; 

unsigned long page1StartTime; 
unsigned long page2StartTime; 

bool isPage1 = true; 
String getValue(String data, char separator, int index){
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
      if (data.charAt(i) == separator || i == maxIndex) {
          found++;
          strIndex[0] = strIndex[1] + 1;
          strIndex[1] = (i == maxIndex) ? i+1 : i;
      }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
////////////////////////////////////////////////////////////////////////////////////
void setup()
{
  Wire.begin();
  Serial.begin(9600);
  mySerial.begin(9600);
  u8g2.begin();  

}
////////////////////////////////////////////////////////////////////////////////////
void senddata()  {
unsigned int data[6]; 
  Wire.beginTransmission(Addr);  
  Wire.write(0x2C);  
  Wire.write(0x06);  
  Wire.endTransmission();  
  delay(500);  
  Wire.requestFrom(Addr, 6); 
  if (Wire.available() == 6) { 
    for (int i = 0; i < 6; i++) {
      data[i] = Wire.read();  
    }
  }
  float temp = ((((data[0] * 256.0) + data[1]) * 175) / 65535.0) - 45;
  if (temp >= -10 && temp <= 60) { 
    nhietdo = String(temp, 2);
    nhietdoP = String(temp, 1);
  } 
   else {
    nhietdo = "1";
    nhietdoP = "1";
  }
  float humidity = ((((data[3] * 256.0) + data[4]) * 100) / 65535.0);
  if (humidity > 1 && humidity <= 100) {
    doamKK = String(humidity, 2);
    doamKKP = String(humidity, 1);

  }
  else {
    doamKK = "1";
    doamKKP = "1";

  }
  doc["ND2"] = nhietdo;
  doc["DA2"] = doamKK;
  serializeJson(doc, mySerial);
  mySerial.println();
  serializeJson(doc, Serial);
  Serial.println();
}
////////////////////////////////////////////////////////////////////////////////////
void MUA_icon(U8G2_ST7920_128X64_1_SW_SPI &u8g2, int x, int y) {
    // Vẽ đường ngang cạnh trên
    u8g2.drawLine(x, y + 6, x + 11, y + 6);

    for (int i = 0; i < 4; i++) { //4 tia mưa
        int startX = x + 1 + i * 3; // Tạo khoảng cách giữa các tia mưa
        int startY = y + 6;
        int endX = startX + 2; // Điểm kết thúc dịch sang phải 2 đơn vị so với điểm bắt đầu
        int endY = startY + 7; // Chiều cao của tia mưa
        u8g2.drawLine(startX, startY, endX, endY);
    }
}
void NANG_icon(U8G2_ST7920_128X64_1_SW_SPI &u8g2, int x, int y) {
    u8g2.drawCircle(x + 3, y + 3, 3);
    // Vẽ các tia của mặt trời
    for (int i = 0; i < 12; i++) {
        float angle = (float)i * 30.0 * M_PI / 180.0; // Chuyển đổi độ sang radian
        int x1 = x + 3 + (int)(3 * cos(angle));
        int y1 = y + 3 + (int)(3 * sin(angle));
        int x2 = x + 3 + (int)(5 * cos(angle));
        int y2 = y + 3 + (int)(5 * sin(angle));
        u8g2.drawLine(x1, y1, x2, y2);
    }
}
////////////////////////////////////////////////////////////////////////////////////
void ICON_beday(int x, int y) {
  // Kích thước của biểu tượng
  int iconSize = 10;
  // Vẽ hình dạng của bể nước
  u8g2.drawHLine(x, y + iconSize - 1, iconSize); // Đường nằm ngang dưới cùng của bể nước
  u8g2.drawVLine(x, y, iconSize - 1); // Đường dọc bên trái của bể nước
  u8g2.drawVLine(x + iconSize - 1, y, iconSize - 1); // Đường dọc bên phải của bể nước
  u8g2.drawBox(x + 1, y + 2, iconSize - 1, 7);  // Vùng nước đầy
}
void ICON_becan(int x, int y) {
  // Kích thước của biểu tượng
  int iconSize = 10;
  // Vẽ hình dạng của bể nước
  u8g2.drawHLine(x, y + iconSize - 1, iconSize); // Đường nằm ngang dưới cùng của bể nước
  u8g2.drawVLine(x, y, iconSize - 1); // Đường dọc bên trái của bể nước
  u8g2.drawVLine(x + iconSize - 1, y, iconSize - 1); // Đường dọc bên phải của bể nước
  // Vẽ nước trong bể (bể đầy nước)
  u8g2.drawBox(x + 1, y + 7, iconSize - 1, 2);  // Vùng nước cạn
}
////////////////////////////////////////////////////////////////////////////////////
void Screen1() {
  u8g2.firstPage();
  if (mySerial.available()) { 
    String data1 = mySerial.readString(); 
    Serial.println(data1);
  do {
    u8g2.setFont(u8g2_font_mozart_nbp_tf );
        u8g2.drawStr(0, 16, "NK2:");
        u8g2.drawStr(25, 16, nhietdoP.c_str());
        u8g2.drawStr(50, 16, "\260");
        u8g2.drawStr(55, 16, "C");
        u8g2.drawStr(67 + 5, 16, "DK2:"); 
        u8g2.drawStr(92 + 5, 16, doamKKP.c_str()); 
        u8g2.drawStr(123, 16, "%"); 
        u8g2.drawStr(0, 7, "NK1:");
        u8g2.drawStr(25, 7, getValue(data1, ';', 0).c_str());
        u8g2.drawStr(50, 7, "\260");
        u8g2.drawStr(55, 7, "C");

        u8g2.drawStr(67 + 5, 7, "DK1:");
        u8g2.drawStr(92 + 5, 7, getValue(data1, ';', 1).c_str()); 
        u8g2.drawStr(123, 7, "%");

    u8g2.drawStr(0, 25, "DA1:");
    u8g2.drawStr(25, 25, getValue(data1, ';', 2).c_str());
    u8g2.drawStr(55, 25, "%");
    u8g2.drawStr(67 + 5, 25, "DA2:"); 
    u8g2.drawStr(92 + 5, 25,getValue(data1, ';', 3).c_str()); 
    u8g2.drawStr(123, 25, "%"); 

    u8g2.drawStr(0, 34, "DA3:");
    u8g2.drawStr(25, 34, getValue(data1, ';', 4).c_str());
    u8g2.drawStr(55, 34, "%");
    u8g2.drawStr(67 + 5, 34, "DA4:"); 
    u8g2.drawStr(92 + 5, 34, getValue(data1, ';', 5).c_str());
    u8g2.drawStr(123, 34, "%"); 

    u8g2.drawStr(0, 43, "AS:");
    u8g2.drawStr(25, 43, getValue(data1, ';', 8).c_str());
    u8g2.drawStr(55, 43, "LX");
    // MUA_icon(u8g2, 85, 31);
    // NANG_icon(u8g2, 88, 38);
  if(getValue(data1, ';', 7).toInt() == 1)
    {
        NANG_icon(u8g2, 88, 38);
    }    
     else
    {
        MUA_icon(u8g2, 85, 31);
    }    
    u8g2.drawStr(9, 60, getValue(data1, ';', 9).c_str());
    u8g2.drawStr(30, 60, getValue(data1, ';', 10).c_str());
    u8g2.drawStr(51, 60, getValue(data1, ';', 11).c_str());
    u8g2.drawStr(72, 60, getValue(data1, ';', 12).c_str());
    u8g2.drawStr(93, 60, getValue(data1, ';', 13).c_str());
    u8g2.drawStr(114, 60, getValue(data1, ';', 14).c_str());

    // ICON_beday(111, 37);
    if(getValue(data1, ';', 6).toInt() == 1)
    {
      ICON_becan(111, 37);
    }
    else 
  {
      ICON_beday(111, 37);
  }
   u8g2.drawFrame(0, 64 - 15, 128, 15);
  for (int i = 1; i < 6; i++) {
    u8g2.drawVLine(i * 128 / 6, 64 - 15, 15);
  }

  } while (u8g2.nextPage());
}}
////////////////////////////////////////////////////////////////////////////////////
void Screen2() {
        
    u8g2.firstPage();
    if (mySerial.available()) {
    String data1 = mySerial.readString(); 

    do {
        u8g2.setFont(u8g2_font_7x14_mr);
        u8g2.drawFrame(0, 0, u8g2.getWidth(), u8g2.getHeight());
        u8g2.drawStr(10, 18, "time:");

        u8g2.setFontMode(1); // Chế độ căn giữa

        // Vị trí x ban đầu
        int x = 55;
        u8g2.drawStr(x, 18, getValue(data1, ';', 15).c_str());
        x += u8g2.getStrWidth(getValue(data1, ';', 15).c_str());
        u8g2.drawStr(x, 18, ":");
        x += u8g2.getStrWidth(":"); 
        u8g2.drawStr(x, 18, getValue(data1, ';', 16).c_str());
        x += u8g2.getStrWidth(getValue(data1, ';', 16).c_str());
        u8g2.drawStr(x, 18, ":");
        x += u8g2.getStrWidth(":");
        u8g2.drawStr(x, 18, getValue(data1, ';', 17).c_str());


        u8g2.drawStr(10, 36, "mode:");
        if (getValue(data1, ';', 18).toInt() == 1) {
            u8g2.drawStr(55, 36, "MANUAL");
        } else if (getValue(data1, ';', 18).toInt() == 2) {
            u8g2.drawStr(55, 36, "AUTO_TIME");
        } else if (getValue(data1, ';', 18).toInt() == 3) {
            u8g2.drawStr(55, 36, "AUTO_VALUE");
        }

       u8g2.drawStr(10, 54, "cb mua:");
        if (getValue(data1, ';', 19).toInt() == 1) { 
            u8g2.drawStr(70, 54, "OFF");
        } else if (getValue(data1, ';', 19).toInt() == 2) { 
            u8g2.drawStr(70, 54, "ON");
        }
    } while (u8g2.nextPage());
}}
////////////////////////////////////////////////////////////////////////////////////
void loop()
{
    Serial.print("Free RAM: ");
  Serial.print(freeMemory());
  Serial.println(" bytes");
  // receiveUART();
  senddata();

unsigned long currentTime = millis();
  if (isPage1) {
    if (currentTime - page1StartTime >= page1DisplayTime) {
      isPage1 = false; 
      page2StartTime = currentTime; 
      Screen2(); 
    } else {
      Screen1();
    }
  } else {
    if (currentTime - page2StartTime >= page2DisplayTime) {
      isPage1 = true; 
      page1StartTime = currentTime; 
      Screen1(); 
    } else {
      Screen2();
    }
  }
}
////////////////////////////////////////////////////////////////////////////////////