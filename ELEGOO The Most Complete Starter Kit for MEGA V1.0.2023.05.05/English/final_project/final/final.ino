
#include <LiquidCrystal.h>
  LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

#include <Keypad.h>
  const byte ROWS = 4; //four rows
  const byte COLS = 4; //four columns
  //define the symbols on the buttons of the keypads
  char hexaKeys[ROWS][COLS] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
  };
  byte rowPins[ROWS] = {22, 23, 24, 25}; //connect to the row pinouts of the keypad
  byte colPins[COLS] = {26, 27, 28, 29}; //connect to the column pinouts of the keypad

  //initialize an instance of class NewKeypad
  Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

#include <Servo.h>
  Servo myservo;

#include "IRremote.h"
  int receiver = 13; // Signal Pin of IR receiver to Arduino Digital Pin 11

  /*-----( Declare objects )-----*/
  IRrecv irrecv(receiver);     // create instance of 'irrecv'
  //vairable uses to store the last decodedRawData
  uint32_t last_decodedRawData = 0;
  /*-----( Function )-----*/
  void translateIR(char * key) // takes action based on IR code received
  {
    // Check if it is a repeat IR code 
    if (irrecv.decodedIRData.flags)
    {
      //set the current decodedRawData to the last decodedRawData 
      irrecv.decodedIRData.decodedRawData = last_decodedRawData;
      Serial.println("REPEAT!");
    } else
    {
      //output the IR code on the serial monitor
      Serial.print("IR code:0x");
      Serial.println(irrecv.decodedIRData.decodedRawData, HEX);
    }
    //map the IR code to the remote key
    switch (irrecv.decodedIRData.decodedRawData)
    {
      case 0xBA45FF00: Serial.println("POWER"); break;
      case 0xB847FF00: Serial.println("FUNC/STOP"); break;
      case 0xB946FF00: Serial.println("VOL+"); break;
      case 0xBB44FF00: Serial.println("FAST BACK");    break;
      case 0xBF40FF00: Serial.println("PAUSE");    break;
      case 0xBC43FF00: Serial.println("FAST FORWARD");   break;
      case 0xF807FF00: Serial.println("DOWN"); key = "*"   break;
      case 0xEA15FF00: Serial.println("VOL-");    break;
      case 0xF609FF00: Serial.println("UP"); key = '#'   break;
      case 0xE619FF00: Serial.println("EQ");    break;
      case 0xF20DFF00: Serial.println("ST/REPT");    break;
      case 0xE916FF00: Serial.println("0"); key = '0';    break;
      case 0xF30CFF00: Serial.println("1"); key = '1'   break;
      case 0xE718FF00: Serial.println("2"); key = '2'   break;
      case 0xA15EFF00: Serial.println("3"); key = '3'   break;
      case 0xF708FF00: Serial.println("4"); key = '4'   break;
      case 0xE31CFF00: Serial.println("5"); key = '5'   break;
      case 0xA55AFF00: Serial.println("6"); key = '6'   break;
      case 0xBD42FF00: Serial.println("7"); key = '7'   break;
      case 0xAD52FF00: Serial.println("8"); key = '8'   break;
      case 0xB54AFF00: Serial.println("9"); key = '9'   break;
      default:
        Serial.println(" other button   ");
    }// End Case
    //store the last decodedRawData
    last_decodedRawData = irrecv.decodedIRData.decodedRawData;
    delay(500); // Do not get immediate repeat
  } //END translateIR

#include <SPI.h>
  #include <MFRC522.h>

  #define RST_PIN   5     
  #define SS_PIN    53   

  MFRC522 mfrc522(SS_PIN, RST_PIN); 

  /* Set your new UID here! */
  #define NEW_UID {0xDE, 0xAD, 0xBE, 0xEF}

  MFRC522::MIFARE_Key key;

 

void setup()
{
  // Serial
    Serial.begin( 9600 );

  // lcd
    // set up the LCD's number of columns and rows:
    lcd.begin( 16, 2 );
    // Print a message to the LCD.
    lcd.print( "Hello, World!" );
  // remote
    Serial.println( "IR Receiver Button Decode" );
    irrecv.enableIRIn(); // Start the receiver
  // rfid
    while ( !Serial );     // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
    SPI.begin();         // Init SPI bus
    mfrc522.PCD_Init();  // Init MFRC522 card
    Serial.println( F( "Warning: this example overwrites the UID of your UID changeable card, use with care!" ));

    // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
    for ( byte i = 0; i < 6; i++ ) 
    {
      key.keyByte[ i ] = 0xFF;
    }
  //servo
    myservo.attach(9);
    myservo.write(90);// move servos to center position -> 90°
}
  
void loop()
{
  // passkey vars
    char key = customKeypad.getKey(); //populate key with keypad entry if present
    char key = (key)? key: irrecv.decode(); 
    char [] string = [17];
    int string_pointer = 0;

  // remote populate 
    if ( irrecv.decode() ) // have we received an IR signal?
    {
      translateIR(&key); // override key with IR entry if present
      irrecv.resume(); // receive the next value
    } 

  // keypad 
    if (key)
    {
      if(string_pointer < 14)
        string[string_pointer++] = key;
      else 
        string_pointer = 0;
        string[string_pointer++] = key
    }


  // lcd
    // set the cursor to column 0, line 1
    // (note: line 1 is the second row, since counting begins with 0):
    lcd.setCursor(0, 1);
    // print the number of seconds since reset:
    lcd.print( string );

/*
  // rfid
      // Look for new cards, and select one if present
    if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) 
    {
      delay( 50 );
      return;
    }

    // Now a card is selected. The UID and SAK is in mfrc522.uid.

    // Dump UID
    Serial.print( F( "Card UID:" ));
    for ( byte i = 0; i < mfrc522.uid.size; i++ ) 
    {
      Serial.print( mfrc522.uid.uidByte[ i ] < 0x10 ? " 0" : " " );
      Serial.print( mfrc522.uid.uidByte[ i ], HEX );
    } 
    Serial.println();

    byte newUid[] = NEW_UID;
    if ( mfrc522.MIFARE_SetUid( newUid, (byte)4, true ) )
    {
      Serial.println( F( "Wrote new UID to card." ) );
    }

    // Halt PICC and re-select it so DumpToSerial doesn't get confused
    mfrc522.PICC_HaltA();
    if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) 
    {
      return;
    }

    // Dump the new memory contents
    Serial.println( F( "New UID and contents:" ) );
    mfrc522.PICC_DumpToSerial( &( mfrc522.uid ) );

    delay( 2000 );
  // servo
    myservo.write( 90 );// move servos to center position -> 90°
    delay( 500 );
    myservo.write(30);// move servos to center position -> 60°
    delay(500);
    myservo.write(90);// move servos to center position -> 90°
    delay(500);
    myservo.write(150);// move servos to center position -> 120°
    delay(500);
*/
}




