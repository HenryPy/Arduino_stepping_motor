/* 0826_ok_2为第二套:急停和暂停未引入(没有时间写)
   相比第一套程序,优化了回原点卡顿问题,采用while true循环;
   计划急停和暂停都用中断来完成;*/
 #include "SCoop.h"
 
 //工作流程
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
   //回原点
   boolean test_bc_1;
   boolean test_bc_2;
   boolean test_bc_3;
   boolean test_bc_4;

//线程2监控两个气缸开关状态    
defineTaskLoop(Task2)
{
  men_up= digitalRead(10);     
  men_down= digitalRead(9);
  }

//线程3双上料控制    
defineTask(Task1)
void Task1::setup() {
  sta_1=1;
  sta_2=1;
  sta_3=1;
  bc_stop=0;
  bc_start=0;
  shangliao_ok=0;   
}
void Task1::loop()  {
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
  test_bc_1=0;
  test_bc_2=0;
  test_bc_3=0;
  test_bc_4=0;
  test_back_o();
}

void loop() {
  if(test_bc_4==1){
    safe_start();}
    delay(100);
}
//回原点速度1
void send_1_pluse(){
     digitalWrite(3,HIGH);
     delayMicroseconds(400); 
     digitalWrite(3,LOW);
     delayMicroseconds(400); 
  } 
//回原点速度2  
void send_2_pluse(){
     digitalWrite(3,HIGH);
     delayMicroseconds(800); 
     digitalWrite(3,LOW);
     delayMicroseconds(800); 
  } 
//回原点速度3  
void send_3_pluse(){
     digitalWrite(3,HIGH);
     delayMicroseconds(2000); 
     digitalWrite(3,LOW);
     delayMicroseconds(2000); 
  }  
//test回原点
void test_back_o(){
  /*回零程序if:1.在原点上,向前快速1发脉冲,感应到上限,改为慢速2反转,感应到原点再慢速3离开,直到离开感应器停止
           else if:2.在原点下,向前快速1发脉冲,感应到原点再慢速3离开,直到离开感应器停止
           else if:3.在原点上,慢速3离开,直到离开感应器停止
           finally:发送固定脉冲到工作位*/
    if(test_bc_1==0){
      digitalWrite(2,LOW);
      while(1){
         send_1_pluse();
         if (digitalRead(4)==LOW){
          test_bc_1=1;
          break;}
        }
      }
      if(test_bc_1==1 && test_bc_2==0){
        digitalWrite(2,HIGH);
        while(1){
         send_2_pluse();
         if (digitalRead(7)==LOW){
          test_bc_2=1;
          break;}
        }
        }
        if(test_bc_2==1 && test_bc_3==0 ){
          digitalWrite(2,LOW);
          while(1){
         send_3_pluse();
         if (digitalRead(7)==HIGH){
          test_bc_3=1;
          break;}
        }
        if(test_bc_3==1 && test_bc_4==0 ){
          digitalWrite(2,LOW);
          for(int x = 0; x < 1500 ;x++){
              digitalWrite(3,HIGH);
              delayMicroseconds(500); 
              digitalWrite(3,LOW);
              delayMicroseconds(500);           
               }
           test_bc_4=1;              
          }
          }
  
  }
     
//工作流程
/*(当在工作位时?)双感应器感应到后电机发送固定脉冲到压合位,等待0.5s,气缸压合,等待10s,气缸打开,等待1s,
   电机发固定脉冲返回工作位,循环执行
*/
void safe_start(){
  boolean site_up=0;
  baochi_third=0;
  site_up=digitalRead(4);     //上限 
   //去掉上限感应器
  if (shangliao_ok && !baochi_first && !men_up ){ 
        Serial.println("电机开始运动");
        work_pulse_on();
        baochi_first=1;
        Serial.print("电机到工作位!");
        }
    //去掉上限感应器
      if ( !baochi_second && baochi_first && !men_up && men_down ){ 
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
     //去掉上限感应器
       if( !baochi_third && baochi_second && !men_up && men_down ){
         delay(1000);
         work_pulse_off();
         baochi_third=1;
         baochi_second=0;
         baochi_first=0;
         Serial.print("已回到上料位,请上料!");
         }  
    delay(100);
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

      
  
