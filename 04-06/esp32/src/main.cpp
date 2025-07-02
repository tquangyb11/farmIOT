#include <ArduinoJson.h>
#include <WIFIManager.h>
#include <FirebaseESP32.h>
#include <ThingSpeak.h>
#include <Wire.h>
#include "RTClib.h"
#include <BH1750.h>

BH1750 lightMeter;
RTC_DS3231 rtc;
WiFiManager wm;
const char* ssid = "Dang Tuan Quang";
const char* password = "22222222";
#define FIREBASE_HOST "farm2-954e9-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "wsyn96Us2KOQX0cKls0djZva9jSL1HwM3gbYOOI6"
#define SECRET_CH_ID 2262643
#define SECRET_WRITE_APIKEY "6HJ30W0CYCVTYW85"
#define RX_PIN 16
#define TX_PIN 17
#define RESET_PIN 0 //keo cao
#define CB_DAT_1 36
#define CB_DAT_2 33
#define CB_DAT_3 34
#define CB_DAT_4 35
#define CB_AS  32  
const int CB_MUA = 25;
const int CB_CAN = 26;
const int relayPin1 = 15;
const int relayPin2 = 27;
const int relayPin3 = 14;
const int relayPin4 = 13;
const int relayPin5 = 19;
const int relayPin6 = 18; 

const int button1 = 5; //keo cao
const int button2 = 23; //cao
const int button3 = 4;
const int button4 = 2;
const int button5 = 12; //keo thap
const int buttonMODE = 3; //keo cao

bool relay1State = LOW;
bool relay2State = LOW; 
bool relay3State = LOW; 
bool relay4State = LOW; 
bool relay5State = LOW; 
bool relay6State = LOW; 

int anhsang ;
float as_docduoc;
const int minas_docduoc = 0;
const int maxas_docduoc = 4095; // giá trị ADC max cảm biến ánh sáng
float doamdat1;
float doamdat2;
float doamdat3;
float doamdat4;
int cbcan;
int mua_docduoc;

int  gio, phut, giay;
String thu;
bool trangthaican = false; // Biến cờ để kiểm tra trạng thái của CB_CAN
bool trangthaimua = false; // Biến cờ để kiểm tra trạng thái của CB_CAN
bool trangthaichedomua = false;

#define Addr 0x44
WiFiClient client;
unsigned long buttonPressTime = 0; // Biến đếm thời gian nút được nhấn
int buttonState = HIGH; // Trạng thái mặc định của nút là HIGH (không được nhấn)
// unsigned long lastConnectionTime = 0;
// const unsigned long connectionInterval = 7000; // Thời gian giữa các kết nối (30 giây)
StaticJsonDocument<100> doc;
StaticJsonDocument<512> doc2;

unsigned long myChannelNumber = SECRET_CH_ID;
const char *myWriteAPIKey = SECRET_WRITE_APIKEY;
FirebaseData firebaseData;
unsigned long thoiGianTruoc = 0;
const long khoangThoiGian = 1000; // 1 giây
unsigned long dataMillis = 0;

String MODE;
float nhietdo2;
float doamkk2;
float nhietdo;
float doamKK;

String SET_CBMUA; 
String relay1;String relay2; 
String relay3; String relay4;
String relay5;String relay6;
String GOB1S;String GOB2S;String GOB3S;String GOB4S;String GO5;
String POB1S;String POB2S;String POB3S;String POB4S;String PO5;
String GOB1C;String GOB2C;String GOB3C;String GOB4C;String GF5;
String POB1C;String POB2C;String POB3C;String POB4C;String PF5;
String PFB1;String PFB2;String PFB3;String PFB4;
String SLB1;String SLB2;String SLB3;String SLB4;

String AF;String AO;String DO1;String DO2;String DO3;
String DO4;String DF1;String DF2;String DF3;String DF4;

void setup() {
  Wire.begin();     
  Serial.begin(115200);
  Serial2.begin(9600 , SERIAL_8N1, RX_PIN,TX_PIN); 
  WiFi.mode(WIFI_STA);
  if (lightMeter.begin()) {
    Serial.println("BH1750 started successfully.");
  } else {
    Serial.println("Error initializing BH1750.");
  }
  pinMode(RESET_PIN, INPUT_PULLUP);
  pinMode(CB_CAN , INPUT_PULLDOWN);
  pinMode(CB_MUA , INPUT_PULLUP);
  pinMode(button1, INPUT_PULLUP);
  pinMode(button3, INPUT_PULLDOWN);
  pinMode(button5, INPUT_PULLDOWN);
  pinMode(button2, INPUT_PULLUP);
  pinMode(button4, INPUT_PULLDOWN);
  pinMode(buttonMODE, INPUT_PULLUP);
  // attachInterrupt(digitalPinToInterrupt(button1), buttonInterrupt1, FALLING);
  // attachInterrupt(digitalPinToInterrupt(button3), buttonInterrupt2, RISING);
  // attachInterrupt(digitalPinToInterrupt(button5), buttonInterrupt3, FALLING);
  // attachInterrupt(digitalPinToInterrupt(button2), buttonInterrupt4, FALLING);
  // attachInterrupt(digitalPinToInterrupt(button4), buttonInterrupt5, RISING); 
  // attachInterrupt(digitalPinToInterrupt(buttonMODE), buttonModeInterrupt, FALLING);
  pinMode(relayPin1, OUTPUT);
  pinMode(relayPin2, OUTPUT);
  pinMode(relayPin3, OUTPUT);
  pinMode(relayPin4, OUTPUT);
  pinMode(relayPin5, OUTPUT);
  pinMode(relayPin6, OUTPUT);
  digitalWrite(relayPin1, HIGH);
  digitalWrite(relayPin2, HIGH);
  digitalWrite(relayPin3, HIGH);
  digitalWrite(relayPin4, HIGH);
  digitalWrite(relayPin5, HIGH);
  digitalWrite(relayPin6, HIGH);
  
if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    // while (1);
  }
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    rtc.adjust(DateTime(2024, 3, 30, 1, 50, 00));
  }   

    wm.setConnectTimeout(240); // chờ kết nối khi khởi động 4seconds
  if(!wm.autoConnect(ssid, password)) {
    Serial.println("Không thể kết nối.");
  }
   else {
    Serial.println("Kết nối thành công!");
    // digitalWrite(LED_PIN1, HIGH); // Bật LED để báo hiệu kết nối thành công
  }
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  ThingSpeak.begin(client);  // Khởi tạo ThingSpeak
  Firebase.setString(firebaseData, "CONTROL/MODE", "1");
  Firebase.setString(firebaseData, "CONTROL/relay1", "1");
  Firebase.setString(firebaseData, "CONTROL/relay2", "1");
  Firebase.setString(firebaseData, "CONTROL/relay3", "1");
  Firebase.setString(firebaseData, "CONTROL/relay4", "1");
  Firebase.setString(firebaseData, "CONTROL/relay5", "1");  
  Firebase.setString(firebaseData, "CONTROL/relay6", "1");
}
////////////////////////////////////////////////////////////////////////////////
void CB() {
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
  // Kiểm tra dữ liệu nhiệt độ
  float temp = ((((data[0] * 256.0) + data[1]) * 175) / 65535.0) - 45;
  if (temp >= -10 && temp <= 60) { 
    nhietdo = round(temp * 100) / 100; // Làm tròn độ ẩm đến 2 chữ số thập phân
  } else {
    nhietdo = 0;
  }
  // Kiểm tra dữ liệu độ ẩm
  float humidity = ((((data[3] * 256.0) + data[4]) * 100) / 65535.0);
  if (humidity > 1 && humidity <= 100) {
    doamKK = round(humidity * 100) / 100; // Làm tròn độ ẩm đến 2 chữ số thập phân
  } else {
    doamKK = 0;
  }
  float giatri_doam_docduoc1 = analogRead(CB_DAT_1);
  float giatri_doam_docduoc2 = analogRead(CB_DAT_2);
  float giatri_doam_docduoc3 = analogRead(CB_DAT_3);
  float giatri_doam_docduoc4 = analogRead(CB_DAT_4);
  
  float ptdoamdat1 = 100 - map(giatri_doam_docduoc1, 0, 4095, 0, 100); // Chuyển đổi giá trị đọc sang phần trăm độ ẩm
   doamdat1 = ptdoamdat1; // 2 là số lẻ sau dấu phẩy
  float ptdoamdat2 = 100 - map(giatri_doam_docduoc2, 0, 4095, 0, 100); 
   doamdat2 = ptdoamdat2; 
  float ptdoamdat3 = 100 - map(giatri_doam_docduoc3, 0, 4095, 0, 100); 
   doamdat3 = ptdoamdat3; 
  float ptdoamdat4 = 100 - map(giatri_doam_docduoc4, 0, 4095, 0, 100); 
   doamdat4 = ptdoamdat4; 
  // float as_docduoc = analogRead(CB_AS);
  // anhsang = 100 - (float)(as_docduoc - minas_docduoc) / (maxas_docduoc - minas_docduoc) * 100;
  // anhsang = round(anhsang);
    anhsang = lightMeter.readLightLevel(); 

  // mua_docduoc = analogRead(CB_MUA);
  mua_docduoc = digitalRead(CB_MUA);
  trangthaimua = (mua_docduoc == LOW); // Cập nhật trạng thái của biến cờ
  cbcan = digitalRead(CB_CAN);
  trangthaican = (cbcan == HIGH); // Cập nhật trạng thái của biến cờ
}
void receiveDataFromFirebase() {
  String combinedData = "";
  String tempData = "";
  if (Firebase.getString(firebaseData, "CONTROL")) {
    String stringValue = firebaseData.stringData();
    if (!stringValue.isEmpty()) { 
      stringValue.remove(0, 1); 
      stringValue.remove(stringValue.length() - 1);

      tempData += stringValue; 
    } else {
      Serial.println("Received empty string from Firebase1");
    }
  }
  if (Firebase.getString(firebaseData, "TIME")) {
    String stringValue = firebaseData.stringData();
    if (!stringValue.isEmpty()) { 
      stringValue.remove(0, 1); // Xóa dấu ngoặc nhọn mở
      stringValue.remove(stringValue.length() - 1); // Xóa dấu ngoặc nhọn đóng
      if (!tempData.isEmpty()) {
        tempData += ","; 
      }
      tempData += stringValue; 
    } else {
      Serial.println("Received empty string from Firebase2");
    }
  }
  if (Firebase.getString(firebaseData, "VALUE")) {
    String stringValue = firebaseData.stringData();
    if (!stringValue.isEmpty()) { 
      stringValue.remove(0, 1); 
      stringValue.remove(stringValue.length() - 1);

      if (!tempData.isEmpty()) {
        tempData += ","; 
      }
      tempData += stringValue; 
    } else {
      Serial.println("Received empty string from Firebase3");
    }
  }
  combinedData += "{" + tempData + "}"; 
  Serial.print("Combined Data: "); 
  Serial.println(combinedData);
 StaticJsonDocument<1024> doc2;
    String line = combinedData;
    DeserializationError error = deserializeJson(doc2, line);
    MODE     =  doc2["MODE"].as<String>();
        relay1 = doc2["relay1"].as<String>();
        relay2 = doc2["relay2"].as<String>();
        relay3 = doc2["relay3"].as<String>();
        relay4 = doc2["relay4"].as<String>();
        relay5 = doc2["relay5"].as<String>();
        relay6 = doc2["relay6"].as<String>();
        SET_CBMUA = doc2["SET_CBMUA"].as<String>();
        GOB1C = doc2["GOB1C"].as<String>();
        GOB2C = doc2["GOB2C"].as<String>();
        GOB3C = doc2["GOB3C"].as<String>();
        GOB4C = doc2["GOB4C"].as<String>();
        GF5 = doc2["GF5"].as<String>();
        GOB1S = doc2["GOB1S"].as<String>();
        GOB2S = doc2["GOB2S"].as<String>();
        GOB3S = doc2["GOB3S"].as<String>();
        GOB4S = doc2["GOB4S"].as<String>();
        GO5 = doc2["GO5"].as<String>();
        POB1C = doc2 ["POB1C"].as<String>();
        POB2C = doc2["POB2C"].as<String>();
        POB3C = doc2["POB3C"].as<String>();
        POB4C = doc2 ["POB4C"].as<String>();
        PF5 = doc2["PF5"].as<String>();
        POB1S = doc2["POB1S"].as<String>();
        POB2S = doc2["POB2S"].as<String>();
        POB3S = doc2["POB3S"].as<String>();
        POB4S = doc2["POB4S"].as<String>();
        PO5 = doc2["PO5"].as<String>();
        PFB1 = doc2["PFB1"].as<String>();
        PFB2 = doc2["PFB2"].as<String>();
        PFB3 = doc2["PFB3"].as<String>();
        PFB4 = doc2["PFB4"].as<String>();
        SLB1 = doc2["SLB1"].as<String>();
        SLB2 = doc2["SLB2"].as<String>();
        SLB3 = doc2["SLB3"].as<String>();
        SLB4 = doc2["SLB4"].as<String>();
        AF = doc2["AF"].as<String>();
        AO = doc2["AO"].as<String>();
        DF1 = doc2["DF1"].as<String>();
        DF2 = doc2["DF2"].as<String>();
        DF3 = doc2["DF3"].as<String>();
        DF4 = doc2["DF4"].as<String>();
        DO1 = doc2["DO1"].as<String>();
        DO2 = doc2["DO2"].as<String>();
        DO3 = doc2["DO3"].as<String>();
        DO4 = doc2["DO4"].as<String>();

  if (combinedData.isEmpty() || combinedData == "{}") {
    Serial.println("Failed to get data from Firebase");
    Serial.print("Error Reason: ");
    Serial.println(firebaseData.errorReason());
  }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void sendDataToFirebase() {
      if (cbcan == 1){
        Firebase.setString(firebaseData, "/CONTROL/relay1", "1");
        Firebase.setString(firebaseData, "/CONTROL/relay2", "1");
        Firebase.setString(firebaseData, "/CONTROL/relay3", "1");
        Firebase.setString(firebaseData, "/CONTROL/relay4", "1");
        Firebase.setString(firebaseData, "/CONTROL/relay5", "1");
    }
    if (doamkk2 != 0 || nhietdo2 != 0) {
        FirebaseJson json;
        json.add("nhietdo", nhietdo);
        json.add("doamKK", doamKK);
        json.add("doamD1", doamdat1);
        json.add("doamD2", doamdat2);
        json.add("doamD3", doamdat3);
        json.add("doamD4", doamdat4);
        json.add("AS", anhsang);
        json.add("Mua", mua_docduoc);
        json.add("Can", cbcan);
        json.add("relay1PH", String(digitalRead(relayPin1)));
        json.add("relay2PH", String(digitalRead(relayPin2)));
        json.add("relay3PH", String(digitalRead(relayPin3)));
        json.add("relay4PH", String(digitalRead(relayPin4)));
        json.add("relay5PH", String(digitalRead(relayPin5)));
        json.add("relay6PH", String(digitalRead(relayPin6)));
        json.add("nhietdo2", nhietdo2);
        json.add("doamkk2", doamkk2);
        String jsonString;
        json.toString(jsonString);
        Serial.println("JSON data to be sent:");
        Serial.println(jsonString);
        // Gửi chuỗi JSON lên Firebase
        if (Firebase.setJSON(firebaseData, "DATA", json)) {
            Serial.println("Send JSON data successfully");
        } else {
            Serial.println("Failed to send JSON data");
            Serial.print("Error Reason: ");
            Serial.println(firebaseData.errorReason());
        }
    }
    ThingSpeak.setField(1, nhietdo);    
    ThingSpeak.setField(2, nhietdo2);       
    ThingSpeak.setField(3, doamKK); 
    ThingSpeak.setField(4, doamkk2); 
    ThingSpeak.setField(5, doamdat1);    
    ThingSpeak.setField(6, doamdat2);      
    ThingSpeak.setField(7, doamdat3); 
    ThingSpeak.setField(8, doamdat4); 
    if (millis() - dataMillis > 600000) {
      int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
      dataMillis = millis();
      }
}
void relay_MANUAL() {
  digitalWrite(relayPin6, relay6.toInt() == 1 ? HIGH : LOW);
if (!trangthaican) {
  digitalWrite(relayPin1, relay1.toInt() == 1 ? HIGH : LOW); 
  digitalWrite(relayPin2, relay2.toInt() == 1 ? HIGH : LOW); 
  digitalWrite(relayPin3, relay3.toInt() == 1 ? HIGH : LOW); 
  digitalWrite(relayPin4, relay4.toInt() == 1 ? HIGH : LOW); 
    if(digitalRead(relayPin1) == LOW || digitalRead(relayPin2) == LOW || digitalRead(relayPin3) == LOW || digitalRead(relayPin4) == LOW) {
        digitalWrite(relayPin5, LOW); // Bật Relay 5
    } else {
        digitalWrite(relayPin5, HIGH); // Tắt Relay 5
    }
  } else {
    // Nếu CB_CAN ở mức logic 1, không cho phép bật bất kỳ relay nào
    digitalWrite(relayPin1, HIGH);
    digitalWrite(relayPin2, HIGH);
    digitalWrite(relayPin3, HIGH);
    digitalWrite(relayPin4, HIGH);
    digitalWrite(relayPin5, HIGH);
  }
}
void controlRelaysByTime() {
    DateTime now = rtc.now();
    int giohientai = now.hour();
    int phuthientai = now.minute();
    bool anyRelayOn = false;

    int goHour5 = GO5.toInt();
    int goMinute5 = PO5.toInt();
    int offHour5 = GF5.toInt();
    int offMinute5 = PF5.toInt();
 if (goHour5 < offHour5 || (goHour5 == offHour5 && goMinute5 < offMinute5)) {
    //không vượt qua ngày
    if ((giohientai > goHour5 || (giohientai == goHour5 && phuthientai >= goMinute5)) &&
        (giohientai < offHour5 || (giohientai == offHour5 && phuthientai < offMinute5))) {
        digitalWrite(relayPin6, LOW);
    } else {
        digitalWrite(relayPin6, HIGH);
    }
} else {
    // vượt qua ngày
    if ((giohientai > goHour5 || (giohientai == goHour5 && phuthientai >= goMinute5)) ||
        (giohientai < offHour5 || (giohientai == offHour5 && phuthientai < offMinute5))) {
        digitalWrite(relayPin6, LOW);
    } else {
        digitalWrite(relayPin6, HIGH);
    }
}

        int Solanbat1 = SLB1.toInt();  
        int giobatsang1 = GOB1S.toInt(); 
        int phutbatsang1 = POB1S.toInt(); 
        int giobatchieu1 = GOB1C.toInt(); 
        int phutbatchieu1 = POB1C.toInt(); 
        int sophutbat1 = PFB1.toInt(); 

        int Solanbat2 = SLB2.toInt(); 
        int giobatsang2 = GOB2S.toInt();
        int phutbatsang2 = POB2S.toInt(); 
        int giobatchieu2 = GOB2C.toInt(); 
        int phutbatchieu2 = POB2C.toInt(); 
        int sophutbat2 = PFB2.toInt(); 

        int Solanbat3 = SLB3.toInt(); 
        int giobatsang3 = GOB3S.toInt(); 
        int phutbatsang3 = POB3S.toInt(); 
        int giobatchieu3 = GOB3C.toInt(); 
        int phutbatchieu3 = POB3C.toInt(); 
        int sophutbat3 = PFB3.toInt();

        int Solanbat4 = SLB4.toInt(); 
        int giobatsang4 = GOB4S.toInt(); 
        int phutbatsang4 = POB4S.toInt(); 
        int giobatchieu4 = GOB4C.toInt(); 
        int phutbatchieu4 = POB4C.toInt(); 
        int sophutbat4 = PFB4.toInt(); 

    if (trangthaichedomua && !trangthaican) {

        int offMinute1 = (phutbatsang1 + sophutbat1) % 60;
        int offMinute2 = (phutbatchieu1 + sophutbat1) % 60;

        if (Solanbat1 == 1) {
            if (giohientai == giobatsang1 && phuthientai >= phutbatsang1 && phuthientai < offMinute1) {
                digitalWrite(relayPin1, LOW);
                anyRelayOn = true;
            } else {
                digitalWrite(relayPin1, HIGH);
            }
        } else if (Solanbat1 == 2) {
            if ((giohientai == giobatsang1 && phuthientai >= phutbatsang1 && phuthientai < offMinute1) ||
                (giohientai == giobatchieu1 && phuthientai >= phutbatchieu1 && phuthientai < offMinute2)) {
                digitalWrite(relayPin1, LOW);
                anyRelayOn = true;
            } else {
                digitalWrite(relayPin1, HIGH);
            }
        }
        int offMinute2_1 = (phutbatsang2 + sophutbat2) % 60;
        int offMinute2_2 = (phutbatchieu2 + sophutbat2) % 60;
        
        if (Solanbat2 == 1) {
            if (giohientai == giobatsang2 && phuthientai >= phutbatsang2 && phuthientai < offMinute2_1) {
                digitalWrite(relayPin2, LOW);
                anyRelayOn = true;
            } else {
                digitalWrite(relayPin2, HIGH);
            }
        } else if (Solanbat2 == 2) {
            if ((giohientai == giobatsang2 && phuthientai >= phutbatsang2 && phuthientai < offMinute2_1) ||
                (giohientai == giobatchieu2 && phuthientai >= phutbatchieu2 && phuthientai < offMinute2_2)) {
                digitalWrite(relayPin2, LOW);
                anyRelayOn = true;
            } else {
                digitalWrite(relayPin2, HIGH);
            }
        }
        int offMinute3_1 = (phutbatsang3 + sophutbat3) % 60;
        int offMinute3_2 = (phutbatchieu3 + sophutbat3) % 60;

        if (Solanbat3 == 1) {
            if (giohientai == giobatsang3 && phuthientai >= phutbatsang3 && phuthientai < offMinute3_1) {
                digitalWrite(relayPin3, LOW);
                anyRelayOn = true;
            } else {
                digitalWrite(relayPin3, HIGH);
            }
        } else if (Solanbat3 == 2) {
            if ((giohientai == giobatsang3 && phuthientai >= phutbatsang3 && phuthientai < offMinute3_1) ||
                (giohientai == giobatchieu3 && phuthientai >= phutbatchieu3 && phuthientai < offMinute3_2)) {
                digitalWrite(relayPin3, LOW);
                anyRelayOn = true;
            } else {
                digitalWrite(relayPin3, HIGH);
            }
        }
        int offMinute4_1 = (phutbatsang4 + sophutbat4) % 60;
        int offMinute4_2 = (phutbatchieu4 + sophutbat4) % 60;

          if (Solanbat4 == 1) {
            if (giohientai == giobatsang4 && phuthientai >= phutbatsang4 && phuthientai < offMinute4_1) {
                digitalWrite(relayPin4, LOW);
                anyRelayOn = true;
            } else {
                digitalWrite(relayPin4, HIGH);
            }
        } else if (Solanbat4 == 2) {
            if ((giohientai == giobatsang4 && phuthientai >= phutbatsang4 && phuthientai < offMinute4_1) ||
                (giohientai == giobatchieu4 && phuthientai >= phutbatchieu4 && phuthientai < offMinute4_2)) {
                digitalWrite(relayPin4, LOW);
                anyRelayOn = true;
            } else {
                digitalWrite(relayPin4, HIGH);
            }
        }
    } else if (!trangthaican && !trangthaimua) {

        int offMinute1 = (phutbatsang1 + sophutbat1) % 60;
        int offMinute2 = (phutbatchieu1 + sophutbat1) % 60;

        if (Solanbat1 == 1) {
            if (giohientai == giobatsang1 && phuthientai >= phutbatsang1 && phuthientai < offMinute1) {
                digitalWrite(relayPin1, LOW);
                anyRelayOn = true;
            } else {
                digitalWrite(relayPin1, HIGH);
            }
        } else if (Solanbat1 == 2) {
            if ((giohientai == giobatsang1 && phuthientai >= phutbatsang1 && phuthientai < offMinute1) ||
                (giohientai == giobatchieu1 && phuthientai >= phutbatchieu1 && phuthientai < offMinute2)) {
                digitalWrite(relayPin1, LOW);
                anyRelayOn = true;
            } else {
                digitalWrite(relayPin1, HIGH);
            }
        }
        int offMinute2_1 = (phutbatsang2 + sophutbat2) % 60;
        int offMinute2_2 = (phutbatchieu2 + sophutbat2) % 60;
        
        if (Solanbat2 == 1) {
            if (giohientai == giobatsang2 && phuthientai >= phutbatsang2 && phuthientai < offMinute2_1) {
                digitalWrite(relayPin2, LOW);
                anyRelayOn = true;
            } else {
                digitalWrite(relayPin2, HIGH);
            }
        } else if (Solanbat2 == 2) {
            if ((giohientai == giobatsang2 && phuthientai >= phutbatsang2 && phuthientai < offMinute2_1) ||
                (giohientai == giobatchieu2 && phuthientai >= phutbatchieu2 && phuthientai < offMinute2_2)) {
                digitalWrite(relayPin2, LOW);
                anyRelayOn = true;
            } else {
                digitalWrite(relayPin2, HIGH);
            }
        }
        int offMinute3_1 = (phutbatsang3 + sophutbat3) % 60;
        int offMinute3_2 = (phutbatchieu3 + sophutbat3) % 60;

        if (Solanbat3 == 1) {
            if (giohientai == giobatsang3 && phuthientai >= phutbatsang3 && phuthientai < offMinute3_1) {
                digitalWrite(relayPin3, LOW);
                anyRelayOn = true;
            } else {
                digitalWrite(relayPin3, HIGH);
            }
        } else if (Solanbat3 == 2) {
            if ((giohientai == giobatsang3 && phuthientai >= phutbatsang3 && phuthientai < offMinute3_1) ||
                (giohientai == giobatchieu3 && phuthientai >= phutbatchieu3 && phuthientai < offMinute3_2)) {
                digitalWrite(relayPin3, LOW);
                anyRelayOn = true;
            } else {
                digitalWrite(relayPin3, HIGH);
            }
        }
        int offMinute4_1 = (phutbatsang4 + sophutbat4) % 60;
        int offMinute4_2 = (phutbatchieu4 + sophutbat4) % 60;

          if (Solanbat4 == 1) {
            if (giohientai == giobatsang4 && phuthientai >= phutbatsang4 && phuthientai < offMinute4_1) {
                digitalWrite(relayPin4, LOW);
                anyRelayOn = true;
            } else {
                digitalWrite(relayPin4, HIGH);
            }
        } else if (Solanbat4 == 2) {
            if ((giohientai == giobatsang4 && phuthientai >= phutbatsang4 && phuthientai < offMinute4_1) ||
                (giohientai == giobatchieu4 && phuthientai >= phutbatchieu4 && phuthientai < offMinute4_2)) {
                digitalWrite(relayPin4, LOW);
                anyRelayOn = true;
            } else {
                digitalWrite(relayPin4, HIGH);
            }
        }
    }
    else {
          digitalWrite(relayPin1, HIGH);
          digitalWrite(relayPin2, HIGH);
          digitalWrite(relayPin3, HIGH);
          digitalWrite(relayPin4, HIGH);
          digitalWrite(relayPin5, HIGH);
        }
    if (!anyRelayOn) {
        digitalWrite(relayPin5, HIGH);
    }
    else {
      digitalWrite(relayPin5, LOW);
    }
}
//////////////////////////////////////////////////////////////////////////////////////////
void relay_VALUE (){
      Serial.println(AO.toInt());
      Serial.println(AF.toInt());
      Serial.println(DO1.toInt());
      Serial.println(DF1.toInt());
      Serial.println(DO2.toInt());
      Serial.println(DF2.toInt());
      Serial.println(DO3.toInt());
      Serial.println(DF3.toInt());
      Serial.println(DO4.toInt());
      Serial.println(DF4.toInt());

    int AF_value = AF.toInt();
    int AO_value = AO.toInt();
    if (anhsang < AO_value) {
        digitalWrite(relayPin6, LOW); 
    } else if (anhsang > AF_value) {
        digitalWrite(relayPin6, HIGH); 
    }
  if (trangthaichedomua && !trangthaican) {
  // Kiểm tra và điều khiển relay 1
    float DF1_value = DF1.toFloat();
    float DO1_value = DO1.toFloat();
    if (doamdat1 < DO1_value) {
        digitalWrite(relayPin1, LOW); 
    } else if (doamdat1 > DF1_value) {
        digitalWrite(relayPin1, HIGH); 
    }
    float DF2_value = DF2.toFloat();
    float DO2_value = DO2.toFloat();
    if (doamdat2 < DO2_value) {
        digitalWrite(relayPin2, LOW);
    } else if (doamdat2 > DF2_value) {
        digitalWrite(relayPin2, HIGH); 
    }
        float DF3_value = DF3.toFloat();
    float DO3_value = DO3.toFloat();
    if (doamdat3 < DO3_value) {
        digitalWrite(relayPin3, LOW); 
    } else if (doamdat3 > DF3_value) {
        digitalWrite(relayPin3, HIGH);
    }
        float DF4_value = DF4.toFloat();
    float DO4_value = DO4.toFloat();
    if (doamdat4 < DO4_value) {
        digitalWrite(relayPin4, LOW); 
    } else if (doamdat4 > DF4_value) {
        digitalWrite(relayPin4, HIGH); 
    }
    }
   else if (!trangthaican && !trangthaimua) {
    float DF1_value = DF1.toFloat();
    float DO1_value = DO1.toFloat();
    if (doamdat1 < DO1_value) {
        digitalWrite(relayPin1, LOW); 
    } else if (doamdat1 > DF1_value) {
        digitalWrite(relayPin1, HIGH);
    }
    float DF2_value = DF2.toFloat();
    float DO2_value = DO2.toFloat();
    if (doamdat2 < DO2_value) {
        digitalWrite(relayPin2, LOW); 
    } else if (doamdat2 > DF2_value) {
        digitalWrite(relayPin2, HIGH);
    }
        float DF3_value = DF3.toFloat();
    float DO3_value = DO3.toFloat();
    if (doamdat3 < DO3_value) {
        digitalWrite(relayPin3, LOW); 
    } else if (doamdat3 > DF3_value) {
        digitalWrite(relayPin3, HIGH);
    }
        float DF4_value = DF4.toFloat();
    float DO4_value = DO4.toFloat();
    if (doamdat4 < DO4_value) {
        digitalWrite(relayPin4, LOW); 
    } else if (doamdat4 > DF4_value) {
        digitalWrite(relayPin4, HIGH);
    }
    }
    else {
    digitalWrite(relayPin1, HIGH);
    digitalWrite(relayPin2, HIGH);
    digitalWrite(relayPin3, HIGH);
    digitalWrite(relayPin4, HIGH);
    digitalWrite(relayPin5, HIGH);
    }
    if (digitalRead(relayPin1) == LOW || digitalRead(relayPin2) == LOW || digitalRead(relayPin3) == LOW || digitalRead(relayPin4) == LOW) {
    digitalWrite(relayPin5, LOW);
} else {
    digitalWrite(relayPin5, HIGH);
}
}
//////////////////////////////////////////////////////////////////////////////////////////
int previousModeValue;
void xuLyGiaTriMode(String mode) {
    if (SET_CBMUA == "1") {
        trangthaichedomua = true;
    } else if (SET_CBMUA == "2") {
        trangthaichedomua = false;
    }
  int modeValue = mode.toInt(); 

  if (modeValue == 1) {
    relay_MANUAL(); 
  }
  if (modeValue == 2) {
    controlRelaysByTime(); 
  }
  if (modeValue == 3) {
        relay_VALUE ();
  }
  if (modeValue != previousModeValue) {
    // Gán giá trị mode mới cho biến previousModeValue
    previousModeValue = modeValue;

    switch (modeValue) {
      case 1:
        Serial.println("Đang ở chế độ 1 - Điều khiển Relay");
        break;
      case 2:
        Serial.println("Đang ở chế độ 2 - Điều khiển TIME");
        break;
      case 3:
        Serial.println("Đang ở chế độ 3 - Điều khiển SENSOR");
            digitalWrite(relayPin1, HIGH);
            digitalWrite(relayPin2, HIGH);
            digitalWrite(relayPin3, HIGH);
            digitalWrite(relayPin4, HIGH);
            digitalWrite(relayPin5, HIGH);
            digitalWrite(relayPin6, HIGH);
        break;
    }
  }
}
//////////////////////////////////////////////////////////////////////////////////////////
void resetWF() {
  buttonState = digitalRead(RESET_PIN); 
  if (buttonState == LOW) {
    buttonPressTime = millis();
    while (digitalRead(RESET_PIN) == LOW) {
      delay(10);
      if (millis() - buttonPressTime >= 4000) {
        Serial.println("Đang xóa cài đặt và khởi động lại...");
        wm.resetSettings(); // Reset các cài đặt WiFi
        ESP.restart(); // Khởi động lại ESP32
      }
    }
  }
}
  //////////////////////////////////////////////////////////////////////////////////////////
  void layThoiGianTuRTC() {
  DateTime now = rtc.now();
  gio = now.hour();
  phut = now.minute();
  giay = now.second();
  Serial.print("Giờ hiện tại là: ");
  Serial.print(gio);
  Serial.print(":");
  Serial.print(phut);
  Serial.print(":");
  Serial.println(giay);
}
void SenddataUART()  {
  String Data = String(nhietdo,1) + ";" + String(doamKK,1)+ ";" + 
  int(doamdat1)+ ";" + int(doamdat2)+ ";" + int(doamdat3)+ ";" + 
  int(doamdat4)+ ";" + String(cbcan)+ ";" + String(mua_docduoc)+ ";" + 
  int(anhsang)+ ";" + String(!digitalRead(relayPin1))+ ";" + 
  String(!digitalRead(relayPin2))+ ";" + String(!digitalRead(relayPin3))+ ";" + String(!digitalRead(relayPin4))+ ";" + 
  String(!digitalRead(relayPin5))+ ";" + String(!digitalRead(relayPin6))+ ";" + int(gio) + ";" + int(phut) + ";" + int(giay) + ";" + String(MODE)+ ";" + String(SET_CBMUA);
  Serial2.println(Data); //Gửi giá trị nhiệt độ và độ ẩm qua Software serial
  Serial.println(Data); //In ra Serial Monitor chuỗi gửi đi
}
int currentMode = 1;
void button (){
  int button1State = digitalRead(button1);
  if (button1State == LOW) {
    relay1State = !relay1State;
    digitalWrite(relayPin1, relay1State);
    Firebase.setString(firebaseData, "/CONTROL/relay1", String(relay1State));
    delay(200); 
  }
  int button2State = digitalRead(button2);
  if (button2State == LOW) {
    relay4State = !relay4State;
    digitalWrite(relayPin4, relay4State);
    Firebase.setString(firebaseData, "/CONTROL/relay4", String(relay4State));
    delay(200); 
  }
   int button3State = digitalRead(button3);
  if (button3State == HIGH) {
    relay2State = !relay2State;
    digitalWrite(relayPin2, relay2State);
    Firebase.setString(firebaseData, "/CONTROL/relay2", String(relay2State));
    delay(200); 
  }
  int button4State = digitalRead(button4);
  if (button4State == HIGH) {
    relay6State = !relay6State;
    digitalWrite(relayPin6, relay6State);
    Firebase.setString(firebaseData, "/CONTROL/relay6", String(relay6State));
    delay(200);
  }
   int button5State = digitalRead(button5);
  if (button5State == HIGH) {
    relay3State = !relay3State;
    digitalWrite(relayPin3, relay3State);
    Firebase.setString(firebaseData, "/CONTROL/relay3", String(relay3State));
    delay(200); 
  }
      if(digitalRead(relayPin1) == LOW || digitalRead(relayPin2) == LOW || digitalRead(relayPin3) == LOW || digitalRead(relayPin4) == LOW) {
        digitalWrite(relayPin5, LOW); 
    } else {
        digitalWrite(relayPin5, HIGH); 
    }
int buttonModeState = digitalRead(buttonMODE);
if (buttonModeState == LOW) {
  currentMode++; 
  if (currentMode > 3) {
    currentMode = 1; 
  }
  Firebase.setInt(firebaseData, "/CONTROL/MODE", currentMode);
  delay(200); 
}
}
//////////////////////////////////////////////////////////////////////////////////////////
void loop() {
   button();
    Serial.print("MODECURREN:  ");
    Serial.println(currentMode);
    SenddataUART();
  if (Serial2.available()) {
    String line2 = Serial2.readStringUntil('\n');
    DeserializationError error = deserializeJson(doc, line2);
    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }
    nhietdo2 = doc["ND2"].as<float>();
    doamkk2 = doc["DA2"].as<float>();
      serializeJson(doc, Serial);
      Serial.println();
  }
  layThoiGianTuRTC();
  resetWF();
  receiveDataFromFirebase();
  xuLyGiaTriMode(MODE);
  CB();
    unsigned long thoiGianHienTai = millis();
    if (thoiGianHienTai - thoiGianTruoc >= khoangThoiGian) {
      sendDataToFirebase();
      thoiGianTruoc = thoiGianHienTai;
    }
  }
///////////////////////////////////////////////////////////////////////