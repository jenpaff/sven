int input_switch = 2;
int led = 4;
unsigned long timer = 0;
long threshold = 5000;
long interval = threshold*2;
int counter = 1;

void setup() {
  pinMode (input_switch, INPUT_PULLUP);
  pinMode (led, OUTPUT);
  Serial.begin(9600);
  Serial.println("starting ...");
}

void loop() {
  
  bool switch_state = digitalRead(input_switch);
  
  Serial.print("timer = ");
  Serial.println(timer);
 
  if (switch_state) {
    Serial.println("open");
    Serial.println(switch_state);
    
    if ((unsigned long)(millis() - timer) >= threshold) {
      // if timer > 30 seconds, turn on light
      digitalWrite(led, switch_state);
      
      if ((unsigned long)(millis() - timer) >= interval) {
        // after that, send a warning every 30 seconds
        Serial.print("WARNING #");
        //counter=counter+1;
        Serial.println(counter++);
        interval+=threshold;
      }
    } 
  } else {
      Serial.println("closed");

      /*
       * light off and reset values
       */
      digitalWrite(led, switch_state); // light off
      timer = millis(); // "reset" timer
      counter = 1;
      interval = threshold*2;
  }
  delay(1000);
}
