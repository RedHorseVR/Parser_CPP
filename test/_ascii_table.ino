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
void setup() //beginfunc
{
    Serial.begin(9600);
    while (!Serial) //beginwhile
    {
        ;
    } //endwhile

    Serial.println("ASCII Table ~ Character Map");
} //endfunc
// first visible ASCIIcharacter '!' is number 33:
int thisByte = 33;

// for example, '!' is the same as 33, so you could also use this:
// int thisByte = '!';
void loop() //beginfunc
{
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

    { // you could also use if (thisByte == '~') {

        while (true) //beginwhile
        {
            int sensorReading = analogRead(A0);
            // map the sensor range to a range of four options:
            int range = map(sensorReading, sensorMin, sensorMax, 0, 3);

            // do something different depending on the range value:
            switch (range) //beginswitch
            {
                case 0: // your hand is on the sensor
                    Serial.println("dark");
                    break;
                case 1: // your hand is close to the sensor
                    Serial.println("dim");
                    break;
                case 2: // your hand is a few inches from the sensor
                    Serial.println("medium");
                    break;
                case 3: // your hand is nowhere near the sensor
                    Serial.println("bright");
                    break;
            } //endswitch
            continue;
        } //endwhile
    } //endif
    thisByte++;
} //endfunc
//  Export  Date: 10:48:54 AM - 23:Apr:2025;
