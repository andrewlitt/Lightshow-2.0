
// Program parameters
#define avg  8            // Window of moving average
int thres = 25;         // Threshold to activate channel
double redScalar = 1.25;   // Red multiplier
double bluScalar = 1.4;    // Blue multiplier
double grnScalar = 0.8;    // Green multiplier

// Configuration
int analogPin = 0; // A0
int strobePin = 2; // D2
int resetPin = 3;  // D3
int redPin = 10;   // D10
int grnPin = 11;   // D11
int bluPin =  9;   // D9

int spectrumValue[7];
int val[3] = {0,0,0};
int pins[3] = {redPin,grnPin,bluPin};
double scalar[3] = {redScalar,grnScalar,bluScalar};
int prevVal[avg][3];

void setup()
{
     Serial.begin(9600);
     pinMode(analogPin, INPUT);
     pinMode(strobePin, OUTPUT);
     pinMode(resetPin, OUTPUT);
     analogReference(DEFAULT);

     digitalWrite(resetPin, LOW);
     digitalWrite(strobePin, HIGH);

     LightStartup();

     delay(20);
     for(int p = 0; p < 3; p++){
        for(int v = 0; v < avg; v++){
          prevVal[v][p] = 0;
        }
     }
}

void loop()
{
    digitalWrite(resetPin, HIGH);
    digitalWrite(resetPin, LOW);

    for (int i = 0; i < 7; i++){
         digitalWrite(strobePin, LOW);
         delayMicroseconds(30); // to allow the output to settle
         spectrumValue[i] = analogRead(analogPin);
         //PrintValue(spectrumValue[i]);
         digitalWrite(strobePin, HIGH);
    }
    //Serial.println();
    val[0] =  map(spectrumValue[0], 0, 1023, 0, 255);
    val[1] =  map(spectrumValue[3], 0, 1023, 0, 255);
    val[2] =  map(spectrumValue[6], 0, 1023, 0, 255);

    // For each channel value
    for(int p = 0; p < 3; p++){

      
      // Set to 0 if output below a threshold
      if(val[p] < thres) val[p] = 0;

      // Multiply by channel scalar (Goal: Boost Red & Blu, subdue Grn)
      double d = val[p];
      d *= scalar[p];
      val[p] = (int) d;
      
      // Shift the Value history back one entry
      for(int v = 0; v < avg-1; v++) prevVal[v][p] = prevVal[v+1][p];
      prevVal[0][p] = val[p];

      // Adjust the value according to a moving average of past X samples
      int  tmp = 0;
      for(int v = 0; v < avg; v++) tmp += prevVal[v][p];
      val[p] = tmp/avg;
      PrintValue(val[p]);
    }
    Serial.println();
    // Write the outputs
    for(int p = 0; p < 3; p++) analogWrite(pins[p],val[p]);
    delay(10);
}

void PrintValue(int val){
    if (val < 10){
        Serial.print("   ");
        Serial.print(val);
    }
    else if (val < 100){
        Serial.print("  ");
        Serial.print(val);
    }
    else{
        Serial.print(" ");
        Serial.print(val);
    }
}

void LightStartup(){
  for(int p = 0; p < 3; p++){
      for (int i = 0; i < 255; i++){
         analogWrite(pins[p], i);
         delay(1);
      }
      for (int i = 255; i > 0; i--){
         analogWrite(pins[p], i);
         delay(1);
      }
  }
}
