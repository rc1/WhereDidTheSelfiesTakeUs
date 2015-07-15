const int buttonPin01 = 9;     // the number of the pushbutton pin
const int buttonPin02 = 10;     // the number of the pushbutton pin


int buttonState01 = 0;             // the current reading from the input pin
int lastButtonState01 = HIGH;   // the previous reading from the input pin
boolean buttonPressed01 = false;

int buttonState02 = 0;             // the current reading from the input pin
int lastButtonState02 = HIGH;   // the previous reading from the input pin
boolean buttonPressed02 = false;


// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime01 = 0;  // the last time the output pin was toggled
long lastDebounceTime02 = 0;  // the last time the output pin was toggled


long debounceDelay = 50;    // the debounce time; increase if the output flickers

int frameCounter = 0;
int led = 13;




void setup() {
  
  pinMode(buttonPin01, INPUT);
  pinMode(buttonPin02, INPUT);
  pinMode(led, OUTPUT); 
  
  Serial.begin(9600);

  lastButtonState01 = digitalRead(buttonPin01);
  lastButtonState02 = digitalRead(buttonPin02);
  
  delay (100);

}

void loop() {

  
    //---------------------------------
  // read the state of the button ONE
  int reading01 = digitalRead(buttonPin01);
  
  if (reading01 != lastButtonState01) {
    // reset the debouncing timer
    lastDebounceTime01 = millis();
  } 
  
  if ((millis() - lastDebounceTime01) > debounceDelay) {
    buttonState01 = reading01;
  }
  
      if (buttonState01 == LOW && buttonPressed01 == false){
      buttonPressed01 = true;
      Serial.write("1 \n");
      frameCounter++;
 
    }
      else if (buttonState01 == HIGH){
      buttonPressed01 = false;
    }

  lastButtonState01 = reading01;
  
  //---------------------------------
  // read the state of the button TWO
  int reading02 = digitalRead(buttonPin02);
  
  if (reading02 != lastButtonState02) {
    // reset the debouncing timer
    lastDebounceTime02 = millis();
  } 
  
  if ((millis() - lastDebounceTime02) > debounceDelay) {
    buttonState02 = reading02;
  }
  
      if (buttonState02 == LOW && buttonPressed02 == false){
      buttonPressed02 = true;
      Serial.write("2 \n");
      frameCounter = 0;
    }
      else if (buttonState02 == HIGH){
      buttonPressed02 = false;
    }

  lastButtonState02 = reading02;
  
//  if (frameCounter >= 5){
//    digitalWrite(led, HIGH);
//  }
//  else {
//    digitalWrite(led, LOW);
//  }
  
  
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read(); 
    // add it to the inputString:
    
    if (inChar == '0'){
      digitalWrite(led, LOW);
    }
    if (inChar == '1'){
      digitalWrite(led, HIGH);
    }
    
//    inputString += inChar;
//    // if the incoming character is a newline, set a flag
//    // so the main loop can do something about it:
//    if (inChar == '\n') {
//      stringComplete = true;
//    } 
  }
}
