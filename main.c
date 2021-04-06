
#include <hidef.h>      /* common defines and macros */
#include <mc9s12dg256.h>     /* derivative information */
#pragma LINK_INFO DERIVATIVE "mc9s12dg256b"
#include "main_asm.h" /* interface to the assembly module */

#define TRIG 1                  // Trigger for us sensor 
#define ECHO 2                  // Echo for us sensor 
#define SWDIST 8                //switch for distance  (PORTE)
#define SWWIPER 4               //switch for wipers   (PORTE)
#define RIGHT_MOT_D 32          // Bitmask for motors( PORTB)
#define RIGHT_MOT_R 16
#define LEFT_MOT_D 64
#define LEFT_MOT_R 128
#define FORWARD 96              //Drive,  LEFT_MOT_D + RIGHT_MOT_D
#define BACKWARD 144            //Reverse,  LEFT_MOT_R + RIGHT_MOT_R
#define LED 8
#define HORN 4
                 
int passWord=7;                
int keyInput;
int isLocked=1;
int yAxes;                      //data from the wheel 
int xAxes;
int motorspeed;
void stop();
void servoOn();
int distance;                   //distance to an obstacle 
int getDist;                    //flag for obtaining distance 
int echoCount;
char inval;                     // input from keyboard 

  void interrupt 7 handler(){   // distance measurement routine 
 
    if(getDist==1){
      
      PORTB |= TRIG;            //send the trigger 
      us_delay(15);         
      PORTB=PORTB&~TRIG;        //stop sending the trigger 
      while (!(PORTB&ECHO));    //wait for echo to return 
      while (PORTB&ECHO){                        
      echoCount ++;             // increment echoCount when echo is returned  
    }    
    distance = echoCount/285;   // scale and assign the distance 
    echoCount=0; 
    getDist=0;   
    }    
    clear_RTI_flag();        
  }
                                //interrupt for SCI port
  void interrupt 20 SCI0_event() {     
    inval = SCI0DRL;
  }
  
  void main(void) {
 
  DDRB=253;                     // all but bit 1 (Echo) for output        
  PLL_init();
  keypad_enable(); 
  lcd_init();
  servo76_init();
  set_servo76(0);
  ad0_enable(); 
  SCI0_init(9600);
  RTI_init(); 




  
    while(1){                   // main loop
         
      while(isLocked){          // car locked loop 
      
        set_lcd_addr(0x00);
        type_lcd ("Hello, enter");
        set_lcd_addr(0x43);
        type_lcd("your password");
        clear_lcd;
        keyInput= getkey();     // receive input from keyboard
                          
        if(keyInput==passWord){ 
          isLocked=0;            // exit loop when password is right 
          clear_lcd();                
        }else{                     // continue looping and requesting  the password 
          set_lcd_addr(0x00);
          type_lcd ("Wrong password  ");
          set_lcd_addr(0x40);  
          clear_lcd();
          ms_delay(200);    
        }
      }  
      set_lcd_addr(0);          // Car is unlocked and ready to go 
      type_lcd ("Go!");
      ms_delay(500);       
                                // loop when car is unlocked
      while(isLocked==0){
      
        PORTB=PORTB&~LED;       // turn off led/horn
        PORTB=PORTB&~HORN;       
        
        yAxes=ad0conv(2);       // get data from the wheel 
        xAxes=ad0conv(7);
        motor1_init(); 
 
     
        while(SCI0SR1&0x20) {   // if sci sends data
          set_lcd_addr(0x40);   // display data   
          type_lcd( "Inval:");
          set_lcd_addr(0x46);  
          inval = inchar0(); 
          data8(inval);
                                // depending on the  input, invoke corespondent methods 
          if(inval=='i'){
            drive(250);
          }
          if(inval=='k'){
            reverse(250);
          }
          if(inval=='j'){
            left(250) ;
          }
          if(inval=='l'){
            right(250);
          } 
        }
              
       
                                 // if distance switch is pressed
        if(!(PORTE&SWDIST)){     // set flag to let the interrupt know 
            getDist=1;           //that we are requesting   distance
            set_lcd_addr(0x40); 
            type_lcd("Distance: ");
            set_lcd_addr(0x49);       
            write_int_lcd(distance);// display the distance  
            ms_delay(1000); 
        }

        if(!(PORTE&SWWIPER)){    // if wiper switch is switched 
           servoOn();            // turn on servo
        }
        
    
        if(yAxes<500){          // reverse 
          reverse(yAxes);  
                 
        }
        if(yAxes>525){          //forward 
   
          getDist=1;            // display obstacle if it's within 8 cm 
          if((distance<8)&(distance>1)&(distance<100) ){
            PORTB=PORTB|LED;    // turn on horn/ led 
            PORTB=PORTB|HORN;
            PORTB &=~FORWARD;   // stop the car
            set_lcd_addr(0);
            type_lcd("Obstacle        ");
            ms_delay(200);
          }else{                // if dist. > 8 cm - drive 
            drive(yAxes);
            set_lcd_addr(0x40); 
            type_lcd("Distance: "); // display distance 
            set_lcd_addr(0x49);
            write_int_lcd(distance);
          }
        }
          
        if(xAxes>550){           // turn right 
         right(xAxes);
       
         
        }if(xAxes<500){          // turn left
          left(xAxes);
         
        }
         if((yAxes>500)&&(yAxes<550)&&(xAxes<600)&&(xAxes>500)){
             stop();             //  stop the car 
        }        
      }             
    }
  }
  int drive(int yAxes) {
                                 // use map function to assign motor speed value (0,250)
       motorspeed=map(yAxes,546,1024,40,250);
       set_lcd_addr(0);
       type_lcd ("D");
       set_lcd_addr(0x3);       
       motor1(motorspeed);
       type_lcd ("Speed: ");
       set_lcd_addr(0x9);    
       write_int_lcd(motorspeed);
       PORTB|=FORWARD;           // 
 
  }
  int reverse(int yAxes){
                                  // use map function to assign motor speed value (0,250)
       motorspeed=map(yAxes,540,0,40,250);
       set_lcd_addr(0);
       type_lcd ("R");            
       set_lcd_addr(0x3);       
       motor1(motorspeed);
       type_lcd ("Speed: ");
       set_lcd_addr(0x9);    
       write_int_lcd(motorspeed);
       PORTB|=BACKWARD;
  }
  int right(int xAxes){
                                  
       motorspeed=map(xAxes,545,1023,40,250);
       set_lcd_addr(0);
       type_lcd ("Right");    
       motor1(motorspeed);
       PORTB|=LEFT_MOT_D;          
    
  }
  int left(int xAxes){
                                  
       motorspeed=map(xAxes,545,0,40,250);
       set_lcd_addr(0);
       type_lcd ("Left");      
       motor1(motorspeed);
       PORTB|=RIGHT_MOT_D;
    
  }
  void stop(){
  
       clear_lcd();
       set_lcd_addr(0);
       type_lcd ("N");
       PORTB &=~FORWARD;
       PORTB &=~BACKWARD;
       motor1(0);
  }
                                   // map input of a given range to an output of a given range 
  int map(int input,int input_start, int input_end, int output_start, int output_end) {
    
    double slope;                  // constand(scale) for given numbers 
    slope  = 1.0* (output_end - output_start) / (input_end - input_start);      
    return output_start + slope * (input - input_start)
 
  void servoOn(){
  
    int width;                     // width of the pulse 

    for (width=4500;width<=6000;width=width+10){
      set_servo76(width);
      ms_delay(1);   
    }
    for (width=6000;width>=3000;width=width-10){
      set_servo76(width);
      ms_delay(1); 
    }
    for (width=3000;width<4500;width=width+10){
      set_servo76(width);
      ms_delay(1);       
    }    
  }
  
  
