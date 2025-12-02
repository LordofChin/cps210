//global vars
  char custom_key;          // key entered by user
  int locked;               // "bool" to check lock state
  int string_pointer = 1;   // pointer for used-defined password
  char string[17] = {};     // user-defined password
  char msg[17] = {};        // variable for the current display message

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

#include <IRremote.h>
  int receiver = 5; // Signal Pin of IR receiver to Arduino Digital Pin 5

  /*-----( Declare objects )-----*/
  IRrecv irrecv(receiver);     // create instance of 'irrecv'
  //vairable uses to store the last decodedRawData
  uint32_t last_decodedRawData = 0;
  /*-----( Function )-----*/
  void translateIR() // takes action based on IR code received
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
      case 0xBF40FF00: Serial.println("PAUSE"); custom_key = 'T'; break;
      case 0xBC43FF00: Serial.println("FAST FORWARD");   break;
      case 0xF807FF00: Serial.println("DOWN"); custom_key = '*';   break;
      case 0xEA15FF00: Serial.println("VOL-");    break;
      case 0xF609FF00: Serial.println("UP"); custom_key = '#';   break;
      case 0xE619FF00: Serial.println("EQ");    break;
      case 0xF20DFF00: Serial.println("ST/REPT");    break;
      case 0xE916FF00: Serial.println("0"); custom_key = '0';    break;
      case 0xF30CFF00: Serial.println("1"); custom_key = '1';   break;
      case 0xE718FF00: Serial.println("2"); custom_key = '2';   break;
      case 0xA15EFF00: Serial.println("3"); custom_key = '3';   break;
      case 0xF708FF00: Serial.println("4"); custom_key = '4';   break;
      case 0xE31CFF00: Serial.println("5"); custom_key = '5';   break;
      case 0xA55AFF00: Serial.println("6"); custom_key = '6';   break;
      case 0xBD42FF00: Serial.println("7"); custom_key = '7';   break;
      case 0xAD52FF00: Serial.println("8"); custom_key = '8';   break;
      case 0xB54AFF00: Serial.println("9"); custom_key = '9';   break;
      default:
        Serial.println(" other button   ");
    }// End Case
    //store the last decodedRawData
    last_decodedRawData = irrecv.decodedIRData.decodedRawData;
    delay(500); // Do not get immediate repeat
  } //END translateIR

// temporary msgs
  char * temp_disp( char * temp_msg ) 
  {
    // clear display
    lcd.clear();

    //display temp_msg
    lcd.setCursor( 0, 0 );
    strcpy( msg, temp_msg );
    lcd.print( msg );
    delay( 700 );

    // reset to default behaviour
    lcd.setCursor( 0, 0 );
    strcpy( msg, "Enter <= 6d code");
    lcd.print( msg );
    lcd.setCursor( 1, 1 );

    //return msg for Serial.print handling
    return temp_msg;
  }
// locking
  char* lock()
  {
    myservo.write(180);
    temp_disp("locking...");
    locked = 1;
    return "locking...";
  }

  char* unlock()
  {
    myservo.write(0);
    temp_disp("unlocking...");
    locked = 0;
    return "unlocking...";
  }

void setup()
{
  // serial
    Serial.begin( 9600 );
  // lcd
    // set up the LCD's number of columns and rows:
    lcd.begin( 16, 2 );
    // generate boiler plate for user-defined password string
    strcpy( string, "^$" );
    // Print the default message to the LCD.
    strcpy( msg, "Enter <= 6d code" );
    lcd.print( msg );
  // remote
    Serial.println( "IR Receiver Button Decode" );
    irrecv.enableIRIn(); // Start the receiver

  // servo
    myservo.attach( 6 ); // use pin 6 for servo
    lock();// move servo to center position -> 0°. ie. the locked pos.

}
//password handling
  char *password = "^022305$"; // gf's brithday

  int check_pass( char *pass )
  {
      for( int i = 0; password[i] != '\0'; i++ )
          if( pass[i] != password[i] ) return 0; // characterwise comparison of passwords
      return 1;
  }

void loop()
{
  // keypad
    custom_key = customKeypad.getKey(); //populate key with keypad entry if present

  // remote
    if ( irrecv.decode() ) // have we received an IR signal?
    {
      Serial.println("Detecting IR Signal");
      translateIR(); // override key with IR entry if present
      irrecv.resume(); // receive the next value
    } 

  // handle cutom_key 
    if ( custom_key )
    {

      if( custom_key == '#' ) // password submission
        Serial.println( ( check_pass( string ) ) ? unlock() : temp_disp("Wrong code") );
      else if ( custom_key == 'T') // toggle lock (pause on remote)
        (locked) ? unlock() : lock(); 
      else if( custom_key == 'A' ) // lock
        lock();
      else if( string_pointer > 6 || custom_key == '*' ) // reset the user-defined password
      {  
        string_pointer = 1;
        strcpy(string, "^$");
        temp_disp("clearing keypad");
      }
      if (custom_key <= '9' && custom_key >= '0') // handle password characters
      {
        string[string_pointer++] = custom_key;
        string[string_pointer] = '$'; 
        string[string_pointer + 1] = NULL;
        Serial.println(string);
      }
    }

  // lcd
    lcd.setCursor( 1, 1 )
    lcd.print( string );
}