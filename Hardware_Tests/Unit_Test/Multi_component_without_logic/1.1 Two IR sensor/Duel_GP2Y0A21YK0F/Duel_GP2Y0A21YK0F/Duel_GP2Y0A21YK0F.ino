/*   
modified on Sep 8, 2020
Modified by MohammedDamirchi from https://github.com/MajenkoLibraries/Average
Home 
*/ 
#include <Average.h>

// Reserve space for 10 entries in the average bucket.
// Change the type between < and > to change the entire way the library works.
Average<float> ave(10);
Average<float> ave2(10);

void setup() {
    Serial.begin(9600);
}

void loop() {
    int minat = 0;
    int maxat = 0;
    
    // Add a new random value to the bucket
    ave.push(27.728/0.7*1.3 * pow(map(analogRead(A0), 0, 1023, 0, 5000) / 1000.0, -1.2045));
    Serial.print("IR1=");
    Serial.print(ave.mean());
        
    // Add a new random value to the bucket
    ave2.push(27.728/0.7*1.3 * pow(map(analogRead(A1), 0, 1023, 0, 5000) / 1000.0, -1.2045));
    Serial.print(" ; IR2=");
    Serial.println(ave2.mean());
    delay(100);
}