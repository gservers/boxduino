# BOXduino - the first Arduino powered e-cigarette!  

Let me introduce You to one of the first almost open source Arduino BOX MOD - BOXduino!  
It took much work, and I'm still not finished! Time to solder this up, because breadboard tests are promising.

###Currently working:
  * Ohmmeter!  
  * Sleep / Power on - makes BOXduino more powersaving!  
  * Firing - It's self-explainatory! 
  * Read input voltage - Watch battery level!  
  * Battery indicator (pretty accurate) - with awesome bettery graphics!  
  * Nice display! - Pretty fine and readable layout  
  * Screen dimming! - afer 10 seconds without any action the screen will be dimmed.  
  * Standard control - see controls below  
  * Screen refreshing! - RMS voltage and resistance, the only thing that should be  
  * 1:1 Scheme - just solder it up! The only changes will be in code! (i hope)
  * Statistics submenu - check how low You can get, check puffs and mode

###Controls:  
  * Refresh everything: 1x click  
  * Fire: Hold the button  
  
  * Switch VV / Bypass: 3x click  
  * Night mode (inverting display): 4x click 
  
  * Power off: 5x click  
  * Power on: Hold the button (while off)  
  
  * Serial debug: 6x click  
  * Wakeup the screen: 1x click  
  
  * Change resistance (potentiometer on 0 and manual = true): 2x click
  * Check puffs, minimal resistance and max wattage: 2x click
  
###Ohmmeter
LM317 is taking over 125mA all the time, which makes me cry. Why would I waste my batteries for such thing?  
So i decided to use S8050 bipolar NPN transistor (up to 0.5A!) to supply power.
Now LM317 is powered just when I need to.

There are two modes available - manual and automatic.  
To change them edit bool called "manual".  

(false) Automatic is pretty easy to explain - ohmmeter circuit (and code) is calculating resistance almost all the time.    
(true) Manual - if You can't/don't want to add LM317 circuit, the resistance is set by yourself.  

You must know the resistance of the coil, but before I add TC it's just to show watts.  
Device will automatically ask for resistance, just turn potentiometer to desired value and press fire.  
To change, just press fire two times with potentiometer on 0.  


I didn't get any feedback, but if You find something hard to understand just pm me.  
##Check Issues for ToDo list and feel free to add new suggestions!  