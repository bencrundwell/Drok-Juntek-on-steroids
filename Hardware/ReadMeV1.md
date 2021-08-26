this was my first design which is now deprecated.

The design uses standard buck converters, an ESP32 TTGO TFT module, simple prototyping boards and a few resistors/condensators.

 ## Bill of materials
 
- Juntek DSP 3606
- TTGO ESP32 TFT board
- 4 x 6 cm prototyping board
- pin male / female pin headers
- 3 x 100 k resistors
- 3 x 180 k resistors
- 2 x 68 k resistors
- 3 x 5.6 M resistors
- 1 x 220 resistor **
- 1 x 3.9 M (behind the Juntek module to sense Vin)
- 4 x 330nF tantal capacitors **
- 1 x 470uF 16V chem capacitor **
- 1 x 47uF 16V chem capacitor **

The components marked (**) are not critical +- 50% are possible.

## Proto wiring
![image](https://user-images.githubusercontent.com/14197155/111206570-d34e2780-85c8-11eb-8fb4-a482fadf0cb9.png) 
![image](https://user-images.githubusercontent.com/14197155/111217876-59bd3600-85d6-11eb-8595-dd1af2165e28.png)

(the PCB layout is simple, even single layer, but i never made a Gerber file, if someone would like to contribute...)  
n.b.: *you may have noticed an apparent discrepancy between the jumper settings on the prototype pictures and the jumper settings in the schematic diagram:
the difference is only that I had to move the pin connectors by one 2.54 step upwards for mechanical reasons and did not want to redraw the diagram accordingly.  

## Schematic diagram
https://github.com/rin67630/Drok-Juntek-on-steroids/blob/main/Hardware/Schematic_Juntek%20on%20Steroids_2021-03-10.pdf

The adapter boards will be slightly different for Drok and Juntek since the pinout varies.  
*that variants to come soon, did not test yet*

Other ESP32 modules can be used, a variant with a BW OLED module and a variant with a HELTEC OLED + LoRa module are  planned, just to give an outlook of the conceptual flexibility.  
The ESP32's ADC are really bad and the simple solution with a few resistors/condensators works, but are stained with +-50mV and +- 50mA error range.
A more precise version with a separate ADC will be provided, requiring however mor components.
