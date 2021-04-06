# UncrashableCar

 The aim of this project is to create a battery-operated toy car which moves and turns using 2 stepper motors. 
 The car is designed to recognize an obstacle in front, measure the distance, display the distance to LCD,
 when a distance is less than 8 cm. - turn on the buzzer, turn on the LED and stop the vehicle to prevent a crash. 
 When the car is forcefully stopped, the algorithm allows the car to move only in the opposite direction or turn.
 The Distance also can be measured when the car is not moving by pressing the switch on the Pulse Width Modulation module (PWM) module. 
 The car also has a servo motor which simulates windshield wipers and works by pressing the button on the breadboard.
   The algorithm starts with a logic which simulates keyless entry. A greeting message appears on the LCD display asking to type in a password number. 
   If the key is wrong, the program will continue asking for a new entry. If the entry is right, the program will exit the loop and enter the main loop.
   While in the main loop the program checks the distance switch, servo switch, state of PWM, 
   input from Serial Communication Interface and invokes a correspondent subroutine if a necessary condition is met. 
   
To control speed and the direction of motion of the car PWM is used. The input from the PWM is used to determine what direction the car should go and the speed . 
To convert the values from the input into values in range 0-250 needed for the motor a map function is used which maps input of a given range to the output of
a given range. The map function uses a double data type in calculation which should be enabled in CodeWarrior IDE before the creation of the project. 
To change motors’ direction SN 754410 quadruple half H driver is used.  
   The car can also be controlled using a keyboard. This feature is implemented using Serial Communication Interface and MiniIDE.
   When the input value equals i - car moves forward, k – backwards, j- left, l – right. In the “keyboard” mode, 
   the speed can’t be regulated and the car will move with maximum speed meaning the argument of motor() function will be 250.
   Input from the keyboard is implemented using interrupt service routine (ISR) 20.
 The distance measurement feature is implemented by ISR 7 which will measure the distance when the flag “getDist” is set to 
 1 meaning we are requesting the distance. Because the ultrasonic sensor is mounted only in front of the car, we only check the distance while moving forward.
 When the obstacle is recognized, the car stops, turns on the buzzer and LED. 

