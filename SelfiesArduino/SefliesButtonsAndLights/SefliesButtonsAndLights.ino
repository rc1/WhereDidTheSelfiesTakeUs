const int capturePin = 9; 
const int savePin = 10;
const int ledPin = 13;
const int baud = 9600;

long unsigned int lastCaptureTriggerMS = 0;
long unsigned int lastSaveTriggerMS = 0;

void setup () {
    pinMode( capturePin, INPUT );
    pinMode( savePin, INPUT );
    pinMode( ledPin, OUTPUT ); 
    Serial.begin( baud ); 
    delay( 100 );
}

void loop() {
    // Throttle the capture button
    if ( millis() - lastCaptureTriggerMS > 200 && digitalRead( capturePin ) == HIGH ) {
        Serial.write( "1" );
        lastCaptureTriggerMS = millis();
    }
    // Throttle the display button
    if ( millis() - lastSaveTriggerMS > 200 && digitalRead( savePin ) == HIGH ) {
        Serial.write( "2" );
        lastSaveTriggerMS = millis();
    }
}

void serialEvent() {
    while ( Serial.available() ) {
        char inChar = (char) Serial.read(); 
        if ( inChar == '0' ) {
            digitalWrite( ledPin, LOW );
        }
        else if ( inChar == '1' ) {
            digitalWrite( ledPin, HIGH );
        }
    }
}
