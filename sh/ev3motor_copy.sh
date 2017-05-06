#!/bin/bash

legoPort=/sys/class/lego-port/port

######################################################################"

waitForTouchSensor()
{
    # Try to find the Touch Sensor port between port 0 to port 3 (in1 to in 4).
    # If the port of sensor if found, stop this function and go on next part.
    # @parameters : none.
    # @return : the complete path of the Touch Sensor.
    let find=0

    while [[ $find == 0 ]]; do
        for nPort in 0 1 2 3;do   # It's ok, only 4 ports !
            port=$(ls $legoPort$nPort | grep "lego-ev3-touch")
            [[ $port ]] && nSensor=$(ls $legoPort$nPort/$port/lego-sensor)
            # echo <=> return for other languages. break for for loop.
            [[ $nSensor ]] && let find=1 && echo $legoPort$nPort/$port/lego-sensor/$nSensor && break
        done
    done
}

waitForLargeMotors()
{
    # Try to find a Large Motor at the specified port : outA:lego-ev3-l-motor or outB:lego-ev3-l-motor.
    # If the port of sensor if found, stop this function and go on next part.
    # @parameters : the port where should be the Large Motor.
    # @return : the complete path of the Large Motor.
    case "$1" in
        outA)
            nPort=4;;
        outB)
            nPort=5;;
        *)
            return 1;;
    esac

    while true;do
        port=$(ls $legoPort$nPort | grep lego-ev3-l-motor)
        # echo <=> return for other languages. break for while loop.
        [[ $port ]] && nMotor=$(ls $legoPort$nPort/$port/tacho-motor) && echo $legoPort$nPort/$port/tacho-motor/$nMotor && break
    done
}

resetMotor()
    # Reset the motor.
    # @parameters : port of the motor to reset.
    # @return : none.
{
    echo
    for motor in $@; do
        echo "reset" > $motor/command
    done
}

btnEnter()      # TODO : how to peek it ?
{
    kbdDevPath=/dev/input/by-path/platform-gpio-keys.0-event
#    echo $(cat $kbdDevPath)
}

thisEnd()
{
    # Running if CTL+C is pressed.
    # @parameters : none.
    # @return : none.
    echo -en "\n"
    beep

    exit 0
}

######################################################################"

ts=""
mOrig=""
mDest=""

clear

echo -en "\nPlease, plug the Touch Sensor in one of 'in' ports.\n"
ts=$(waitForTouchSensor)
echo -en "Discovered in '$(echo $ts | cut -d":" -f 1 | cut -d"/" -f 6)'\n"  # the port is always at the 6th field, before the ":".

echo -en "\nPlease, plug the holding large motor in hand in the 'outA' port.\n"
mOrig=$(waitForLargeMotors 'outA')
echo -en "Discovered in '$(echo $mOrig   | cut -d":" -f 1 | cut -d"/" -f 6)'\n"  # the port is always at the 6th field, before the ":".

echo -en "\nPlease, plug the destination large motor in the 'outB' port.\n"
mDest=$(waitForLargeMotors 'outB')
echo -en "Discovered in '$(echo $mDest   | cut -d":" -f 1 | cut -d"/" -f 6)'\n"  # the port is always at the 6th field, before the ":".

echo -en "\n\nMove the both large motors at their wanted position 0, and press Touch Sensor"
# Absolutely want to reset the both large motors at first !!!
while [ $(cat $ts/value0) -eq "0" ] ;do
    /bin/false
done

resetMotor $mOrig $mDest
echo -en "Zero of the both large motors choosen !\n\n"

echo -en "Move the motor plugged in outA port.\n"
echo -en "Touch sensor to reset the both motors.\n"

#echo -en "Middle brick's button to stop program."  # Uncomment if btnEnter() is used.
echo -en "Any keys to stop program.\n"      # Delete it if use btnEnter().

# Not really nice. TODO : change this ending way. (See TODO in btnEnter()).
trap thisEnd EXIT   # The real exit program is at this thisEnd() way.

# The destination motor has to stop at once.
echo "hold" > $mDest/stop_action

# Big problem : the speed reading and sending values between the both Large Motors !
# The whole comment out of the while loop.
    # Playing twice 50Hz sound during 100ms with 10ms between the both.

    # The destination motor is simply rotate at the same position as the original one,
    # with the same speed (a.k.a power in original Lego Brick Program)

while true;do
#    btnEnter

    if [ $(cat $ts/value0) -eq "1" ]; then
        resetMotor $mOrig $mDest
        beep -f 50 -l 100 -d 10 -r 2
    fi

    echo $(cat $mOrig/speed) > $mDest/speed_sp
    echo $(cat $mOrig/position) > $mDest/position_sp
    echo "run-to-abs-pos" > $mDest/command

done

echo -en "Should be not here !!"    # To control the good ending trap.
echo -en "\n"
beep

exit 0
