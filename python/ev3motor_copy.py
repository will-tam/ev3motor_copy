#!/usr/bin/python3
# -*- coding: utf-8 -*-

# Standard library import.
import sys

# Third-part library import.
import ev3dev.ev3 as ev3ev3
import ev3dev.core as ev3core
import ev3dev.brickpi as ev3bp
import ev3dev.helper as ev3h

# Project library import.

def waitFor(sensorMotor, port=None):
    """
    Try to detect a sensor or motor plugged in the ports.
    @parameters : sensorMotor = class of waiting sensor or motor.
                port = in which port is waiting the sensor or the motor. If none, autodiscovered port.
                Do not use port=None if there are several (for e.g.) motors to discover.
                Only the first will discovred.
    @return : sensor or motor instance.
    """
    connected = False

    while not connected:
        objInst = sensorMotor(port)
        connected = objInst.connected   # See 'ev3dev.ev3' to see where comes from 'connected' attribute.

    print("\tDetected !")
    return objInst

######################

def main(arg):
    """
    Main function.
    @parameters : some arguments, in case of use.
    @return : 0 = all was good.
              ... = some problem occures.
    """

    btn = ev3ev3.Button()   # Not need to be discovered. Always on the brick.

    print("\nPlease, plug the Touch Sensor in one of 'in' ports")
    ts = waitFor(ev3ev3.TouchSensor)

    print("\nPlease, plug the holding large motor in hand in the 'outA' port")
    mOrig = waitFor(ev3ev3.LargeMotor, 'outA')

    print("\nPlease, plug the destination large motor in the 'outB' port")
    mDest = waitFor(ev3ev3.LargeMotor, 'outB')

    print("\n\nMove the both large motors at their wanted position 0, and press Touch Sensor")

    # Absolutely want to reset the both large motors at first !!!
    while not ts.value():
        pass

    mOrig.reset()
    mDest.reset()

    print("Zero of the both large motors choosen !\n\n")

    print("Move the motor plugged in outA port.")
    print("Touch sensor to reset the both motors.")
    print("Middle brick's button to stop program.")

    while not btn.enter:
        if ts.value():
            mOrig.reset()
            mDest.reset()
            # Playing twice 50Hz sound during 100ms with 10ms between the both.
            ev3ev3.Sound.tone([(50, 100, 10)] * 2).wait()

        # The destination motor is simply rotate at the same position as the original one,
        # with the same speed (a.k.a power in original Lego Brick Program)
        # The destination motor has to stop at once.
        mDest.run_to_abs_pos(position_sp=mOrig.position, speed_sp=mOrig.speed, stop_action="hold")

    print("\n")
    ev3ev3.Sound.beep()

    return 0

######################

if (__name__ == "__main__"):
    rc = main(sys.argv[1:])      # Keep only the argus after the script name.
    sys.exit(rc)
