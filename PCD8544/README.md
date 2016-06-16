#What is it?  

**PCD8544** is a library for the [Arduino](http://arduino.cc/) to interface with LCDs based on the
Philips PCD8544 controller ([datasheet](https://github.com/carlosefr/pcd8544/blob/docs/docs/pcd8544.pdf?raw=true))
or compatibles. These displays are commonly found on older monochrome mobile phones, such as the
[Nokia 3310](http://en.wikipedia.org/wiki/Nokia_3310) or [5110](http://en.wikipedia.org/wiki/Nokia_5110),
so if you have one of these stuck in a drawer, take it out and start hacking away! :)

#How it Works  

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

#Custom Symbols  

The library allows the use of custom bitmap symbols (5x8), defined by an array of five bytes.
To make it easy to create custom symbols, there's a graphical glyph editor
[available online](http://cloud.carlos-rodrigues.com/projects/pcd8544/).  

#This isn't my library!
I'm using it on following license:  
The MIT License (MIT)

>Copyright (c) 2013 Carlos Rodrigues <cefrodrigues@gmail.com>

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