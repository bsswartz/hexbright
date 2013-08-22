/*
Copyright (c) 2013, "Bailey Swartz" <bsswartz@gmail.com>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the FreeBSD Project.
*/



#include <hexbright.h>
#include <Wire.h>

// Modes
#define MODE_OFF 0
#define MODE_RUNNING 1

static char mode = MODE_OFF;

static const int click = 350; // Maximum duration of a normal click (milliseconds)

hexbright hb;

void setup() {
  // We just powered on!  That means either we got plugged
  // into USB, or the user is pressing the power button.
  hb.init_hardware();
}

void loop() {
   //Have the HexBright library handle the boring stuff!
   hb.update();
   if(!hb.printing_number()) {
     hb.print_power();
   }
   
   
   // When a normal click is encountered, perform state transitions.
   // This only happens once for a transition, only perform
   // initialization tasks!!!
   if(hb.button_just_released() && hb.button_pressed_time() < click) {
     switch(mode){
       // We are off, but someone wants light now... engage light at full power.
       case MODE_OFF:
        mode=MODE_RUNNING;
        hb.set_light(CURRENT_LEVEL, MAX_LEVEL, 50);
        break;
        
        // A normal press when the light is on should always turn it off.
       default:
        mode=MODE_OFF;
        hb.shutdown();
        break; 
     }
   }
   
   // The button is beind held down, check if we are in a button long-hold condition.
   if (hb.button_pressed()) {  
     if (hb.button_pressed_time() > click) {
       // If the hold is from an off state, give the user some medium light.
       if (mode != MODE_RUNNING) {
         mode=MODE_RUNNING;
         hb.set_light(CURRENT_LEVEL, 500, 50);
       }
     } 
     
     if (hb.button_pressed_time() > 2*click) {
       // It has been longer, do angle adjust mode.
       adjustLED();
     }
   }
}

int adjustLED() { 
  double angle = hb.difference_from_down(); // Between 0 (straight down) and 1 (straight up)
  int intensity = (int)(angle * 2 * MAX_LEVEL); // Translate to light intensity range.
  
  //Force intensity value to be within range.  Truncating at MAX_LEVEL 
  //from the total range of 2 * MAX_LEVEL has the effect
  //of creating full intensity at horizontal level and above.
  intensity = (intensity > MAX_LEVEL) ? MAX_LEVEL : intensity;
  intensity = (intensity <= 0) ? 1 : intensity;
  
  mode=MODE_RUNNING;
  hb.set_light(CURRENT_LEVEL, intensity, 100);
  
  return intensity;
}
