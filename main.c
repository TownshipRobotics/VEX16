#pragma config(Motor,  port1,           leftArm,       tmotorVex269_HBridge, openLoop)
#pragma config(Motor,  port2,           frontRight,    tmotorServoContinuousRotation, openLoop)
#pragma config(Motor,  port3,           frontLeft,     tmotorServoContinuousRotation, openLoop)
#pragma config(Motor,  port4,           backLeft,      tmotorServoContinuousRotation, openLoop)
#pragma config(Motor,  port5,           backRight,     tmotorServoContinuousRotation, openLoop)
#pragma config(Motor,  port6,           claw,          tmotorNone, openLoop)
#pragma config(Motor,  port10,          rightArm,      tmotorNone, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//
// DO NOT TOUCH CONFIGURATION OPTIONS! >:U

//**********************************
//		OMNIWHEEL CONTROL
//**********************************

// Limits n to between -limit and limit
int lim(int n, int limit)
{
    if(n > limit) return limit;
    if(n < -limit) return -limit;
    return n;
}

// Makes values closer to 0 more sensitive:
// 0 -> 0; 127 -> 127; 64 -> 64^3/127^2 = 16.25
// As n -> 127, returns an int that increases exponentially
int curve(int n)
{
    return pow(lim(n, 127), 3)/16129;
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
int * getOmniPowers(int x, int y, int z) {
		float rotPer = 1.0-abs(z)/127.0;
    int d = curve(sqrt(pow(x, 2) + pow(y, 2)));
    float A = (1.27323954474 * atan2(y, x)+8)%8;
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

// Updates the power of the wheels
void updateOmni()
{
    int * powers = getOmniPowers(vexRT[Ch4], vexRT[Ch3], vexRT[Ch1]);
    motor[frontLeft] = powers[0];
    motor[frontRight] = powers[1];
    motor[backRight] = powers[2];
    motor[backLeft] = powers[3];
}

//**********************************
//		ARM CONTROL
//**********************************

//**********************************
//		CLAW CONTROL
//**********************************

bool open = false;

// Opens the claw
void openClaw() {
    // Check if claw is already open
    if(!open) {
        motor[port6] = 35;
        //wait 1/2 of a second (value will probably turn into a constant)
        sleep(500);
        //stop motor
        motor[port6] = 0;
        open = true;
    }
}

// Closes the claw
void closeClaw() {
    // Check if claw is already closed
    if(open) {
        motor[port6] = -35;
        //wait 3? (value will probably turn into a constant) seconds then stop
        sleep(500);
        //stop motor
        motor[port6] = 0;
        open = false;
    }
}

// Updates claw's position
void updateClaw() {
    if(vexRT[Btn5U] == 1) // If upper Z button down
        openClaw();
    else if(vexRT[Btn5D] == 1) // If lower Z button down
        closeClaw();
}

//**********************************
//		MAIN LOOP
//**********************************
task main()
{
    while(true) {
        updateOmni();
        updateClaw();
    }
}
