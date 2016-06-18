#BOXduino libraries file tree:  
libraries\  
	 PCD8544\
		- charset.cpp  
		- LICENSE.txt  
		- library.properties  
		- PCD8544.cpp  
		- PCD8544.h  
	 ClickButton\  
		- ClickButton.cpp
		- ClickButton.h
	 Prescaler\  
		- Prescaler.h  
There is also folder orig_backup whick contains original libraries without modifications.  

##PCD8544 manual

To use this library, you must first connect your LCD to the proper pins on the Arduino.
For a Nokia 3310 display the connections would be the following:

Display Pin       | Arduino Pin
------------------|------------
Pin 1             | +3.3V Pin
Pin 2 (GND)       | GND
Pin 3 (SCE)       | Digital Pin 7
Pin 4 (RESET)     | Digital Pin 8
Pin 5 (D/C)       | Digital Pin 5
Pin 6 (DN/MOSI)   | Digital Pin 4
Pin 7 (SCLK)      | Digital Pin 3
Pin 8 (backlight) | Wherever You want

Every pin should get 3.3 volts. Connect them through transistors, level converter or whatever you like.

Now, take a moment and read through the included [`HelloWorld.ino`](examples/HelloWorld/HelloWorld.ino) example.
It shows how to use the basic features of the library. There is also another
[`Thermometer.ino`](examples/Thermometer/Thermometer.ino) example that demonstrates bitmapped graphics and charts.

The library allows the use of custom bitmap symbols (5x8), defined by an array of five bytes.
To make it easy to create custom symbols, there's a graphical glyph editor
[available online](http://cloud.carlos-rodrigues.com/projects/pcd8544/).  

###This isn't my library!
I'm using it on following license:  

>The MIT License (MIT)
>Copyright (c) 2013 Carlos Rodrigues <cefrodrigues@gmail.com>
>
>Permission is hereby granted, free of charge, to any person obtaining a copy
>of this software and associated documentation files (the "Software"), to deal
>in the Software without restriction, including without limitation the rights
>to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
>copies of the Software, and to permit persons to whom the Software is
>furnished to do so, subject to the following conditions:  
>
>The above copyright notice and this permission notice shall be included in
>all copies or substantial portions of the Software.  
>
>THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
>IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
>FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
>AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
>LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
>OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
>THE SOFTWARE.  

##ClickButton manual

###This lib is not mine, I'm just using it on owner's license.

>Copyright (C) 2010,2012, 2013 raron
>  
> GNU GPLv3 license
> 
> This program is free software: you can redistribute it and/or modify
> it under the terms of the GNU General Public License as published by
> the Free Software Foundation, either version 3 of the License, or
> (at your option) any later version.
> 
> This program is distributed in the hope that it will be useful,
> but WITHOUT ANY WARRANTY; without even the implied warranty of
> MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
> GNU General Public License for more details.
> 
> You should have received a copy of the GNU General Public License
> along with this program.  If not, see <http://www.gnu.org/licenses/>.
> Contact: raronzen@gmail.com
> 
> History:
> 2013.08.29 - Some small clean-up of code, more sensible variable names etc.
>                Added another example code for multiple buttons in an object array
> 2013.04.23 - A "minor" debugging: active-high buttons now work (wops)!
>                Thanks goes to John F. H. for pointing that out!
> 2013.02.17 - Some improvements, simplified click codes.
>				Added a LED fader example. Thanks to Tom K. for the idea.
> 2012.01.31 - Tiny update for Arduino 1.0
> 2010.06.15 - First version. Basically just a small OOP programming exercise.

##Prescaler.h  
This library is used to fix millis() and delay() issues caused by changing PWM multiplier.
Thanks to this library now I don't need to multiplicate every function by 64 :D
Also not mine, taken from [Arduino.cc](http://playground.arduino.cc/Code/Prescaler).

> prescaler.h  
>  
>  Provides useful tools to manage the clock prescaler and issues  
>  related to time and delays. Allows to easily set the prescaler  
>  and get access to its value. Also provides alternative functions  
>  to the millis() and delay() functions.  
>  
>  (c) 2008 Sofian Audry | info(@)sofianaudry(.)com  
>  http://sofianaudry.com  
> 
>  This program is free software: you can redistribute it and/or modify  
>  it under the terms of the GNU General Public License as published by  
>  the Free Software Foundation, either version 3 of the License, or  
>  (at your option) any later version.  
>  
>  This program is distributed in the hope that it will be useful,  
>  but WITHOUT ANY WARRANTY; without even the implied warranty of  
>  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the  
>  GNU General Public License for more details.  
>  
>  You should have received a copy of the GNU General Public License  
>  along with this program.  If not, see <http://www.gnu.org/licenses/>.  