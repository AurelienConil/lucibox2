/*
 *        LUCIBOX GENERIQUE
 *        MOON LOOPSTATION
 *        github.com/undessens/lucibox
 *        
 *        Author : Aurelien Conil
 *        Casemate Fablab de Grenoble
 */

 #include "Adafruit_NeoPixel.h"

//NEOPIXEL
#define PIN            12

#define NUMPIXELS      8
#define MAXLUMINOSITY  0.1


#define ANALOGIN 3    // Nombre de potentiometre
#define DIGITALIN 10   // Nombre de boutons
#define DIGITALOUT 0  // Nombre de leds
#define ANALOG_THRESH 10 

//--------------- NEOPIXEL ----------------
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_RGB +  NEO_KHZ800);
int digitaloutValue[DIGITALOUT];
int ledStrip1DotLooper[NUMPIXELS];
int ledStrip3DotsLooper[NUMPIXELS];
int ledStripSelection[NUMPIXELS];
int digitaloutPin[] = {};

// ------------   POTENTIOMETRE --------------
int analogValue[ANALOGIN];
int analogPin[] = { A0, A1, A2};

// ------------   BOUTONS  ------------------
int digitalinValue[DIGITALIN];
int digitalinPin[] =  { 2,3,4,5,6,7,8, 9, 10, 11};

void setup(){

    Serial.begin(38400);
    while(!Serial);
    
   
  
  for( int i=0 ;i<ANALOGIN ; i++){
    
    pinMode(analogPin[i], INPUT);
    analogValue[i] = analogRead(analogPin[i]);

  }

  for(int i=0 ; i<DIGITALIN ; i++ ){
    pinMode(digitalinPin[i], INPUT_PULLUP);
    digitalinValue[i] = digitalRead(digitalinPin[i]);
  }

  pixels.begin();
  for (int i= 0; i<NUMPIXELS; i++){
     setNeoPixel(i, 0,0,200);
  }
  pixels.show();
  // Init the memory of these virtual strip leds
  for (int i= 0; i<NUMPIXELS; i++){
     ledStrip1DotLooper[i] = 0 ;
     ledStrip3DotsLooper[i] = 0;
     ledStripSelection[i] = 0;
  }
  pinMode(digitaloutPin[0], OUTPUT );
  pinMode(digitaloutPin[1], OUTPUT );

   
      

}

void loop(){
  
  // READ analog pin and write to serial
  for (int i= 0; i<ANALOGIN; i++){
   
    
    int newValueAn = analogRead(analogPin[i]);

    //Send message if value is different
    if ( abs( newValueAn - analogValue[i]) > ANALOG_THRESH ){
     
      analogValue[i] = newValueAn;
      sendMessage(i, map(newValueAn, 0, 1024, 0, 255 )); 
      
    }

    }

    //READ digital pin and write to serial
    for (int i= 0; i<DIGITALIN; i++){
    int newValueDi = !digitalRead(digitalinPin[i]);  
    if( newValueDi != digitalinValue[i] ){

      digitalinValue[i] = newValueDi;
      int finalValue = 0;
      if (newValueDi) finalValue = 255;
      sendMessage(i + 20, newValueDi*255); 
      
    }


    
    
  }
  
  
  // Serial receive message
  /*
   *  Message to strip led
   *  content of a message : mode index channel value end_of_line
   *  mode : 1( one dot loop ) 2 ( 3 dots loop ) 3 ( selection ) 4 ( song )
   *  index : index of looper
   *  channel : only for 3 dots loop
   *  value : color to print  O (off) 2 (red) 3(light white) 4( medium white) 5( blue) 6 ( yellow ) 7 ( green )
   *  
   *  in order to refresh without changing, send index = 0
   */
  while(Serial.available()>4){

    int mode = (Serial.read()) ;
    int index = (Serial.read()) - 1 ; // 1 is the first looper
    int channel = (Serial.read()) ;
    int value = (Serial.read()) ;
    char end_of_line = Serial.read();
    
    if(end_of_line == 13 ){
      
         switch(mode){
             /* One dot looper. One color for each looper */
             case 1:
                   if(index>=0) set1DotLooper( index, value);
                   update1DotLooper();
              break;
              /* 3 dot looper. 3 dots for each looper. Basically rec | recorded | play */
             case 2:
                   if(index>=0) set3DotsLooper( index, channel, value);
                   update3DotsLooper();
                break; 
              /* Selection . dots white according  to the current selection */
            case 3: 
                   if(index>=0) setSelection( index, value );
                   updateSelection();
                break;
                /* Song . one dot according to the current song open */
           case 4:
                   if(index>=0) setSong( index, value );
              break;
                      

             
             }


     
     
     
    }
   
   

    
  }
  // Pixels are not updated when changed
  //pixels.show();

  // How to know if computer is connected
  /* the only to know is to wait for the 1st messages received
   * boolean isConnected = false;
   * Dès que le premier serial.available() existe, c'est variable est passée à false.
   * Pas moyen de savoir si la connection est coupé
   * if(Serial) sert juste juste à savoir si phyiquement la prise usb est reliée à l'ordinateur.
   */
  
  
  delay(5);
  
  
}


void sendMessage(int address, int value){

// Normal way to send a message

 Serial.print(char(address) );
 if(value ==14) value = 13;
 Serial.print(char(value));
 Serial.print(char(14));

/*
//debug way to send a message
Serial.print("ad: ");
Serial.print(address);
Serial.print(" - val: ");
Serial.println(value);
 */
}

void setNeoPixel(int channel, int r, int v, int b){

  int finalr, finalv, finalb;
  finalr = r*MAXLUMINOSITY;
  finalv = v*MAXLUMINOSITY;
  finalb = b*MAXLUMINOSITY;

  pixels.setPixelColor(channel, pixels.Color(finalv,finalr,finalb));
  

}

void setColorNeoPixel(int channel,int colorIndex){

  int finalr ;
  int finalg ;
  int finalb ;
  
  
  // if colorIndex is 1 or 0 : set On or Off
  if(channel>=0  && channel<NUMPIXELS){

   
    switch( colorIndex){
        //OFF
        case 0 :finalr = 0; finalg = 0; finalb = 0;
        break;
        //RED
        case 2: finalr = 0; finalg = 25; finalb = 0;
          break;
        //LIGHT WHITE
        case 3: finalr = 2; finalg = 2; finalb = 2;
          break;
        //MEDIUM WHITE
        case 4: finalr = 30; finalg = 30; finalb = 30;
          break;
        //BLUE
        case 5: finalr = 0; finalg = 0; finalb = 25;
          break;
        // YELLOW
        case 6: finalr = 12; finalg = 12; finalb = 0;
          break;
       // GREEN
        case 7: finalr = 25; finalg = 0; finalb = 0;
          break;
    }

    // TODO ; choose if inverted or not
    pixels.setPixelColor(NUMPIXELS-(channel+1), pixels.Color(finalr,finalg,finalb));


  }

  
}


void set1DotLooper( int index, int value){

  ledStrip1DotLooper[index] = value;
  
}

void update1DotLooper(){
 
 for (int i= 0; i<NUMPIXELS; i++){
     setColorNeoPixel(i,ledStrip1DotLooper[i]);
  }
  pixels.show();
  
}

void set3DotsLooper( int index,int channel, int value){

  int finalIndex = index*3 + channel;
  ledStrip3DotsLooper[finalIndex] = value;
  
  
}

void update3DotsLooper(){
  for (int i= 0; i<NUMPIXELS; i++){
     setColorNeoPixel(i,ledStrip3DotsLooper[i]);
  }
  pixels.show();
}

void setSelection( int index,int value ){

  ledStripSelection[index] = value;
}

void updateSelection(){
  for (int i= 0; i<NUMPIXELS; i++){
     int finalColor = 0;
     if(ledStripSelection[i]>0) finalColor = 3; //Medium white
     setColorNeoPixel(i,finalColor);
  }
  pixels.show();
}

void setSong(int index, int value){
//Song don't need to be updated. Don't need to save current state in a virtual led strip
// -> draw it directly

  int selectedColor = 0;
  

  switch(value){
    case 0 : selectedColor = 0;
    break;
    case 1 : selectedColor = 7; // Gren means "loading"
    break;
    case 2 : selectedColor = 2; // Red means "recording"
    break;
  }
  
  for (int i= 0; i<NUMPIXELS; i++){
     int finalColor = 0;
     if(i==index) finalColor = 5; //Blue;
     setColorNeoPixel(i,finalColor);
  }
  pixels.show();
}
