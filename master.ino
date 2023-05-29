#include <RotaryEncoder.h>
#include <M5Core2.h>
#include "BluetoothSerial.h"
BluetoothSerial SerialBT;

String slave_name = "BT_Slave";    // スレーブの接続名
String master_name = "BT_master";  // マスターの接続名
bool connected = 0;                // 接続状態格納用
int connect_count = 3;             // 再接続実行回数
String data = "";                  // 受信データ格納用
int btn_pw = 0;                    // ボタン状態取得用

#define RE_A 0   //m5stackCore2のG0に接続
#define RE_B 26  //G26に接続

RotaryEncoder* encoder = nullptr;  //RotaryEncoderのクラス生成

// 再起動処理
void restart() {
  btn_pw = M5.BtnC.pressedFor(1000);
  if (btn_pw == 1) {  // ボタンCを1秒押したら
    ESP.restart();    // 再起動
  }
}

//初期設定
void setup() {
  M5.begin();          // 本体初期化
  Serial.begin(9600);  // シリアル通信初期化
  M5.Lcd.setTextSize(3); //テキストサイズ調整

  // Bluetooth接続
  SerialBT.begin(master_name, true);  // マスターとして初期化。trueを書くとマスター、省略かfalseを指定でスレーブ
  M5.Lcd.print("start\n.");
  Serial.print("start\n.");
  while (connected == 0) {                       // connectedが0(未接続)なら接続実行
    if (connect_count != 0) {                    // 再接続実行回数カウントが0でないなら
      connected = SerialBT.connect(slave_name);  // 接続実行
      M5.Lcd.print(".");
      connect_count--;  // 再接続実行回数カウント -1
    } else {            // 再接続実行回数カウントが0なら接続失敗
      M5.Lcd.setTextColor(RED, BLACK);
      M5.Lcd.print("\nFail");
      Serial.print("\nFail");
      while (1) { restart(); }  // 無限ループ(再起動待機)
    }
  }
  // 接続確認
  M5.Lcd.setTextColor(WHITE, BLACK); //画面の色調整
  if (connected) {                // 接続成功なら
    M5.Lcd.setTextColor(CYAN, BLACK);
    M5.Lcd.println("\nConnect");  // 「Connect」表示
    Serial.println("\nConnect");
  } else {                        // 接続失敗なら
    M5.Lcd.setTextColor(RED, BLACK);
    M5.Lcd.println("\nFail");  // 「Fail」表示
    Serial.println("\nFail");
    while (1) { restart(); }  // 無限ループ(再起動待機)
  }
  delay(1000);  // 接続結果確認画面表示時間
  // 電源ON時のシリアルデータが無くなるまで待つ
  while (Serial.available()) { data = Serial.read(); }

  M5.Lcd.fillScreen(BLACK);     // LCD表示
  M5.Lcd.setTextColor(WHITE, BLACK); // 文字の色調整
  M5.Lcd.setCursor(0, 10);           // 文字の位置調整
  M5.Lcd.setTextSize(9);             // 文字の大きさ調整
  M5.Lcd.println("\n  10000");  // 初めは10000を表示

  pinMode(RE_A, INPUT_PULLUP);  // 内蔵抵抗をプルアップ
  pinMode(RE_B, INPUT_PULLUP);

  encoder = new RotaryEncoder(RE_A, RE_B, RotaryEncoder::LatchMode::TWO03); //RotaryEncoderのクラス初期化
}
// メイン
void loop() {

  static int pos = 0;  // 一つ前の状態を格納
  M5.update();         // 本体ボタン状態更新

  encoder->tick();     // ピンの状態を確認
  M5.Lcd.setTextColor(WHITE, BLACK);

  int newPos = encoder->getPosition(); //現在のピンの位置を取得
  int sendPos = 0;                     // slaveに送信する用
  if (pos != newPos) {                 // 現在のピンに変化があれば
    if ((newPos % 5) == 0) {           // 5回に1度の変化で実行
      if (newPos >= 0) {               //ピンの状態が正なら
        M5.Lcd.fillScreen(BLACK);
        M5.Lcd.setCursor(0, 10);
        M5.Lcd.setTextSize(9);
        M5.Lcd.printf("\n  %d\n", 10000 - ((newPos / 5) * 100)); // 10000から100ずつ減らして画像表示

        Serial.print(newPos);
        char str[10];
        sendPos = (newPos / 5) * 100; // 送信する用を100ずつ増やす
        sprintf(str, "%d", sendPos);
        SerialBT.print(str);          // slaveに送信
        pos = newPos;                 // 一つ前の状態を更新
      }
    }
  }
  restart();           // 再起動処理
  delay(20);           // 20ms遅延
}