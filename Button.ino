#include <avr/sleep.h>
#include <avr/wdt.h>
#include <Stepper_28BYJ_48.h>

#define ZERO B1111110
#define ONE B0110000
#define TWO B1101101
#define THREE B1111001
#define FOUR B0110011
#define FIVE B1011011
#define SIX B1011111
#define SEVEN B1110010
#define EIGHT B1111111
#define NINE B1111011

Stepper_28BYJ_48 stepper(16,17,18,19); //モーターのピン設定

/***************************************/
/** 変数・定数定義                     **/  
/***************************************/
const int Button_hour_Pin=12;   // 12ピン:時間セットボタン
const int Button_minute_Pin=13; // 13ピン:分セットボタン
const int Button_do_Pin=14;     // 14ピン:実行ボタン
volatile int wdt_cycle = 0;
int Button_hour_pre_state=LOW;
int Button_minute_pre_state=LOW;
int Number[10]={ZERO,ONE,TWO,THREE,FOUR,FIVE,SIX,SEVEN,EIGHT,NINE};
int time_hour=0;
int time_minute=0;
int time_minute_1=0;
int time_minute_2=0;
unsigned long time_start;
unsigned long time_now;
unsigned long time_left;
unsigned long time_motor_start;
unsigned long time_motor_now;
int time_set;
int time_motor_set=10;
bool do_state=false; //実行中か否か 実行中:true 実行していない:false
int sleep_10_count;

unsigned long test;

/***********************************/
/** 初期化                         **/
/** ピン設定・セグメント桁の表示切替 **/
/***********************************/
void setup(){
  Serial.begin(9600); 
  //pinの設定
  pinMode(Button_hour_Pin,INPUT);   // pin12: 時間ボタン用INPUT
  pinMode(Button_minute_Pin,INPUT); // pin13: 分ボタン用INPUT
  pinMode(Button_do_Pin,INPUT);     // pin14: 実行ボタン用INPUT
  pinMode(15,OUTPUT);               // pin15: 実行中表示LED
  for(int i=2;i<=11;i++){           // pin2~11: 4桁7セグ用OUTPUT
    pinMode(i,OUTPUT);
  }
  //4桁7セグの1,2,4桁目の表示ON
  for(int i=9;i<=11;i++){  //1桁目:9,2桁目:10,4桁目:11
    digitalWrite(i,HIGH);   
  }
 }

void loop(){
  if(do_state==false){ //実行中はカウントされない
    digitalWrite(15,HIGH);
    //ボタンが押されるとカウント
    //時間用
    if(digitalRead(Button_hour_Pin)==HIGH){
      if(Button_hour_pre_state==LOW){
        time_hour++;
        if(time_hour>=10) time_hour=0;
      }
      Button_hour_pre_state=HIGH;
    }else{
      Button_hour_pre_state=LOW;
    }
    //分用
    if(digitalRead(Button_minute_Pin)==HIGH){
      if(Button_minute_pre_state==LOW){
        //time_minute+=10;
        time_minute+=1;
        if(time_minute>=60){ 
          time_minute=0;
        }
        time_minute_2=time_minute/10;    // 分の2桁目をtime_minute_2に代入
        time_minute_1=time_minute%10;    // 分の1桁目をtime_minute_1に代入
      }
      Button_minute_pre_state=HIGH;
    }else{
      Button_minute_pre_state=LOW;
    }
  }

  //4桁7セグ表示
  Seg_Display(time_hour,11);   //時間(4桁目)表示
  Seg_Display(time_minute_2,10); //分(2桁目)表示
  Seg_Display(time_minute_1,9);  //分(1桁目)表示

  //実行ボタンが押されたか否か
  if(digitalRead(Button_do_Pin)==HIGH){
    digitalWrite(15,LOW);
    time_set=time_hour*60+time_minute;  // 設定時間を分で算出
    //タイマー実行
    do_state=true;
    time_start=millis();
    time_now=0;
    time_hour=time_set/60;
    time_minute_2=(time_set%60)/10;
    time_minute_1=(time_set%60)%10;
    //テスト用プログラム
    do{
      time_now=(millis()-time_start)/60000;
      time_hour=(time_set-time_now)/60
      time_minute_2=((time_set-time_now)%60)/10;
      time_minute_1=((time_set-time_now)%60)%10;
      Seg_Display(time_hour,11);     // 時間(4桁目)表示
      Seg_Display(time_minute_2,10); // 分(2桁目)表示
      Seg_Display(time_minute_1,9);  // 分(1桁目)表示
    }while(time_set>time_now);
    /*
    do{
      time_now=(millis()-time_start)/60000;
      Seg_Display(time_hour,11);     // 時間(4桁目)表示
      Seg_Display(time_minute_2,10); // 分(2桁目)表示
      Seg_Display(time_minute_1,9);  // 分(1桁目)表示
    }while(time_now<1);
    sleep_10_count=(time_left-4)/10; // 10分間のスリープ回数
    for(int i=0;i<45;i++){           // 6分間のスリープ
      delayWDT(9);  // 9(8秒間)を45回
    }
    for(int i=0;i<sleep_10_count;i++){ // 10分間のスリープ
      for(int j=0;j<75;j++){
        delayWDT(9);
      }
    }
    time_start=millis();
    time_hour=0;
    time_minute_2=0;
    do{
      time_now=(millis()-time_start)/60000;
      time_minute_1=3-time_now;
      Seg_Display(time_hour,11);     // 時間(4桁目)表示
      Seg_Display(time_minute_2,10); // 分(2桁目)表示
      Seg_Display(time_minute_1,9);  // 分(1桁目)表示
    }while(time_now<2);
    do{
      time_now=(millis()-time_start)/1000; //経過時間を秒(s)で表示
      time_minute_2=(180-time_now)/10;
      time_minute_1=(180-time_now)%10;
      Seg_Display(time_hour,11);     // 時間(4桁目)表示
      Seg_Display(time_minute_2,10); // 分(2桁目)表示
      Seg_Display(time_minute_1,9);  // 分(1桁目)表示
    }while(time_now<180);
    */
    //タイマー終了
    /*
      do{  //【※Sleep関数の入る余地あり※】
        time_now=((millis()-time_start)/60000); //経過時間(分)を計算
        time_left=time_set-time_now;            //残り時間(分)を算出
        time_hour=time_left/60;
        time_minute_2=(time_left%60)/10;
        time_minute_1=(time_left%60)%10;
        Serial.print("time_minute_1=");
        Serial.print(time_minute_1);
        Serial.println("");
        //4桁7セグ表示
        Seg_Display(time_hour,11);   //時間(4桁目)表示
        Seg_Display(time_minute_2,10); //分(2桁目)表示
        Seg_Display(time_minute_1,9);  //分(1桁目)表示
      }while(time_set>time_now);
    */
    //タイマー終了
    for(int i=2;i<=11;i++){ //4桁7セグ表示を停止
      digitalWrite(i,LOW);
    }
    time_motor_start=millis(); //モーター制御は秒単位で管理
    do{
      time_motor_now=(millis()-time_motor_start)/1000;
      if(time_motor_set>time_motor_now){ //一定時間モーターを回転させ、その後に同じ時間だけ反回転させる
        stepper.step(1);
      }else{
        stepper.step(-1);
      }
    }while(time_motor_now<(time_motor_set*2));
    stepper.step(0);
    time_hour=0;
    time_minute=0;
    time_minute_2=0;
    time_minute_1=0;
    do_state=false;
    //4桁7セグの1,2,4桁目の表示ON
    for(int i=9;i<=11;i++){  //1桁目:9,2桁目:10,4桁目:11
      digitalWrite(i,HIGH); 
    }
    //実行終了
  }
}

/****************************/
/** 4桁7セグメント表示関数   **/
/** (数字,桁目のピン番号)   **/
/****************************/
void Seg_Display(int time_num,int Pin){
  int Bit_Number=Number[time_num];
  digitalWrite(Pin,LOW);
  for(int i=2;i<=8;i++){
    digitalWrite(i,bitRead(Bit_Number,i-2));
  }
  delay(5);
  digitalWrite(Pin,HIGH);
}

/******************/
/** スリープ関数  **/
/******************/
void delayWDT(unsigned long t) {        // パワーダウンモードでdelayを実行
  delayWDT_setup(t);                    // ウォッチドッグタイマー割り込み条件設定
  ADCSRA &= ~(1 << ADEN);               // ADENビットをクリアしてADCを停止（120μA節約）
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);  // パワーダウンモード(最も省エネ)
  sleep_enable();

  sleep_mode();                         // ここでスリープに入る

  sleep_disable();                      // WDTがタイムアップでここから動作再開 
  ADCSRA |= (1 << ADEN);                // ADCの電源をON (|=が!=になっていたバグを修正2014/11/17)

}

/************************/
/** スリープ関数初期設定 **/
/************************/
void delayWDT_setup(unsigned int ii) {  // ウォッチドッグタイマーをセット。
  // 引数はWDTCSRにセットするWDP0-WDP3の値。設定値と動作時間は概略下記
  // 0=16ms, 1=32ms, 2=64ms, 3=128ms, 4=250ms, 5=500ms
  // 6=1sec, 7=2sec, 8=4sec, 9=8sec
  byte bb;
  if (ii > 9 ){                         // 変な値を排除
    ii = 9;
  }
  bb =ii & 7;                           // 下位3ビットをbbに
  if (ii > 7){                          // 7以上（7.8,9）なら
    bb |= (1 << 5);                     // bbの5ビット目(WDP3)を1にする
  }
  bb |= ( 1 << WDCE );

  MCUSR &= ~(1 << WDRF);                // MCU Status Reg. Watchdog Reset Flag ->0
  // start timed sequence
  WDTCSR |= (1 << WDCE) | (1<<WDE);     // ウォッチドッグ変更許可（WDCEは4サイクルで自動リセット）
  // set new watchdog timeout value
  WDTCSR = bb;                          // 制御レジスタを設定
  WDTCSR |= _BV(WDIE);
} 

/***************************************/
/** タイマー処理 **/
/*****************/
/*void timer_do_process(){
  millis
}
*/
/******************************************/
/** スリープ関数が終了する度に実行される処理 **/
/******************************************/
ISR(WDT_vect) {                       // WDTがタイムアップした時に実行される処理
  wdt_cycle++;                        // 必要ならコメントアウトを外す
}