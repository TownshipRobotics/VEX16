#pragma config(Sensor, in1,             pot,            sensorPotentiometer)
#pragma config(Motor,  port1,           armLeft,       tmotorVex393_HBridge, openLoop)
#pragma config(Motor,  port2,           frontRight,    tmotorServoContinuousRotation, openLoop)
#pragma config(Motor,  port3,           frontLeft,     tmotorServoContinuousRotation, openLoop)
#pragma config(Motor,  port4,           backLeft,      tmotorServoContinuousRotation, openLoop)
#pragma config(Motor,  port5,           backRight,     tmotorServoContinuousRotation, openLoop)
#pragma config(Motor,  port6,           claw,          tmotorNone, openLoop)
#pragma config(Motor,  port10,          armRight,      tmotorVex393_HBridge, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

// DO NOT TOUCH CONFIGURATION OPTIONS! >:U

//speed variables
int verySlow = 10;
int slow = 30;
int medium = 50;
int fast = 100;
//wait variables
int normal = 125;
int uTurn = 250;

//**********************************
//		OMNIWHEEL CONTROL
//**********************************

// Limits n to between -limit and limit
int lim(int n, int limit)
{
    if(n > limit)
    		return limit;
    if(n < -limit)
    		return -limit;
    return n;
}

// Makes values closer to 0 more sensitive:
// 0 -> 0; 127 -> 127; 64 -> 40
// As n -> 127, returns an int that increases exponentially
// (It graphs like y = x/2+x^3)
int curve(int n)
{
    float l = lim(n, 127);
    return (int) (l*(0.5+pow(l, 2)/32258.619)+0.5);
}

// Takes xyz vectors and creates the correct omni powers for it
/*
Calculate vector as (A, d) where 0<=A<8, 0<=d<127
if(floor(A/2)):
	0= -1, A-1, 1, 1-A
	1= A-3, 1, 3-A, -1
	2= 1, 5-A, -1, A-5
	3= 7-A, -1, A-7, 1

multiply all by d
*/
int * getOmniPowers(int x, int y, int z)
{
		// The percent the bot is rotating
    float rotPer = 1.0-abs(z)/127.0;

    // The "distance" of joystick from center 0 <= d < 127
    int d = sqrt(pow(x, 2) + pow(y, 2));

    // The angle 0 <= A < 8 of the joystick in relation to origin
    // A = 0 is to the right
    float A = (1.27323954474 * atan2(y, x)+8)%8;

    // Too long to explain in a comment >.>
    // (Ask me if you want an explanation)
    int powers[4];
    switch(floor(A/2)) {
        case 0:
            powers[0] = z+rotPer*-d;
            powers[1] = z+rotPer*d*(A-1);
            powers[2] = z+rotPer*d;
            powers[3] = z+rotPer*d*(1-A);
            break;
        case 1:
            powers[0] = z+rotPer*d*(A-3);
            powers[1] = z+rotPer*d;
            powers[2] = z+rotPer*d*(3-A);
            powers[3] = z+rotPer*-d;
            break;
        case 2:
            powers[0] = z+rotPer*d;
            powers[1] = z+rotPer*d*(5-A);
            powers[2] = z+rotPer*-d;
            powers[3] = z+rotPer*d*(A-5);
            break;
        case 3:
            powers[0] = z+rotPer*d*(7-A);
            powers[1] = z+rotPer*-d;
            powers[2] = z+rotPer*d*(A-7);
            powers[3] = z+rotPer*d;
    }

    return powers;
}

// Sets the powers of each of the wheels for the x, y, z numbers specified
void setOmniPowers(int x, int y, int z)
{
    // Array with the powers for each motor
    int * powers = getOmniPowers(x, y, z);
    // Set each motor's power correspondingly
    motor[frontLeft] = powers[0];
    motor[frontRight] = powers[1];
    motor[backRight] = powers[2];
    motor[backLeft] = powers[3];
}

// Updates the power of the wheels
void updateOmni()
{
    // Update the omniwheel powers to the vexRT powers
    setOmniPowers(curve(-vexRT[Ch4]), curve(vexRT[Ch3]), curve(vexRT[Ch1]));
}

//**********************************
//		ARM CONTROL
//**********************************
//move arm up or down with speed sent through the
void raiseArm(int power)
{
		motor[armLeft] = -power;
  	motor[armRight] = power;
}

void updateArm()
{
    // Gets the power from joystick & curves it
    int power = 0;

    if(vexRT[Btn6D] == 1)
    		power += -64;
    if(vexRT[Btn6U] == 1)
    		power += 64;

    // Compensate for gravity using potentiometer readings
    power += 18-ceil(SensorValue[pot]/128.0);

    // Set motors to proper powers
    raiseArm(power);
}

//**********************************
//		CLAW CONTROL
//**********************************


// Opens the claw
void openClaw()
{
		motor[claw] = 35;
		//wait 1/2 of a second (value will probably turn into a constant)
		sleep(500);
 		//stop motor
  	motor[claw] = 0;
}

// Updates claw's position
void updateClaw()
{
    if(vexRT[Btn5U] == 1) // If upper Z button down
        motor[claw] = 35;
    else if(vexRT[Btn5D] == 1) // If lower Z button down
        motor[claw] = -35;
    else
    		motor[claw] = 0;
}

//**********************************
//		AUTOMATION
//**********************************

//knock the jacks off of the wall
void knockJacksOff()
{
		while(time1[T1] < 3000){
				raiseArm(slow);
				sleep(100);
				raiseArm(-slow);
				sleep(100);
		}
}

// autonomous code for right side of arena
void rightAutoPeriod()
{
    // Raise arm
    raiseArm(medium);
    // wait
    sleep(normal);
    // stop motors
    raiseArm(0);

    //move to wall
    setOmniPowers(0, fast, 0);
    sleep(1500);
    setOmniPowers(0,0,0);

    //drop jack
    setOmniPowers(0, 0, medium);
    sleep(uTurn);
    openClaw();

		//knock jacks off of the wall
    setOmniPowers(0, -slow, 0);
    sleep(100);
    setOmniPowers(verySlow, 0, 0);
    knockJacksOff();
}

// autonomous code for left side of arena
void leftAutoPeriod()
{
    // Raise arm
    raiseArm(medium);
    // wait
    sleep(normal);
    // stop motors
    raiseArm(0);

    //move to wall
    setOmniPowers(0, fast, 0);
    sleep(1500);
    setOmniPowers(0, 0, 0);

    //drop jack
    setOmniPowers(0, 0, medium);
    sleep(uTurn);
    openClaw();

		//knock jacks off of the wall
    setOmniPowers(0, -slow, 0);
    sleep(100);
    setOmniPowers(-verySlow, 0, 0);
    knockJacksOff();

}

//**********************************
//		MAIN LOOP
//**********************************
task main()
{
  	if(vexRT[Btn7R] == 1) {
    		rightAutoPeriod();
  	}
  	else if(vexRT[Btn7L] == 1) {
    		leftAutoPeriod();
  	}
  	else if(vexRT[Btn8L] == 1) {
    		// Loop forever updating controls
    		while(true) {
      			updateOmni();
      			updateArm();
    				updateClaw();
  			}
		}
}
