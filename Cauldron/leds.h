#include <WS2812Serial.h>
	
#define red(a) (a>>16 & 0xFF)
#define green(a) (a>>8 & 0xFF)
#define blue(a) (a & 0xFF)

#define red_(a) ((a<<16)&0xFF0000)
#define green_(a) ((a<<8)&0x00FF00)
#define blue_(a) ((a)&0x0000FF)

uint8_t ring_size[] = {60, 48, 40, 32, 24, 16, 12, 8, 1};
uint8_t quad_size[] = {15, 12, 10,  8,  6,  4,  3, 2, 1};
uint8_t ring_offset[9];
uint8_t quad_offset[9];

#define R_UP 1
#define R_DO 20
#define F_UP 3
#define F_DO 30

uint8_t quad_to_i(uint8_t quad, uint8_t ix)
{
	uint8_t i;
	for(i = 0; i < 8; i++)
	{
		if(ix < quad_offset[i+1])
			break;
	}
	uint8_t offset = ix - quad_offset[i];
	return quad_size[i]*quad + ix;

}

class light{
	public:
			uint32_t val = 0;
			uint32_t tgt = 0;
			uint8_t idx;
			uint8_t partner;
			uint32_t ramp_up = 1;
			uint32_t ramp_down = 20;
			bool is_rising = false;
			bool is_falling = false;

	light(int8_t _idx)
	{
		idx = _idx;
		if(_idx >= 241) partner = 240;
		else partner = _idx;
			/*if(idx > ring_offset[8])
			{
				partner = idx - 1;
				return;
			}
			for(uint8_t i = 1; i < 9; i++)
			{
				if(idx == ring_offset[i] - 1)
				{
					partner = idx - 1;
					return;
				}

			}*/
	}

	void init()
	{
		ring_offset[0] = 0;
		quad_offset[0] = 0;
		for(int i=1; i<9; i++)
		{
			quad_offset[i] = quad_offset[i-1] + quad_size[i-1];
			ring_offset[i] = ring_offset[i-1] + ring_size[i-1];
		}
	}

	uint8_t set(uint32_t color)
	{
		tgt = color;
		is_rising = true;
		ramp_up = R_UP;
		ramp_down = R_DO;
		return partner;
	}

	uint8_t set_fast(uint32_t color)
	{
		tgt = color;
		is_rising = true;
		ramp_up = F_UP;
		ramp_down = F_DO;
		return partner;
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