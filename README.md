## 1 程序说明
- 板子用mega2560编写,内存用的不多,可以用Nano
- 单片机控制步进电机,
- 程序包括:开机复位+循环工作+急停+暂停
- 单片机接通电源后开始复位:向上快速找上限位,感应到后中速返回找原点,找到原点在慢速离开原点,离开后即停止
- 循环工作:双开关上料,上料1感应到2秒内上料2要收到信号,否则报警.OK后电机正向运动到压合位,气缸下压,10s后
- 气缸上升,电机反向回到上料位,循环执行


## 2 项目结构 src

### 两种上料程序,其中一种差急停和暂停没有写,开新项目了,等有时间再完善


## 3 包package
- 程序只用了一个多线程,需要导入这个包
## 4 图片
![单片机](https://user-images.githubusercontent.com/90136935/187114495-7a3c7530-99ef-401a-9704-f51957c50d09.jpg)
![电机](https://user-images.githubusercontent.com/90136935/187114502-e00c2999-d3e6-4bfb-b4c0-3660cff23ebe.jpg)
