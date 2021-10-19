#include <WS2812Serial.h>
//#include <Servo.h>
#include <PulsePosition.h>
#include "leds.h"

PulsePositionOutput v_fan1(FALLING);
PulsePositionOutput v_fan2(FALLING);
PulsePositionOutput h_fan1(FALLING);
PulsePositionOutput h_fan2(FALLING);

#define NEUTRAL 1600 // Motor stopped
#define MIN_FWD 1670
#define MIN_REV 1535

byte drawingMemory[241*3];         //  3 bytes per LED
DMAMEM byte displayMemory[241*12]; // 12 bytes per LED

light* leds_[241];
uint8_t quad = 0;


WS2812Serial leds(241, displayMemory, drawingMemory, 1, WS2812_GRB);

void setup() {

	for (int i = 0; i < 241; ++i)
	{
		/* code */
		leds_[i] = new light(i);
	}

	leds_[0]->init();

	leds.begin();
		
	v_fan1.begin(9);
	v_fan1.write(1, NEUTRAL);

	v_fan2.begin(10);
	v_fan2.write(1, NEUTRAL);

	h_fan1.begin(5);
	h_fan1.write(1, NEUTRAL);

	h_fan2.begin(6);
	h_fan2.write(1, NEUTRAL);

	for (int i = 0; i < 241; i++) {
		leds.setPixel(i, 0);
	}
	
	leds.show();

	delay(500);
	//v_fan1.write(93);

	Serial.println("Begin");

}
static uint16_t j = 0;
static uint16_t k = 0;
static uint8_t fan = 0;
static uint16_t timer = 1000;
static uint32_t color = 170;
uint16_t pulse = 1525;

elapsedMillis timer1;

elapsedMillis h_fan_timer = 0;
uint32_t h_fan_time = 0;
uint8_t h_fan_on = 0;
elapsedMillis v_fan_timer = 0;
uint32_t v_fan_time = 0;
uint8_t v_fan_on = 0;

void loop() {
	LEDs();

	if (h_fan_timer > h_fan_time) {
		if (h_fan_on) {
			h_fan_on = 0;
			h_fan_timer = 0;
			h_fan_time = random(1000, 7500);
			h_fan1.write(1, NEUTRAL);
			h_fan2.write(1, NEUTRAL);
		}
		else {
			h_fan_on = 1;
			h_fan_timer = 0;
			h_fan_time = random(300, 750);
			switch (random(-1, 4)) {
				case -1: h_fan1.write(1, MIN_REV); break;
				case 0: h_fan1.write(1, NEUTRAL); break;
				default: h_fan1.write(1, MIN_FWD); break;
			}
			switch (random(-1, 4)) {
				case -1: h_fan2.write(1, MIN_FWD); break;
				case 0: h_fan2.write(1, NEUTRAL); break;
				default: h_fan2.write(1, MIN_REV); break;
			}
		}
	}

	if (v_fan_timer > v_fan_time) {
		if (v_fan_on) {
			v_fan_on = 0;
			v_fan_timer = 0;
			v_fan_time = random(1000, 3000);
			v_fan1.write(1, MIN_REV);
			v_fan2.write(1, NEUTRAL);
		}
		else {
			v_fan_on = 1;
			v_fan_timer = 0;
			v_fan_time = random(250, 1000);
			switch (random(-1, 5)) {
				case -1: v_fan1.write(1, MIN_FWD); break;
				case 0: v_fan1.write(1, NEUTRAL); break;
				default: v_fan1.write(1, MIN_REV); break;
			}
			switch (random(-1, 2)) {
				case -1: v_fan2.write(1, MIN_FWD); break;
				case 0: v_fan2.write(1, NEUTRAL); break;
				default: v_fan2.write(1, MIN_REV); break;
			}
			if (random(0, 20) == 0) {
				v_fan_time = 500;
				v_fan1.write(1, 1900);
				v_fan2.write(1, 1900);
			}
		}
	}

	delay(100);
}



void LEDs() {
	uint8_t num_leds = 0;
	for(int i =0; i<241; i++)
	{
		uint32_t tmp = leds_[i]->update();
		if(tmp != 0) num_leds ++;
		leds.setPixel(i, tmp);
	}
	j++;
	if(j%25 == 0 && num_leds < 10)
	{
			if(j%3)
			{
				color += random(10);
			}
			else
			{
				color -= random(8);
			}
		/*	if(j%4==1)
			{
				uint8_t partner = leds_[random(241)]->set_fast(Wheel(color));
		  	leds_[partner]->set_fast(Wheel(color));
			}*/
			//else
			{
				uint8_t led;
				if(quad == 5) 
					{
						led = 241;
						quad = 0;
					}
				else
					led = quad_to_i(quad, k++/*random(60)*/);
				uint8_t partner = leds_[led]->set(Wheel(color));
		  	leds_[partner]->set(Wheel(color));
		  	quad ++;
		  	if(k>241) k = 0;
			}
		  
	}
	leds.show();
}
/*
void color_loop(int loop, int color)
{
	uint8_t offset = ring_offset[loop];
	for(int i=0; i < ring_size[loop]; i++)
	{
		leds.setPixel(i+offset, color);
	}
	leds.show();

}

void setPixel(uint8_t loop, uint8_t idx, uint32_t color)
{
	uint8_t offset = ring_offset[loop];
	leds.setPixel(idx+offset, color);
}*/

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  uint8_t dim = 1;
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
	return ((255 - WheelPos * 3)/dim << 16 |  0 | (WheelPos * 3)/dim);
  }
  if(WheelPos < 170) {
	WheelPos -= 85;
	return (0 | (WheelPos * 3)/dim<< 8 |  (255 - WheelPos * 3)/dim);
  }
  WheelPos -= 170;
  return ((WheelPos * 3)/dim<< 16 | (255 - WheelPos * 3)/dim<<8 |  0);
}
/*
void colorWipe(int color, int wait) {
	for (int i=0; i < 241; i++) {
		leds.setPixel(i, color);
		//leds.show();
		//delayMicroseconds(wait);
	}
}*/
