## BOXduino - the first Arduino powered e-cigarette!

Using Arduino to take care about Your e-cig

Currently working:
  * Sleep / Power on
  * Firing
  * Read input voltage
  * Very simple control
  * Battery indicator and ohmmeter (pretty accurate)
  * (Almost) perfect scheme in Fritzing  
  * Nice display!  

Currently unavaiable:
  * Power saving: now in idle mode it's taking 150mAh which is really bad for me (ohmmeter circuit)  
  * Backlight
  * Dynamic refreshing (printing stuff takes A LOT of time and Arduino can't comply updating screen and button data as fast as I need)  

Controls:  
* Refresh everything: 1x click
* Fire: Hold the button
* Switch VV / Bypass: 3x click  
* Night mode (inverting display): 4x click
* Power off: 5x click  
* Power on: Hold the button (while off)  
* Serial debug: 6x click
  
Check Issues for ToDo list and feel free to add new suggestions!  
