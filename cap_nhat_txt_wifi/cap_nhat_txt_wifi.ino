#include <WiFi.h> 
#include <HTTPClient.h>
#include <GxEPD2_3C.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans18pt7b.h>
#include <SPI.h>
#include <QRCodeGFX.h>

const char* ssid = "Wifi123";
const char* password = "123456";
const String websiteUrl = "https://huykhong.com/IOT/hello.php";

GxEPD2_3C<GxEPD2_420_Z98c, GxEPD2_420_Z98c::HEIGHT> display(
    GxEPD2_420_Z98c(/*CS=*/ 15, /*DC=*/ 27, /*RST=*/ 26, /*BUSY=*/ 25)
);

String lastFetchedData = "";
QRCodeGFX qrcode(display);

void displayQRCode(String text) {
   // display.clearScreen();
    //display.setFullWindow();
    //display.firstPage();

    do {
        //display.fillScreen(GxEPD_WHITE);
        //display.setTextColor(GxEPD_BLACK);

        bool success = qrcode.generateData(text);
        if (success) {
          qrcode.setScale(4);        
          int16_t sideLength = qrcode.getSideLength();

          int16_t x = display.width() - sideLength;
          int16_t y = display.height()  / 4;

          qrcode.draw(x, y);
        }

    } while (display.nextPage());

    display.hibernate();
}

// 🔹 Xử lý Markdown & HTML (nâng cấp để kiểm tra QR)
void displayFormattedText(String text) {
    display.clearScreen();
    display.setFullWindow();
    display.firstPage();
    int append_line = 0;
    
    bool showQR = false;
    String qrContent = "";
    
    // Kiểm tra xem có phần nào bắt đầu bằng "[QR]" không
    int qrPos = text.indexOf("[QR]");
    if (qrPos != -1) {
        // Tìm vị trí kết thúc của nội dung QR (dấu xuống dòng tiếp theo hoặc hết chuỗi)
        int endPos = text.indexOf('\n', qrPos);
        if (endPos == -1) {
            qrContent = text.substring(qrPos + 4); // Lấy toàn bộ phần còn lại
            text = text.substring(0, qrPos); // Giữ phần text trước "[QR]"
        } else {
            qrContent = text.substring(qrPos + 4, endPos);
            // Loại bỏ phần QR khỏi text, giữ phần còn lại
            text = text.substring(0, qrPos) + text.substring(endPos + 1);
            Serial.print(text);
        }
        showQR = true;
    }
    
    // Thay thế thẻ <br> bằng xuống dòng
    text.replace("<br>", "\n");
    
    do {
        display.fillScreen(GxEPD_WHITE);
        display.setTextColor(GxEPD_BLACK);

          int16_t x = 10, y = 10;
          int maxWidth = showQR ? 200 : 280;
          String currentLine = "";

          while (text.length() > 0) {
              int breakPos = text.indexOf('\n');
              String line = (breakPos == -1) ? text : text.substring(0, breakPos);
              Serial.print(line);

              // Font lớn cho tiêu đề
              if (line.startsWith("[H1]")) {
                  y += 40;
                  display.setTextColor(GxEPD_RED);
                  display.setFont(&FreeSans18pt7b);                  
                  line = line.substring(4);
              } else if(line.startsWith("[H2]")) {
                  y += 10;
                  display.setFont(&FreeSans12pt7b);
                  display.setTextColor(GxEPD_BLACK);
                  line = line.substring(4);
              } else {
                  display.setFont(&FreeSans9pt7b);
                  display.setTextColor(GxEPD_BLACK);
              }

              display.setCursor(x, y);
              display.println(line);
              y += 20;

              text = (breakPos == -1) ? "" : text.substring(breakPos + 1);
          }
        
        // Hiển thị QR code ở phần bên phải nếu có
        if (showQR && qrContent.length() > 0) {         
            displayQRCode(qrContent);
        }
        
    } while (display.nextPage());
    
    display.hibernate();
}
// 🔹 Lấy dữ liệu từ server
String fetchDataFromWebsite() {
    HTTPClient http;
    http.begin(websiteUrl);
    int httpResponseCode = http.GET();
    
    String payload = "";
    if (httpResponseCode == 200) {
        payload = http.getString();
        payload.trim();
    } else {
        payload = "Lỗi HTTP: " + String(httpResponseCode);
    }

    http.end();
    return payload;
}

void setup() {
    Serial.begin(115200);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    SPI.begin(13, -1, 14, 15);
    display.init();
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) {
        String currentData = fetchDataFromWebsite();

        if (currentData != lastFetchedData) {
            displayFormattedText(currentData);
            lastFetchedData = currentData;
            Serial.println("Dữ liệu mới: " + currentData);
        }
    } else {
        WiFi.reconnect();
    }

    delay(10000);
}
