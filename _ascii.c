/*
ASCII table
Prints out byte values in all possible formats:
- as raw binary values
- as ASCII-encoded decimal, hex, octal, and binary values
For more on ASCII, see http:
The circuit: No external hardware needed.
created 2006
by Nicholas Zambetti <http:
modified 9 Apr 2012
by Tom Igoe
This example code is in the public domain.
https:
*/
const int sensorMin = 0;
const int sensorMax = 600;
void setup() {
	Serial.begin(9600);
	while (!Serial) { //beginwhile
		;
	} //endwhile

	Serial.println("ASCII Table ~ Character Map");
}
// first visible ASCIIcharacter '!' is number 33:
int thisByte = 33;
// for example, '!' is the same as 33, so you could also use this:
// int thisByte = '!';
void loop() {
	// will show up as '!'
	Serial.write(thisByte);
	Serial.print(", dec: ");
	Serial.print(thisByte);
	Serial.print(", hex: ");
	Serial.print(thisByte, HEX);
	Serial.print(", oct: ");
	Serial.print(thisByte, OCT);
	Serial.print(", bin: ");
	Serial.println(thisByte, BIN);
	// if printed last visible character '~' or 126, stop:
	if (thisByte == 126) //beginif

	{  // you could also use if (thisByte == '~') {
		delay(4000);
		while (true) { //beginwhile
			int sensorReading = analogRead(A0);
			int range = map(sensorReading, sensorMin, sensorMax, 0, 3);
			switch (range) { //beginswitch
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
			} //endswitch
			continue;
		} //endwhile
	} //endif
	thisByte++;
}

//  Export  Date: 06:53:33 PM - 24:Apr:2025;
