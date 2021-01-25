# IotHomeMesh
### ESP12/ESP32 IoT Mesh Network  
  
The goal of this project is to implement a relatively straightforward plug-and-play IoT system that is also extensible.  
The ideal final product would be one that could be fitted into any house regardless of type or construction

#### Current Target  

Current goal is early prototyping; try to control a WS2811 RGB strip using an ESP12.
One ESP12 will control the RGB lights directly, while a second ESP12 will be connected to a computer. The second ESP12 module will be receiving inputs from a computer through a serial com port, parsing them, and then forwarding them as commands to the ESP12 controlling the lights.  
In future, the role of the ESP12 sending commands should be played by a SBC (Raspberry Pi, LattePanda, etc) serving as a bridge and access point between the user (on a mobile phone, computer, etc) and the target device (light, power outlet, appliance, etc).  

##### To Do:  
##### Programming:  
- Get communication between two ESP12s (NodeMCUs) working
- Program RGB patterns on the 'device' ESP12
- Program serial input on the 'bridge' ESP12
- Develop protocol
    - Protocol must must strings (due to the nature of the TCP connections used in the painlessMesh library
- Write a C application capable of sending serial communications through a specific com port in Linux
    - Also develop functionality for Windows machines
- Develop server application to facilitate control through ESP12/32
- Enable OTA updates

Interoperability is a must; the code must be lightweight, portable and workable for a wide range of devices. The protocol must also be simple and robust enough to be extensible into other networks, for example there may be two mesh networks, one using 2.4 GHz WiFi range and another using lower frequency radio for longer communication; a bridge should be able to effectively join multiple networks together using the same standard protocol.
##### Circuit Development:
- Find ways to deliver power to ESP12/ESP32 modules and associated devices
    - 24V/12V/5.5V DC input to 3.3V DC output
    - Rectify and step down 120V-60Hz (American) AC to 3.3V DC
    - Preferably maintaining an efficiency above 80%
    - Power must be delivered to the ESP12/ESP32 *and* the device(s) being controlled which may require power in a different form (i.g. ESP12 at 3.3V controlling 60 WS2811s at 12V)
    - Batteries are always an option but would result in a need for constant replacement which would be a hassle, especially in the case of devices imbedded into walls and ceilings or in other hard to access places
- Use of bistable latches for control of high power devices (i.g. a switch controllable by the 3.3V logic of an ESP to switch a 120V AC lamp on/off)
    - Must be bistable and capable of holding a position or signal *even without an input signal*, meaning an MC would need to only apply an 'on' signal momentarily for the latch to switch on and *stay* on
- Development of circuits capable of controlling 120V AC current
    - dimming of lamps
    - preferably capable of maintaining a specific power output despite losing a control signal
- Miniaturization is to be a goal in design (i.g. an ESP12 controller and associated circuitry should be able to be easily inserted into a preexisting wall or light socket)

Stability is a big factor; should the ESP12/ESP32 lose power or stop providing a signal, the end device should be able to either return to a safe default state *or* maintain its last state.
