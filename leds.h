#include <WS2812Serial.h>
	
#define red(a) (a>>16 & 0xFF)
#define green(a) (a>>8 & 0xFF)
#define blue(a) (a & 0xFF)

#define red_(a) ((a<<16)&0xFF0000)
#define green_(a) ((a<<8)&0x00FF00)
#define blue_(a) ((a)&0x0000FF)

class light{
	public:
			uint32_t val = 0;
			uint32_t tgt = 0;
			uint8_t idx;
			uint32_t ramp_up = 1;
			uint32_t ramp_down = 20;
			bool is_rising = false;
			bool is_falling = false;

	light(int8_t _idx)
	{
			idx = _idx;
	}

	void set(uint32_t color)
	{
		tgt = color;
		is_rising = true;
	}

	uint32_t update()
	{
		if(tgt == val) return val;
		if(is_rising)
		{
			uint8_t	r_tmp, g_tmp, b_tmp;
			
			r_tmp = red(val) + (ramp_up);
			g_tmp = green(val) + (ramp_up);
			b_tmp = blue(val) + (ramp_up);
			if(r_tmp > red(tgt)) r_tmp = red(tgt);
			if(g_tmp > green(tgt)) g_tmp = green(tgt);
			if(b_tmp > blue(tgt)) b_tmp = blue(tgt);
			val = red_(r_tmp) | green_(g_tmp) | blue_(b_tmp);

			if(r_tmp == red(tgt) && g_tmp == green(tgt) && b_tmp == blue(tgt))
			{
				is_rising = false;
				is_falling = true;
				tgt = 0;
			}
			/*val += (tgt-val)/2;
			if(val > tgt-1)
			{
				is_rising = false;
				is_falling = true;
				tgt	= 0;
			}*/
		}
		if(is_falling)
		{
			int8_t	r_tmp, g_tmp, b_tmp;
			
			r_tmp = (red(val) > (ramp_down)) ? red(val) - (ramp_down): 0;
			g_tmp = (green(val) > (ramp_down)) ? green(val) - (ramp_down): 0;
			b_tmp = (blue(val) > (ramp_down)) ? blue(val) - (ramp_down): 0;
			val = red_(r_tmp) | green_(g_tmp) | blue_(b_tmp);
			
			if(r_tmp == red(tgt) && g_tmp == green(tgt) && b_tmp == blue(tgt))
			{
				is_falling = false;
			}
		}
		return val;
	}
};