/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * ev3motor_copy.c
 * Takes the position and the speed to go of the Large Motor plugged in OutA
 * , and gives it to the Large Motor plugged in OutB.
 *
 * Use Touche Sensor any ports to reset both Large Motors.
 * 
 * Middle brick button to close programme.
 * 
 * !! No check if unpugged Large Motors while running !!
 * 
 * Copyright (C) 2017 - Will_tam
 * 
 * ev3motor_copy is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * ev3motor_copy is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>

#include "ev3.h"
#include "ev3_port.h"
#include "ev3_sensor.h"
#include "ev3_tacho.h"

uint8_t wait_for_touch_sensor(void)
{
	/*
	 * Try to detect the Touch Sensor in anyone ports.
	 * @parameters : void.
	 * @return : the sequence number as 8 bits unsined int.
	*/

	bool nok = true;
	uint8_t sn;
	
	while (nok)
	{
		if (ev3_sensor_init() > -1)
		{
			nok = ! ev3_search_sensor(LEGO_EV3_TOUCH, &sn, 0);
		}
	}
	
	return sn; 
}

uint8_t wait_for_large_motor(uint8_t port)
{
	/*
	 * Try to detect a Large Motor in specified port.
	 * @parameters : port = 8 bits unsigned int port number 
	   (see http://in4lio.github.io/ev3dev-c/group__ev3__port.html#ga5cf2b6f10962d9254e937c35de5e6f12).
	 * @return : the sequence number as 8 bits unsigned int.
	*/

	bool nok = true;	/* jump if = is faster than jump if != */
	uint8_t sn;
	
	while (nok)
	{
		if (ev3_tacho_init() > -1)
		{
			nok = ! ev3_search_tacho_plugged_in(port, EXT_PORT__NONE_, &sn, 0);
		}
	}
	
	return sn; 
}

bool touch_sensor_pressed(uint8_t sn_touch)
{
	/*
	 * Check if Touch Sensor is pressed.
	 * @parameters : sn_touch = 8 bits unsigned int sequence number of the Touch Sensor.
	 * @return : true if the Touch Sensor was pressed, everelse false.
	*/

	int value;

	return (get_sensor_value(0, sn_touch, &value) && (value != 0));
}

uint8_t brick_key_pressed(void)
{
	/*
	 * Check which brick's key is pressed.
	 * @parameters : void.
	 * @return : the key number 
	   (see ev3_both.h @ http://in4lio.github.io/ev3dev-c/ev3__both_8h_source.html).
	*/

	uint8_t keypressed = EV3_KEY__NONE_;	/* No key pressed by default */

	ev3_read_keys(&keypressed);

	return (keypressed);
}

void run_to_abs_pos(uint8_t sn_lmotor, int position, int speed)
	/*
	 * run the outB plugged motor at absolute position .
	   (see example @ http://in4lio.github.io/ev3dev-c/tacho_8c-example.html).
	 * @parameters : sn_lmotor = unsigned 8 bits integer sequence number of the Large Motor. 
					 position = integer of the position of the outA plugged motor.
	 *				 speed = integer of the speed of the OutA plugged motor.
	 * @return : void. 
	*/

{
	set_tacho_stop_action(sn_lmotor, "break"); /* just for fun */
	set_tacho_speed_sp(sn_lmotor, speed);
	set_tacho_position_sp(sn_lmotor, position);
	set_tacho_command_inx(sn_lmotor, TACHO_RUN_TO_ABS_POS);
}

void info(const char *to_print)
{
	/*
	 * Print simple a simple string and flush it at once.
	 * @parameters : to_print = a char pointer to the string to print.
	 * @return : void.
	*/

	printf("%s", to_print);
	fflush(stdout); 
}


int main()
{
	enum {
		M_ORIG,
		M_DEST
	};

	uint8_t sn_touch;
	uint8_t sn_lmotors[2];	  /* [motor OUTA, motor OUTB] */

	int from_pos, from_speed;

	touch_sensor_pressed(0);

	if (ev3_init() < 1)
	{
		info("\nCan't initialize the EV3 brick !\nExit.\n");
		return (1);
	}

    info("\nPlease, plug the Touch Sensor in one of 'in' ports.\n");
	sn_touch = wait_for_touch_sensor();
	
    info("\nPlease, plug the holding large motor in hand in the 'outA' port.\n");
	sn_lmotors[M_ORIG] = wait_for_large_motor(OUTPUT_A);
	
    info("\nPlease, plug the destination large motor in the 'outB' port.\n");
	sn_lmotors[M_DEST] = wait_for_large_motor(OUTPUT_B);

	/* Absolutely want to reset the both large motors at first !!! */
    info("\n\nMove the both large motors at their wanted position 0, and press Touch Sensor.");
	while (! touch_sensor_pressed(sn_touch))
	{
		fflush(stdout);
	}

	multi_set_tacho_command_inx(sn_lmotors, TACHO_RESET);
    info("\n\nZero of the both large motors choosen !\n\n");

    info("\n\nMove the motor plugged in outA port."\
         "\nTouch sensor to reset the both motors."\
         "\nMiddle brick's button to stop program.\n");    

	while (brick_key_pressed() != EV3_KEY_CENTER)
	{
		if (touch_sensor_pressed(sn_touch))
		{   
			multi_set_tacho_command_inx(sn_lmotors, TACHO_RESET);
			/* Playing twice 50Hz sound during 100ms with 10ms between the both. */
			system("/usr/bin/beep beep -f 50 -r 2 -d 10 -l 100");
		}

/*      
		The destination motor is simply rotate at the same position as the original one,
        with the same speed (a.k.a power in original Lego Brick Program)
        The destination motor has to stop at once.
*/
		get_tacho_position(sn_lmotors[M_ORIG], &from_pos);
		get_tacho_speed(sn_lmotors[M_ORIG], &from_speed);

		/* mDest.run_to_abs_pos(position_sp=mOrig.position, speed_sp=mOrig.speed, stop_action="hold") */
		run_to_abs_pos(sn_lmotors[M_DEST], from_pos, from_speed);
	}

	info("\n\n");
	system("/usr/bin/beep");

	ev3_uninit();

	return (0);
}

