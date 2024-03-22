#include <LiquidCrystal.h>

#include "pitches.h"

#define RED_LED 49
#define YELLOW_LED 47
#define GREEN_LED 45

// The following arrays include the music for Mary Had A Little Lamb, Twinkle Twinkle Little Star, Success Noise (TM PENDING), and Meh Noise (TM ALSO PENDING)
// These songs were selected as a result of my miniscule music crafting experience as well as the songs being extremely short.

// notes in the melody:
int MARY[] = {

  NOTE_E4, NOTE_D4, NOTE_C4, NOTE_D4, NOTE_E4, NOTE_E4, NOTE_E4, NOTE_D4, NOTE_D4, NOTE_D4, NOTE_E4, NOTE_G4, NOTE_G4, NOTE_E4, NOTE_D4, NOTE_C4, NOTE_D4, NOTE_E4, NOTE_E4, NOTE_E4,
  NOTE_E4, NOTE_D4, NOTE_D4, NOTE_E4, NOTE_D4, NOTE_C4, '\0'
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int MARY_DURATIONS[] = {

  4, 4, 4, 4, 4, 4, 8, 4, 4, 8, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 16
};

int TWINKLE[] = {
  NOTE_C4, NOTE_C4, NOTE_G4, NOTE_G4, NOTE_A4, NOTE_A4, NOTE_G4, NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4, NOTE_D4, NOTE_C4,
  NOTE_G4, NOTE_G4, NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4, NOTE_G4, NOTE_G4, NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4,
  NOTE_C4, NOTE_C4, NOTE_G4, NOTE_G4, NOTE_A4, NOTE_A4, NOTE_G4, NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4, NOTE_D4, NOTE_C4, '\0'
};

int TWINKLE_DURATIONS[] = {
  4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 8,
  4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 8,
  4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 16, '\0'
};

int SUCCESS[] {
  NOTE_F4, NOTE_F4, NOTE_G4, NOTE_D5, NOTE_C5, '\0' 
};

int SUCCESS_DURATIONS[] = {
  2, 2, 4, 2, 2
};

int FAIL[] {
  NOTE_A5, NOTE_G5, NOTE_A5, NOTE_G5, '\0' 
};

int FAIL_DURATIONS[] = {
  4, 4, 4, 4
};

int MEH[] {
  NOTE_D4, NOTE_E4, NOTE_F4, NOTE_A5, NOTE_G5, '\0' 
};

int MEH_DURATIONS[] = {
  4, 4, 4, 4, 4
};

// The following is the list of affirmations teh plant will issue when it has been happy
// for more than one cycle and the magic number was rolled.  

String affirmations[] = {
  "YOU'RE AMAZING",
  "YOU'RE AWESOME",
  "FANTASTIC",
  "WOWZA",
  "NO WAY!",
  "10/10 JOB",
  "YOU'RE THE BEST",
  "LIVING MY BEST LIFE",
  "WEEEEEE",
  "YOU'RE BEAUTIFUL",
  "GOOD JOB"
};

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

void playDiddy(String str) {
  int (* melody)[] = NULL;         // Create a pointer that will point to one of the note lists
  int (* noteDurations)[] = NULL;  // Create a pointer that will point to one of the note duration lists

  // Set the NULL pointers to the appropriate arrays based on the passed string
  if (str == "MARY") {
    melody = (int(*)[])&MARY;                     // Convert the fixed size notes array into a pointer to a sizeless array of notes
    noteDurations = (int (*)[]) &MARY_DURATIONS;  // Convert the fixed size note durations array into a pointer to a sizeless array of note durations
  } else if (str == "TWINKLE") {
    melody = (int(*)[])&TWINKLE;
    noteDurations = (int (*)[]) &TWINKLE_DURATIONS;
  } else if (str == "SUCCESS") {
    melody = (int(*)[]) &SUCCESS;
    noteDurations = (int (*)[]) &SUCCESS_DURATIONS;
  } else if (str == "FAIL") {
    melody = (int(*)[]) &FAIL;
    noteDurations = (int (*)[]) &FAIL_DURATIONS;
  } else if (str == "MEH") {
    melody = (int(*)[]) &MEH;
    noteDurations = (int (*)[]) &MEH_DURATIONS;
  }

  // Keep track of the currently playing note
  int thisNote = 0;

  // Traverse down the notes array until we hit the NULL terminator
  // Using a NULL terminator allows us to abstractly select an array rather than hardcoding this loop over and over
  do {
    // Execute each note for 100x it's not value; i.e. quarter notes = 4, half notes = 8, full notes = 16, then multiply by 100
    int noteDuration = (*noteDurations)[thisNote] * 100;

    // Pulse a tone on PWM Pin 6 with the appropriate frequency and duration
    tone(6, (*melody)[thisNote], noteDuration);

    // Add a note of rest between each note equal to the length of the played note
    delay(noteDuration);
  } while ((*melody)[++thisNote] != '\0');
}

// Plants are simple creatures, and being watered correctly once sets this flag 
// to true.  When happy, the plant will randomly issue affirmations to the child
// to encourage future watering.  The chances are rare to alleviate a poor parent
// from being harrassed from a typically inanimate object day in and day out.
bool happy = true;

// The unhappy timer indicates how many plant checks have passed with the
// plant begging it's caregiver to provide it the sustenance it needs to no avail
// It resets everytime the plant is made happy even once because the plant doesn't
// know better
int unhappyTimer = 0;

void checkMoisture() {
  int soilMoisture = analogRead(0);

  // The following ranges were picked from the water sensor's documentation regarding soil humidity ranges
  // See: https://wiki.keyestudio.com/Ks0049_keyestudio_Soil_Humidity_Sensor
  if (soilMoisture <= 300){
    if (unhappyTimer == 0 || unhappyTimer % 10 == 0) {
      // All lights must be either turned on or off since any light can switch to any other light
      // and arduinos do as they're told; nothing more, nothing less.  As such, the pins will stay
      // powered until told to do otherwise.
      digitalWrite(RED_LED, HIGH);
      digitalWrite(YELLOW_LED, LOW);
      digitalWrite(GREEN_LED, LOW);

      // The LCD is similarly lazy to the arduino and refuses to read my mind - among also simply
      // being rude and not displaying text properly because text wrapping is hard - thus, it too must
      // be reset before being written to again
      lcd.clear();
      lcd.print("WATER ME"); 
      playDiddy("FAIL");
    }

    happy = false;
    unhappyTimer += 1;
  } else if (soilMoisture >= 700){
    if (unhappyTimer == 0 || unhappyTimer % 10 == 0) {
      digitalWrite(RED_LED, HIGH);
      digitalWrite(YELLOW_LED, LOW);
      digitalWrite(GREEN_LED, LOW);
      lcd.clear();
      lcd.print("TOO MUCH WATER"); 
      playDiddy("FAIL");
    }

    happy = false;
    unhappyTimer += 1;
  }
  else if (soilMoisture <= 400 || soilMoisture >= 600) {
    if (unhappyTimer == 0 || unhappyTimer % 10 == 0) {
      digitalWrite(YELLOW_LED, HIGH);
      digitalWrite(GREEN_LED, LOW);
      digitalWrite(RED_LED, LOW);

      lcd.clear();
      lcd.clear();
      lcd.print("You're almost");
      lcd.setCursor(0, 1);
      lcd.print("there!");
      playDiddy("MEH");
    }

    happy = false;
    unhappyTimer += 1;
  }
  else {
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);

    lcd.clear();
    if (happy) {
      // The arduino picks a number 0-10
      long num = random(11);

      // I picked 7 to be the magic number because it is the statistically most likely to be rolled when rolling two dice
      // Mind you, the arduino does not roll dice, so this number is inherently meaningless in this context.
      if (num == 7) {
        lcd.clear();
        lcd.print(affirmations[random(11)]);
        // Creates an array of the different good song names
        String arr[] = {"MARY", "TWINKLE", "SUCCESS"};
        
        // Selects and plays one of the three nice tunes.  The plant nanny refused to play twinkle despite nonstop testing for 
        // several minutes, so I do not know if twinkle simply never plays or if the universe has decided it will not be sharing
        // it's twinkling stars with anyone
        playDiddy(arr[random(3)]);
      }
    } else {
      lcd.print("GOOD JOB WATERING!"); 
      playDiddy("SUCCESS");
    }

    happy = true;
    unhappyTimer = 0;
  }
}

void setup() {
  // Setup debug band so we can read messages through the serial monitor
  Serial.begin(9600);

  // Initialize the LCD with the appropriate rows and columns
  lcd.begin(16, 2);

  // Set traffic light LED pins to OUTPUT mode
  pinMode(49, OUTPUT);
  pinMode(47, OUTPUT);
  pinMode(45, OUTPUT);

  // You have to check the moisture in setup initially, or you'd have to
  // wait the full check interval before getting results.  This would be quite 
  // annoying if the interval was on par with the length of say a day rather 
  // than 30 seconds like it is now.
  checkMoisture();
}

// This global keeps track of the relative now.  Now becomes now everytime 
// the distance between now now and now exceeds the check interval, which 
// in this case is 30,000 milliseconds, aka 30 seconds
unsigned long now = millis();

void loop() {
  // Delay causes the whole arduino to freeze up, so you have to poll millis instead to be safe
  if (millis() - now >= 30000) {
    checkMoisture();
    now = millis();
  }
}

