#include <M5StickCPlus.h>
#include "BluetoothSerial.h"
BluetoothSerial SerialBT;

String name = "BT_Slave"; // 接続名
int btn_pw = 0;           // 電源ボタン状態取得用
String data = "";         // 受信データ格納

// 初期設定
void setup() {
  M5.begin();             // 本体初期化
  Serial.begin(9600);     // シリアル通信初期化

  M5.Lcd.setTextFont(2); //文字のフォント設定
  M5.Lcd.println(name); // 接続名表示

  SerialBT.begin(name); // 接続名を指定して初期化。第2引数にtrueを書くとマスター、省略かfalseでスレーブ

  // 電源ON時のシリアルデータが無くなるまで待つ
  while (Serial.available()) {
    data = Serial.read();
  }
  M5.Lcd.setTextFont(4);  // テキストサイズ変更
  M5.Lcd.fillScreen(BLACK);  // 画面の色変更
  M5.Lcd.setTextColor(WHITE, BLACK);  // 文字の色変更
  M5.Lcd.setRotation( 3 );  // 表示させる向き調整
  M5.Lcd.setCursor(0, 1);  // 文字の開始位置調整
  M5.Lcd.setTextSize(4);  // 文字の大きさ調整
  M5.Lcd.printf("0");    //最初は0表示
}

// メイン
void loop() {
  M5.update();  // 本体のボタン状態更新
  
  if (SerialBT.available()) {               // Bluetoothデータ受信で
  
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(WHITE, BLACK);
    M5.Lcd.setRotation( 3 );
    M5.Lcd.setCursor(0, 1);
    M5.Lcd.setTextSize(4);
    
    data = SerialBT.readStringUntil('\r');  // 区切り文字の手前までデータ取得
    M5.Lcd.printf("%s", &data);                  // 液晶表示は改行あり
    Serial.print(data);                     // シリアル出力は改行なし
  }
  if (Serial.available()) {               // シリアルデータ受信で
    data = Serial.readStringUntil('\r');  // 「CR」の手前までデータ取得
    M5.Lcd.print("SR: ");                 // シリアルデータ液晶表示
    M5.Lcd.println(data);                 // 液晶表示は改行あり
    Serial.print(data);                   // シリアル出力
  }

  // 再起動処理
  if (btn_pw == 2) {          // 電源ボタン短押し（1秒以下）なら
    ESP.restart();
  }
  // 電源ボタン状態取得（1秒以下のONで「2」1秒以上で「1」すぐに「0」に戻る）
  btn_pw = M5.Axp.GetBtnPress();
  delay(20);  // 20ms遅延時間
}
