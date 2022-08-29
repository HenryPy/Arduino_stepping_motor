/* 0826_ok_1为第一套:
   线程1计时,线程2读取上下气缸状态;新建的标志位如果是布尔则为1
   目前复位+工作程序已经合并,运行OK,双上料程序OK,上料防呆OK,
   急停属于物理控制5V电源，暂停电动按钮控制完成，未引入中断！！*/
 #include "SCoop.h"
 boolean baochi_shangxi;      //上限保持
 boolean baochi_yuandi;       //原点保持
 boolean fuwei_wancheng;      //复位完成
 boolean workdian_wanch;      //工作点完成
 boolean baochi_first;        //程序保持  
 boolean baochi_second;       //程序保持 
 boolean baochi_third;        //程序保持
 boolean baochi_forth;        //程序保持
 boolean men_up;              //气缸上升到位
 boolean men_down;            //气缸下降到位
 //双上料参数
  boolean sta_1; 
  boolean sta_2;
  boolean sta_3;
  boolean shangliao_ok;          //上料条件OK
  unsigned long  time_start;     //上料1时间
  unsigned long  time_stop;      //上料2时间
  unsigned long  bc_start;       //上料1保持时间
  unsigned long  bc_stop;        //上料2保持时间
  unsigned long  time_chazhi;    //双上料延迟
  //暂停开关
  boolean pause_button;
  boolean bc_pause;           //暂停点动按钮
  boolean old_bc;             //按钮按下抬起的一瞬间被扫描了多次，所以引入保持
//线程1记录复位完成状态
defineTask(Task1)
void Task1::setup() {
    fuwei_wancheng=0;
    workdian_wanch=0;
}
void Task1::loop()  {
  boolean yuandian  = digitalRead(7); 
  if(!yuandian && !fuwei_wancheng ){
     delay(5000);
     fuwei_wancheng=1;}
  if (fuwei_wancheng && !workdian_wanch){
    Serial.print("复位完成!");
     digitalWrite(2,LOW);
    for(int x = 0; x < 1500 ;x++){
     digitalWrite(3,HIGH);
     delayMicroseconds(800); 
     digitalWrite(3,LOW);
     delayMicroseconds(800);           
      } workdian_wanch=1;
      Serial.print("已到上料位!");
     }
    }
    
//线程2监控两个气缸开关状态    
defineTaskLoop(Task2)
{
  men_up= digitalRead(10);     
  men_down= digitalRead(9);
  }
  
//线程3双上料控制    
defineTask(Task3)
void Task3::setup() {
  sta_1=1;
  sta_2=1;
  sta_3=1;
  bc_stop=0;
  bc_start=0;
  shangliao_ok=0;   
}
void Task3::loop()  {
  boolean sta_shangli1=digitalRead(0);
  boolean sta_shangli2=digitalRead(8);
  if(time_start == bc_start){sta_3=1;} //如果当前时间和
  if(!sta_shangli1){                   //如果上料1亮
    time_start = millis();             //记录当前时间
    if(sta_1){                         //如果本步可以执行
      bc_start=time_start;             //将上料1的时间记录下来
      sta_1=0;                         //置为0->本步不在执行
      }
     }
  if(!sta_shangli2){                   //如果上料2亮
    time_stop = millis();              //记录当前时间
    if(sta_2){                         //如果为1
    bc_stop=time_stop;                 //将上料2的时间记录下来
    sta_2=0;                           //置为0->本步不在执行
    }
  }
  if(bc_stop > bc_start){              //为保证时间为正数,进行转换
  time_chazhi =  bc_stop - bc_start;}else{
  time_chazhi =  bc_start - bc_stop;}
 // 如果时间小于2s,且本步未执行过(保证本步执行一次)
if (5<time_chazhi && time_chazhi<2000 && bc_stop!=0 && bc_start!=0 && sta_3 && !sta_shangli1 && !sta_shangli2){
  Serial.println("触发上料OK!");
  shangliao_ok = 1;
  delay(1000);
  shangliao_ok = 0;
  sta_1=1;
  sta_2=1;
  sta_3=0;
  
  }
  //情况1:只有一个开关亮
if (5<time_chazhi && time_chazhi<2000 && bc_stop!=0 && bc_start!=0 && sta_3  && !sta_shangli2){
  Serial.println("情况1NG!");
  shangliao_ok = 0;
  delay(1000);
  sta_1=1;
  sta_2=1;
  sta_3=0;
  
  } 
//情况2:只有一个开关亮  
if (5<time_chazhi && time_chazhi<2000 && bc_stop!=0 && bc_start!=0 && sta_3 && !sta_shangli1 ){
  Serial.println("情况2NG!");
  shangliao_ok = 0;
  delay(1000);
  sta_1=1;
  sta_2=1;
  sta_3=0;
  
  }   
//如果时间大于2s,且本部未执行过  
if (time_chazhi>2000 && bc_stop!=0 && bc_start!=0 && sta_3 && !sta_shangli1 && !sta_shangli2){
  Serial.println("超时1NG!");
  shangliao_ok = 0;
  delay(1000);
  sta_1=1;
  sta_2=1;
  sta_3=0;
  }
  }
//线程4暂停
defineTaskLoop(Task4){
  pause_button = digitalRead(11);
  if(pause_button==1 && old_bc==0){
    bc_pause=1 - bc_pause;
    delay(10);//加延时去杂波
    }
    //按钮按下未松开时，old_bc永远为1，只有当松开后才为0
    old_bc = pause_button;
  }
   
//开关亮为0,不亮是1 ,开关HIGH为1,LOW为0
void setup() {
  Serial.begin(9600);
  Serial.print("串口打开成功!");
  mySCoop.start();
  pinMode(3,OUTPUT);       //方向
  pinMode(2, OUTPUT);      //脉冲
  pinMode(5, OUTPUT);      //气缸升
  pinMode(6, OUTPUT);      //气缸降
  pinMode(0, INPUT);       //上料1
  pinMode(1, INPUT);       //上料2
  pinMode(4, INPUT);       //运动上限输入
  pinMode(7, INPUT);       //电机原点输入
  pinMode(8, INPUT);       //运动下限输入
  pinMode(9, INPUT);       //气缸上升到位输入
  pinMode(10,INPUT);       //气缸下降到位输入
  pinMode(11,INPUT);       //暂停开关输入
  digitalWrite(6,HIGH);
  digitalWrite(5,LOW);
  baochi_shangxi=0;
  baochi_yuandi=0;
  fuwei_wancheng=0;
  workdian_wanch=0;
  baochi_first=0;             //程序保持  
  baochi_second=0;            //程序保持 
  baochi_third=0;             //程序保持
  baochi_forth=0;             //程序保持
  bc_pause=0;                 //暂停保持
  old_bc=0;
  
}

void loop() { 
  if(!workdian_wanch){
    re_back_on();}
  else {
    safe_start();} 
  delay(100);
}

//回原点       
void re_back_on(){
  /*回零程序if:1.在原点上,向前快速1发脉冲,感应到上限,改为慢速2反转,感应到原点再慢速3离开,直到离开感应器停止
           else if:2.在原点下,向前快速1发脉冲,感应到原点再慢速3离开,直到离开感应器停止
           else if:3.在原点上,慢速3离开,直到离开感应器停止
           finally:发送固定脉冲到工作位
  */
    boolean yuandian;
    boolean shangxian;
    shangxian = digitalRead(4);    //上限输入
    yuandian  = digitalRead(7);    //原点输入 
    if(shangxian == 0){
      baochi_shangxi=1;}
    if(yuandian == 0) {
      baochi_yuandi=1;
      }
      if(!men_up && !bc_pause ){
      if(!baochi_shangxi){     //如果上限没有亮过     
        if(!baochi_yuandi){    //如果上限没有亮过->如果原点也没有亮
        digitalWrite(2,LOW);
        fast_pulse();           
       }else{                   //如果上限没有亮过->如果原点亮过
         if(!yuandian){         //如果上限没有亮过->如果原点亮过->如果正在亮
        digitalWrite(2,LOW);
        low_pulse();
         }
        }
       } else{                  //如果上限亮过
       if(!baochi_yuandi ){     //如果上限亮过->如果原点没亮过
        digitalWrite(2,HIGH);
        fast_pulse();
        } else{                //如果上限亮过->如果原点亮过
       if(!yuandian){          //如果上限亮过->如果原点亮过->如果正在亮
        digitalWrite(2,LOW);
        low_pulse();
          }
        }
      }
     } 
    }
    
void fast_pulse(){
    for(int x = 0; x < 100 ;x++){
     digitalWrite(3,HIGH);
     delayMicroseconds(1000); 
     digitalWrite(3,LOW);
     delayMicroseconds(1000);           
     }     
  } 
//慢速回原点 
void low_pulse(){
  for(int x = 0; x < 100 ;x++){
     digitalWrite(3,HIGH);
     delayMicroseconds(2000); 
     digitalWrite(3,LOW);
     delayMicroseconds(2000);           
     }  
}
//工作流程
/*(当在工作位时?)双感应器感应到后电机发送固定脉冲到压合位,等待0.5s,气缸压合,等待10s,气缸打开,等待1s,
   电机发固定脉冲返回工作位,循环执行
*/
void safe_start(){
  boolean sta_up=0;           //双上料感应保持 
  boolean site_up=0;
  boolean sta_1 =0;
  boolean sta_2 =0;
  baochi_third=0;
  sta_1 =digitalRead(0);
  sta_2 =digitalRead(1);
  site_up=digitalRead(4);     //上限 
  // 如果上料触发一次,且上限未亮(1),门是开的,(本步)未执行
 // if (shangliao_ok  && site_up && !baochi_first && !men_up){
  //去掉上限感应器
  if (shangliao_ok && !baochi_first && !men_up && !bc_pause ){ 
        Serial.println("电机开始运动");
        work_pulse_on();
        baochi_first=1;
        Serial.print("电机到工作位!");
        }
    //如果上限是亮的(0),(本步)未执行,(上一步)执行完     
    //if (!site_up && !baochi_second && baochi_first && !men_up && men_down){ 
    //去掉上限感应器
      if ( !baochi_second && baochi_first && !men_up && men_down && !bc_pause){ 
         delay(500);
         digitalWrite(6,LOW);
         digitalWrite(5,HIGH);
         Serial.print("气缸压合完成!");
         delay(10000);
         digitalWrite(6,HIGH);
         digitalWrite(5,LOW);
         baochi_second=1;
         baochi_first=0;
         Serial.print("气缸打开完成!");
           }
   //如果上限是亮的(0),门是开的,(本步)未执行,(上一步)执行完   
     //if(!site_up && !baochi_third && baochi_second && !men_up && men_down){
   //去掉上限感应器
       if( !baochi_third && baochi_second && !men_up && men_down && !bc_pause){
         delay(1000);
         work_pulse_off();
         baochi_third=1;
         baochi_second=0;
         baochi_first=0;
         Serial.print("已回到上料位,请上料!");
         }  
    delay(100);
}
//开门
void close_door(){
  boolean men_up = digitalRead(10);    //气缸上升到位
  boolean men_down = digitalRead(9);   //气缸下降到位
  boolean sta_up_bc=0;
  if(!men_up){sta_up_bc=1;}
  if(sta_up_bc && men_down ){
  digitalWrite(6,LOW);
  digitalWrite(5,HIGH);
  sta_up_bc=0;}
  } 
//关门
void open_door(){
  boolean men_up = digitalRead(10);    //气缸上升到位
  boolean men_down = digitalRead(9);   //气缸下降到位
  boolean sta_down_bc=0;
  if(!men_down){sta_down_bc=1;}
  if(sta_down_bc && men_up){
  digitalWrite(5,LOW);
  digitalWrite(6,HIGH);
  sta_down_bc=0;}
  } 
    
//工作脉冲正转    
void work_pulse_on(){
  digitalWrite(2,LOW);
  for(int x = 0; x < 5000 ;x++){
     digitalWrite(3,HIGH);
     delayMicroseconds(500); 
     digitalWrite(3,LOW);
     delayMicroseconds(500);           
     }  
  }
//工作脉冲反转   
void work_pulse_off(){
  digitalWrite(2,HIGH);
  for(int x = 0; x < 5000 ;x++){
     digitalWrite(3,HIGH);
     delayMicroseconds(500); 
     digitalWrite(3,LOW);
     delayMicroseconds(500);           
     } 
  }
//复位完成到工作点    
void work_point_to(){
  digitalWrite(2,HIGH);
  for(int x = 0; x < 500 ;x++){
     digitalWrite(3,HIGH);
     delayMicroseconds(800); 
     digitalWrite(3,LOW);
     delayMicroseconds(800);           
     }  
  } 
      
  
