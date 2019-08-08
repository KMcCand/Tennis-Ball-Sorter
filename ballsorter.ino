#include <LiquidCrystal_I2C.h>

/* File Name: TFMP_example.ino
 * Developer: Bud Ryerson
 * Inception: 29 JAN 2019
 * Last work: 07 JUN 2019
 *
 * Description: Arduino sketch to test the Benewake TFMini Plus
 * time-of-flight Lidar ranging sensor using the TFMPlus library.
 *
 * Default settings for the TFMini Plus are a 115200 serial baud rate
 * and a 100Hz measurement frame rate. The device will begin returning
 * measurement data right away:
 *   Distance in centimeters,
 *   Signal strength in arbitrary units,
 *   and an encoded number for Temperature in degrees centigrade.

   **********************     IMPORTANT    ************************
   ****  Changed name of 'buildCommand()' to 'sendCommand()'.  ****
   ****************************************************************

 * Use the 'sendCommand' to send commands and return a status code.
 * Commands are selected from the library's list of defined commands.
 * Parameters can be entered directly (115200, 250, etc) but for
 * safety, they should be chosen from the library's defined lists.
 *
 */

#include <TFMPlus.h>  // Include TFMini Plus Library v1.3.4
TFMPlus tfmP;         // Create a TFMini Plus object

#include "printf.h"   // May not work corrrectly with Intel devices

//#include <SoftwareSerial.h>       // alternative software serial library
//SoftwareSerial mySerial(10, 11);  // Choose the correct RX, TX Pins


#define BALL_GOING_UP 0
#define BALL_GOING_DOWN 1
#define BALL_NOT_IN_PLAY 2
#define BEYOND_TUBE_LENGTH 180
#define MILLIS_BETWEEN_FRAMES 2
#define NOISE_THRESHOLD 5
#define GREEN_LED_PIN 8

int ballState = BALL_NOT_IN_PLAY;
long maxDist = 0;
long minDist = BEYOND_TUBE_LENGTH; // the tube is 120 cm tall
uint16_t tfDist;       // Distance measurement in centimeters (default)
uint16_t tfFlux;       // Luminous flux or intensity of return signal
uint16_t tfTemp;       // Temperature in degrees Centigrade (coded)
uint16_t loopCount;    // Loop counter (1-20)
unsigned long startMs;
bool ballIsFalling;
bool lastBallIsFalling;


char array1[]=" SunFounder               "; //the string to print on the LCD
char array2[]="hello, world!             "; //the string to print on the LCD
int tim = 500; //the value of delay time
// initialize the library with the numbers of the interface pins
LiquidCrystal_I2C lcd(0x27,16,2); // set the LCD address to 0x27 for a 16 chars and 2 line display

/*  - - - - -  A few useful Lidar commands    - - - - - */

// Try three times to get the firmware version number
// which is saved as 'tfmp.version', a three byte array.
void firmwareVersion() {
  
    for( uint8_t fvi = 1; fvi < 4; ++fvi) {
      
        if( tfmP.sendCommand( OBTAIN_FIRMWARE_VERSION, 0)) {
          
            // If successful, display the version number...
            printf( "Lidar firmware version: %1u.%1u.%1u\r\n", tfmP.version[ 0], tfmP.version[ 1], tfmP.version[ 2]);
            break;      // and brreak out of loop.
        }
        
        else {
          
            // If not successful, display the attempt number
            // and the error: HEADER, CHERCKSUM, SERIAL, tec.
            printf( "Get firmware version failed. "); // Display error message...
            printf( "Attempt: %u", fvi);              // attempt number..
            tfmP.printStatus( false);                 // and error status.
            printf("\r\n");
            
        }
        delay(100);  // Wait to try again
    }
}

void factoryReset() {
  
    printf( "Lidar factory reset ");
    if( tfmP.sendCommand( RESTORE_FACTORY_SETTINGS, 0)) {
      
        printf( "passed.\r\n");
    }
    else {
      
        printf( "failed.");
        tfmP.printStatus( false);
    }
}

void frameRate( uint16_t rate) {
  
    printf( "Lidar frame rate ");
    if( tfmP.sendCommand( SET_FRAME_RATE, rate)) {
      
        printf( "set to %2uHz.\r\n", rate);
    }
    
    else {
      
        printf( "command failed.");
        tfmP.printStatus( false);
    }
}

void saveSettings() {
  
    printf( "Lidar device settings ");
    if( tfmP.sendCommand( SAVE_SETTINGS, 0)) {
        printf( "saved.\r\n");
    }
    else {
        printf( "not saved.");
        tfmP.printStatus( false);
    }
}

/*  - - -   End of useful Lidar commands   - - - -   */

void setup() {
  
    Serial.begin( 115200);   // Intialize terminal serial port
    delay(20);               // Give port time to initalize
    printf_begin();          // Initialize printf.
    printf("\r\nTFMPlus Library Example - 07JUN2019\r\n");  // say 'hello'

    Serial1.begin( 115200);  // Initialize TFMPLus device serial port.
    delay(20);               // Give port time to initalize
    tfmP.begin( &Serial1);   // Initialize device library object and...
                             // pass device serial port to the object.

    // Send commands to device during setup.
    firmwareVersion();
    frameRate(500); 
    //saveSettings();
    //factoryReset();

    // Initialize the variables for this example
    loopCount = 0;         // Reset loop counter.
    tfDist = 0;            // Clear device data variables.
    tfFlux = 0;
    tfTemp = 0;

    pinMode(GREEN_LED_PIN, OUTPUT);
    greenOff();

    delay(500);            // And wait for half a second.
    startMs = millis();

    lcd.init(); //initialize the lcd
    lcd.backlight(); //open the backlight 
}

void greenOn() {
  digitalWrite(GREEN_LED_PIN, HIGH);
}

void greenOff() {
  digitalWrite(GREEN_LED_PIN, LOW);
}

// Use the 'getData' function to pass back device data.
void loop() {

  long timeStamp = millis();
  
  // 1. On each loop, try up to five tries to get a good data frame.
  for( uint8_t fvi = 1; fvi < 6; ++fvi) {
    
      // printf( "Loop:%02u", loopCount);           // Display the loop count.
      
      if( tfmP.getData( tfDist, tfFlux, tfTemp)) { // Get data from the device.
        
        if (tfDist < 110) {
          
          if (ballIsFalling == false) {
            startMs = timeStamp;
            ballIsFalling = true;
            loopCount = 0;
          }
          
          //printf( " Loop:%05u time: %07u Dist:%04u\r\n", loopCount, (int) (timeStamp - startMs), tfDist);           // Display the distance.
          
        } else {
          
          ballIsFalling = false;
        }
        break;                                   // Escape this sub-loop
        
      } else {                       // If the command fails...
        printf( "getData failed. The reason:\n");
        tfmP.printStatus( true);  // display the error.
        tfDist = -1;
      }
  }

  if (tfDist >= 0) {

      //printf( "tfDist: %d\n", tfDist);
      //lcd.clear();
      if (loopCount % 5 == 0) {
       lcd.setCursor(1, 0); // set the cursor to column 1, line 0
       lcd.print("                ");
       String s = String(tfDist) + "    ";
       lcd.setCursor(1, 0); // set the cursor to column 1, line 0
       lcd.print(s );
      }
      
    if (ballState == BALL_NOT_IN_PLAY && tfDist < 20) {

      greenOn();
      ballState = BALL_GOING_DOWN;
      printf ( "Ball entered tube and is going down!!!!!!!!\n");
      //lcd.setCursor(1,0); // set the cursor to column 15, line 0
      //lcd.print(s);
      maxDist = 0;
      minDist = BEYOND_TUBE_LENGTH;
    }
    
    else if (ballState == BALL_GOING_DOWN) {
      //printf( "tfDist: %d\n", tfDist);
      
      if  (tfDist > maxDist) {
        maxDist = tfDist;
      }
      else if (maxDist - tfDist > NOISE_THRESHOLD) {
        ballState = BALL_GOING_UP;
        printf ( "Ball bounced and is going up!!!!!\n");
      }
    }
    
    else if (ballState == BALL_GOING_UP) {
       //printf( "tfDist: %d\n", tfDist);
      
       if (tfDist < minDist) {
         minDist = tfDist;
       }
       else if (tfDist - minDist > NOISE_THRESHOLD) {
         ballState = BALL_NOT_IN_PLAY;
         printf( "Ball reached max and bounced up to: %d\nBall is no longer in play.\n", minDist);
         greenOff();
         String s = String(minDist);
         lcd.setCursor(1, 1); // set the cursor to column 1, line 1
         lcd.print("Min: " + s + "     ");
       }
       
    }
  }

  // 2. Every twenty loops, show the two additional values.
  /*if( loopCount >= 20)
  {
    // Display signal strength in arbitrary units.
    printf( " Flux:%05u", tfFlux);

    // Decode temperature data and display as degrees Centigrade.
    uint8_t tfTempC = uint8_t(( tfTemp / 8) - 256);
    printf( " Temp:% 2uC", tfTempC);

    // Convert temperature and display as degrees Farenheit.
    uint8_t tfTempF = uint8_t( tfTempC * 9.0 / 5.0) + 32.0;
    printf( "% 3uF", tfTempF);

    loopCount = 0;                   // Reset loop counter.
  }
*/

  /*
  lcd.setCursor(15,0); // set the cursor to column 15, line 0
  for (int positionCounter1 = 0; positionCounter1 < 26; positionCounter1++) {
    //lcd.scrollDisplayLeft(); //Scrolls the contents of the display one space to the left.
    lcd.print(array1[positionCounter1]); // Print a message to the LCD.
    delay(tim); //wait for 250 microseconds
  }
  
  lcd.clear(); //Clears the LCD screen and positions the cursor in the upper-left  corner.
  lcd.setCursor(15,1); // set the cursor to column 15, line 1
  for (int positionCounter = 0; positionCounter < 26; positionCounter++) {
    //lcd.scrollDisplayLeft(); //Scrolls the contents of the display one space to the left.
    lcd.print(array2[positionCounter]); // Print a message to the LCD.
    delay(tim); //wait for 250 microseconds
  }
  
  lcd.clear(); //Clears the LCD screen and positions the cursor in the upper-left corner.
  // 3. Finish up and advance the loop counter
  // printf("\r\n");  // Send CR/LF to terminal
  */
  
  loopCount++;

  long expectedTimeMs = startMs + loopCount * MILLIS_BETWEEN_FRAMES;
  
  //printf(" The delay time:%07u\n", delayTimeMs);
  
  while (millis() < expectedTimeMs);
  
}
