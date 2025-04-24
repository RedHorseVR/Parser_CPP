/*
Switch statement
Demonstrates the use of a switch statement. The switch statement allows you
to choose from among a set of discrete values of a variable. It's like a
series of if statements.
To see this sketch in action, put the board and sensor in a well-lit room,
open the Serial Monitor, and move your hand gradually down over the sensor.
The circuit:
- photoresistor from analog in 0 to +5V
- 10K resistor from analog in 0 to ground
created 1 Jul 2009
modified 9 Apr 2012
by Tom Igoe
This example code is in the public domain.
https:
*/


const int sensorMin = 0;
const int sensorMax = 600;
void setup()
	{
	
	Serial.begin(9600);
	}
void loop()
	{
	
	int sensorReading = analogRead(A0);
	
	int range = map(sensorReading, sensorMin, sensorMax, 0, 3);
	
	switch (range)
	{
	case 0:
		Serial.println("dark");
		break;
	case 1:
		Serial.println("dim");
		break;
	case 2:
		Serial.println("medium");
		break;
	case 3:
		Serial.println("bright");
		break;
		}
	delay(1);
	}
//  Export  Date: 12:11:31 PM - 23:Apr:2025;

