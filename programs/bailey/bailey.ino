#include <hexbright.h>
#include <Wire.h>

// Modes
#define MODE_OFF 0
#define MODE_RUNNING 1

static char mode = MODE_OFF;

static const int click = 350; // maximum time for a "short" click

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
   // This only happens once, do not put in state runtime code!!!
   if(hb.button_just_released() && hb.button_pressed_time() < click) {
     switch(mode){
       
       // We are off, but someone wants light now... engage light at full power.
       case MODE_OFF:
        mode=MODE_RUNNING;
        hb.set_light(CURRENT_LEVEL, MAX_LEVEL, 100);
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
         hb.set_light(CURRENT_LEVEL, 500, 100);
       }
     } 
     
     if (hb.button_pressed_time() > 2*click) {
       // It has been longer, do angle adjust mode.
       adjustLED();
     }
   }
}

int adjustLED() { 
  double angle = hb.difference_from_down(); // Between 0 and 1
  int intensity = (int)(angle*2000.0); // (0,2000)
  
  //Force intensity value to be within range.  Truncating at 1000 from the total range of 2000 has the effect
  //of creating full intensity at horizontal level and above.
  intensity = intensity>1000 ? 1000 : intensity;
  intensity = intensity<=0 ? 1 : intensity;
  
  mode=MODE_RUNNING;
  hb.set_light(CURRENT_LEVEL, intensity, 100);
  
  return intensity;
}

