#include <IRremote.h>
#include <LiquidCrystal_I2C.h>
#include <DS1302.h>
#include <Wire.h> 
#include <EEPROM.h>
#include <Servo.h>


int RECV_PIN = 9;

IRrecv irrecv(RECV_PIN);
Servo myservo;

decode_results  results;
 
String  SystemStatus ="";
String  Function="";
String  TimeValue="";


DS1302 rtc(6,5, 4);
 
LiquidCrystal_I2C lcd(0x27,16,2); 

int led1 = 3;
int feedTimeSetupNumber=1;

int potpin = A0;  
int val;    

void setup()
{
  
  rtc.halt(false);
  rtc.writeProtect(false);
  irrecv.enableIRIn(); 
  Serial.begin(9600); 
  
  //myservo.attach(9);
  if (EEPROM.read(0)==0){ //Set Empty values  if not set any value of time
   
      writeStringToEEPROM(1,"00:0000:0000:0000:00");
  }
 
   
  pinMode(led1, OUTPUT);
  SystemStatus ="Close";
  lcd.init();  
  // turn off backlight
  lcd.noBacklight();
  
}


void loop() {
  if (SystemStatus=="Close")
  {
      while(true)
      {
        
            if (irrecv.decode(&results))  {
                String keyValue=GetKeyValue(results.value);
                if (keyValue=="Close")
                {   Serial.println("OpenSystem");
                    OpenSystem();
                     delay(1000);
                     irrecv.resume();
                    break;
                }
                delay(1000);
                irrecv.resume();
            }
        } 
  }

   if (irrecv.decode(&results))  {
      String keyValue=GetKeyValue(results.value);
   
     if ( keyValue== "Close")
      { 
          Function="Close";
      }
      
      if (keyValue == "Menu")
      { 
          Function="Menu"; 
      }


      if (Function == "Close" && SystemStatus=="Open" )
      {
        Serial.println("CloseSystem");
        CloseSystem();
       // delay(100);
      }else if (Function=="Menu")
      {   Serial.println("  ReadKeysOnMenu");
          irrecv.resume();
          ReadKeysOnMenu(); 
           
      }
       Serial.println("Okunan Değer ");
      Serial.print(results.value,HEX);   
      
      irrecv.resume(); 
  }else{
    if (SystemStatus=="Open")
    {
       StandartViewOfScreen();
    }
  }


  Serial.println(SystemStatus);


}
 

void ReadKeysOnMenu()
{
  
  ShowMenu();
  while(true)
  {
   
    if (irrecv.decode(&results))  {
        String keyValue=GetKeyValue(results.value);
        
 
       // Serial.println("Rrad Key " +keyValue);
       
        if (keyValue=="1")
        {  irrecv.resume();
           delay(400);
               
           ReadKeysOnTimeMenu();
           ShowMenu();
        }else if (keyValue=="2")
        { irrecv.resume();
           delay(400);
           ReadKeysonPlanMenu();
           ShowMenu();
        }
        else if (keyValue=="Del")
        {
          Serial.println("Return Main");
          irrecv.resume();
          return;
        } 
    } 
  
     delay(30);
  }
 
}

void RunStep(){

  myservo.write(60);

}
void ShowFeedTime(){
 
  lcd.clear() ;
  lcd.setCursor(1,0);

  lcd.print("Setup Schedule");

  lcd.setCursor(1,1);

  lcd.print("Set Number");

}
void ReadKeysonPlanMenu(){
  ShowFeedTime();
  while(true)
  {
   
    if (irrecv.decode(&results))  {
        String keyValue=GetKeyValue(results.value);
       
          if (keyValue=="Menu" || keyValue=="Del") 
          {    
              delay(300);
              irrecv.resume();
              return;
          }else if (keyValue=="1" ||keyValue=="2" ||keyValue=="3")
          { 
            
              delay(300);
            feedTimeSetupNumber=keyValue.toInt();
             irrecv.resume();
            ReadSetupFeedTime();
          }else
          {
              
              delay(300);
              irrecv.resume();
          }
         
    }

  }
}

void SaveFeadTimeToEprom(){
      int startNumber=(feedTimeSetupNumber*5)-4;
      writeStringToEEPROM(startNumber,TimeValue);

}
void WriteScreenNumber()
{
  
    lcd.clear() ;
  lcd.setCursor(1,0);

  lcd.print("Schedule "+String( feedTimeSetupNumber));
  lcd.setCursor(1,1);
  lcd.print(TimeValue);

}
void SetupFeadTimeNumber(){
  

  int endNumber=feedTimeSetupNumber*5;
 
   delay(100); 
   TimeValue= readStringFromEEPROM(1,endNumber);
   TimeValue="";
   /*if (TimeValue=="00:00")
   {
      TimeValue="";
   }*/
   Serial.println(TimeValue);
   WriteScreenNumber();
    
  while(true)
  {
        if (irrecv.decode(&results))  {
          
          String keyValue=GetKeyValue(results.value);
       
          Serial.println("Rrad Key " +keyValue+" Time " +TimeValue);
        
          if (keyValue=="Menu") 
          {
            if (TimeValue.length()==5)
            {
                Serial.println("SaveFeadTimeToEprom");
                SaveFeadTimeToEprom(); 
                delay(100); 
                irrecv.resume(); 
                return;
            }else
            {
               Serial.println("Return");
               delay(100); 
               irrecv.resume(); 
               return;
            } 
          }
          
          if (keyValue == "Del")
          {
              if (TimeValue.length()==1)
              {
                irrecv.resume(); 
                return;
              }
              int to= TimeValue.length()-1;

              TimeValue=  TimeValue.substring(0, to);
              WriteScreenNumber();
              delay(100); 
              irrecv.resume(); 
          } 
          else if (keyValue == "1" || keyValue == "2" || keyValue == "3"  || keyValue == "4" || keyValue == "5" || keyValue == "6" || keyValue == "7" || keyValue == "8" || keyValue == "9" || keyValue == "0"){
              TimeValue=TimeValue+keyValue;
              if (TimeValue.length()==2 || TimeValue.length()==4)
              {
                TimeValue=TimeValue+":";
              }
              if (TimeValue.length()<6)
              {
                WriteScreenNumber();               
              }
               delay(100); 
                   
              irrecv.resume(); 
          }
           irrecv.resume();   
      }
  }

}

void ReadSetupFeedTime()
{
  ShowFeedTime();
  delay(3000);
  while(true)
  {
   
    if (irrecv.decode(&results))  {
        String keyValue=GetKeyValue(results.value);
       
        if (keyValue=="Del" || keyValue=="Menu") //Return Main Menu 
        {    
            irrecv.resume();
            return;
        } 

        if (keyValue=="1" || keyValue=="2" || keyValue=="3" || keyValue=="4" || keyValue=="5"){

         feedTimeSetupNumber=keyValue.toInt();
         
         irrecv.resume();
         Serial.println(feedTimeSetupNumber);
         delay(200);
        
         SetupFeadTimeNumber();
         
         

        } 
    }
 
  }
}

void ReadKeysOnTimeMenu(){

  SetTime(); 
  TimeValue="";
  while(true)
  {
 
      if (irrecv.decode(&results))  {
          
          String keyValue=GetKeyValue(results.value);
          irrecv.resume();
          Serial.println("Rrad Key " +keyValue+" Time " +TimeValue);
        
          if (keyValue=="Menu") 
          {
            if (TimeValue.length()>=5)
            {
                Serial.println("TimeToRTC");
                TimeToRTC(); 
                return;
            }else
            {
               Serial.println("Return");
        
            } 
            return;
          }
          
          if (keyValue == "Del")
          {
              if (TimeValue.length()==1)
              {
                return;
              }
              int to= TimeValue.length()-1;

              TimeValue=  TimeValue.substring(0, to);
              WriteTimeToScreen();

          }  else if (keyValue != "None" ){
              TimeValue=TimeValue+keyValue;
              if (TimeValue.length()==2 || TimeValue.length()==5)
              {
                TimeValue=TimeValue+":";
              }
              if (TimeValue.length()<6)
              {
                delay(100);
                WriteTimeToScreen();
              } 
          } 
          
          irrecv.resume(); 
      }
  }
}

void TimeToRTC(){
  
  Serial.println(TimeValue);
  //writeStringToEEPROM(1,TimeValue);
 int hour= TimeValue.substring(0, 2).toInt();
 

 int minutes = TimeValue.substring(3, 5).toInt();
 
  
 int second = 0;
 
    EEPROM.write(0,1); 
   
    rtc.setTime(hour,minutes, second);    
 
    lcd.clear() ;
    lcd.setCursor(1,0); 
    lcd.print("Time Saved ...");

    delay(5000);
 
}

void StandartViewOfScreen()
{
  

    String timeStr=rtc.getTimeStr();
    Serial.println(timeStr);
    lcd.clear() ;
    lcd.setCursor(1,0); 

    lcd.print("Time " +timeStr );
    
    delay (1000);
    
}

void ClearSystemValues()
{
 
   Function="";
   TimeValue="";
}

 
void SetTime(){
  lcd.backlight(); 
  lcd.clear() ;
  lcd.setCursor(1,0);

  lcd.print("Set Time ");

  lcd.setCursor(1,1);
}

void WriteTimeToScreen(){
  
    lcd.backlight(); 
    lcd.clear() ;
    lcd.setCursor(1,0);

    lcd.print("Set Time ");

    
    lcd.setCursor(1,1);
    lcd.print(TimeValue);
}

void ShowMenu(){
   lcd.backlight(); 
  lcd.clear() ;
  lcd.setCursor(1,0);

  lcd.print("1 Set Date Time");
  
  lcd.setCursor(1,1);
  lcd.print("2 Feed Times");
  
}

 
void OpenSystem(){
      digitalWrite(led1,HIGH);
      SystemStatus="Open";

      lcd.backlight();  
      int value1 = EEPROM.read(0); 
      if ( value1==0)
      {  Serial.println("Show Menu");
          SystemStatus="Menu";
          Function="Menu";
          ReadKeysOnMenu();
      }
}

void CloseSystem(){
  
  digitalWrite(led1,LOW);
  SystemStatus="Close";
  Function="";
  lcd.noBacklight(); // turn off backlight
  lcd.clear();

           
    delay(3000);
} 

void writeStringToEEPROM(int addrOffset, const String &strToWrite)
{
  byte len = strToWrite.length();
  //EEPROM.write(addrOffset, len);
  for (int i = 0; i < len; i++)
  {
    EEPROM.write(addrOffset  + i, strToWrite[i]);
  }
}
String readStringFromEEPROM(int addrOffset,int length)
{
   
  char data[length];
  for (int i = addrOffset; i <= length; i++)
  {// data[i] = EEPROM.read(addrOffset + 1 + i);
    char t=EEPROM.read(i);
    Serial.println( t);
  //  delay(2000);
    data[i] = t;
     
  }
  data[length] = '\ 0'; // !!! NOTE !!! Remove the space between the slash "/" and "0" (I've added a space because otherwise there is a display bug)
  return String(data);
}


String GetKeyValue(uint32_t keyvalue){
  String returnV="None";

  if (  keyvalue == 0x1 || keyvalue == 0x10001)
  {
    returnV="1";

  }
  else if (  keyvalue == 0x2 || keyvalue == 0x10002) {
    returnV="2";

  }
  else if ( keyvalue == 0x3)
  {
      returnV="3"; 

  } else if (  keyvalue == 0x4)
  {
    returnV="4"; 

  } else if (  keyvalue == 0x5)
  {
    returnV="5"; 

  } else if (  keyvalue == 0x6)
  {
    returnV="6"; 

  } else if (  keyvalue == 0x7)
  {
    returnV="7"; 

  } else if ( keyvalue == 0x8)
  {
    returnV="8"; 

  } else if (  keyvalue == 0x9)
  {
    returnV="9"; 

  } else if ( keyvalue == 0x0)
  {
    returnV="0"; 

  } else if (  keyvalue == 0xA ||  keyvalue == 0x1000A)
  {
    returnV="Del"; 
  } else if (  keyvalue == 0xC || keyvalue==0x1000C)
  {  
        returnV="Close";
  }else if ( keyvalue==0x1005C || keyvalue == 0x5C)
  { 
      returnV="Menu";
  }else {
         Serial.println(keyvalue,HEX);  
  }
    return returnV;

}
 