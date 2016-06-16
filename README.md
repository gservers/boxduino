## BOXduino - the first Arduino powered e-cigarette!  

Scheme is OUT OF DATE, couple of things were changed!  

##Suprise!  
Ohmmeter is now available!  
Thanks to bipolar transistor and some circuit changes it's now possible!  
There are two modes available - manual and automatic.
To change them edit bool called "manual". 
(false) Automatic is pretty easy to explain - ohmmeter circuit (and code) calculates the resistance by itself.  
(true) Manual - if You can't/don't want to add LM317 circuit, the resistance is set by yourself.  
You must know the resistance of the coil, but before I add TC it's just to show watts.  
Device will automatically ask for resistance, just turn potentiometer to desired value and press fire.  
To change, just press fire two times.  
Easy and clear, isn't it?

Currently working:
  * Ohmmeter!  
  * Sleep / Power on - makes BOXduino more powersaving!  
  * Firing - It's self-explainatory! 
  * Read input voltage - Watch battery level!  
  * Battery indicator (pretty accurate) - with awesome bettery graphics!  
  * Nice display! - Pretty fine and readable layout  
  * Screen dimming! - afer 10 seconds without any action the screen will be dimmed.  
  * Standard control - see controls below  
  * Screen refreshing! - RMS voltage and resistance, the only thing that should be  

Currently unavaiable:
  * Backlight data as fast as I need)  
  * Scheme  

Controls:  
  * Refresh everything: 1x click  
  * Fire: Hold the button  
  * Switch VV / Bypass: 3x click  
  * Night mode (inverting display): 4x click  
  * Power off: 5x click  
  * Power on: Hold the button (while off)  
  * Serial debug: 6x click  
  * Wakeup the screen: 1x click   
  
I didn't get any feedback, but if You find something hard to understand just pm me.  
##Check Issues for ToDo list and feel free to add new suggestions!  
