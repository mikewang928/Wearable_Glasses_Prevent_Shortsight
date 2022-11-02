

// Definition of Variable
int16_t s_en = 4;
int16_t s0 = 5;
int16_t s1 = 6;
int16_t s2 = 7;


// read mutiplexer
int readMux(int channel) { 
  int controlPin[] = {s0, s1, s2}; 
  int muxChannel[8][3] = { 
    // {0,0,0}, //channel 0 
    // {0,0,0}, //channel 0 
    // {0,0,0}, //channel 0 
    {0,0,0}, //channel 0 

    {1,0,0}, //channel 1 
    // {1,0,0}, //channel 1 
    // {1,0,0}, //channel 1 
    // {1,0,0}, //channel 1 

    {0,1,0}, //channel 2 
    // {0,1,0}, //channel 2 
    // {0,1,0}, //channel 2 
    // {0,1,0}, //channel 2 

    {1,1,0}, //channel 3 
    // {1,1,0}, //channel 3 
    // {1,1,0}, //channel 3 
    // {1,1,0}, //channel 3 

    {0,0,1}, //channel 4 
    {1,0,1}, //channel 5 
    {0,1,1}, //channel 6 
    {1,1,1}, //channel 7  //loop through the 3 Signals 
  };
  for(int i = 0; i < 3; i++){ // Connecting MUX Channel
    digitalWrite(controlPin[i], muxChannel[channel][i]); 
    Serial.print(muxChannel[channel][i]);
  }   
}


void setup() {
  Serial.begin(115200); // Baud Rate

  pinMode(s_en, OUTPUT);
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);

  digitalWrite(s_en, HIGH);
  digitalWrite(s0, HIGH); // 1
  digitalWrite(s1, HIGH); // 1
  digitalWrite(s2, HIGH); // 1
}



void loop() {

  digitalWrite(s_en, LOW); // Enabling the Enable Switch
  for(int j=0; j<8;j++){
    readMux(j);
    delay(500);
  }
}




