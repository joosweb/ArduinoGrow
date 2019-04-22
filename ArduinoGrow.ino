#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include "DHT.h"
#include <virtuabotixRTC.h>
#include <PinButton.h>


LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); //Configuracion del LCD I2C (puede ser necesario cambiar el primer valor con la direccion del LCD)

// DHT 11

#define DHTPIN 13 //Data
#define DHTTYPE DHT11 

// define pins of RTC module

#define rtcSCLK     A3
#define rtcIO       A2
#define rtcCE       A1

#define relay1      5
#define relay2      6
#define relay3      7
#define relay4      8

virtuabotixRTC myRTC(rtcSCLK, rtcIO, rtcCE);

DHT dht(DHTPIN, DHTTYPE);

 byte LT[8] = 
{

  B00111,
  B01111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};
byte UB[8] =
{
  B11111,
  B11111,
  B11111,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};
byte RT[8] =
{

  B11100,
  B11110,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};
byte LL[8] =
{

  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B01111,
  B00111
};
byte LB[8] =
{
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
  B11111,
  B11111
};
byte LR[8] =
{

  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11110,
  B11100
};
byte MB[8] =
{
  B11111,
  B11111,
  B11111,
  B00000,
  B00000,
  B00000,
  B11111,
  B11111
};
byte block[8] =
{
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};


/**
 *  MACROS, CONSTANTES, ENUMERADORES, ESTRUCTURAS Y VARIABLES GLOBALES
 */

PinButton myButton(2);

int LuzFondo =0;

int maxi = -100;
int mini = 100; // to start max/min temperature

int hmaxi = -100;
int hmini =  100; 
 
#define COUNT(x) sizeof(x)/sizeof(*x)                   // Macro para contar el numero de elementos de un array
const byte pENCO_SW   = 2;                              // Pin encoder SW
const byte pENCO_DT   = 3;                              // Pin encoder DT
const byte pENCO_CLK  = 4;                              // Pin encoder CLK
const byte rowsLCD    = 4;                              // Filas del LCD
const byte columnsLCD = 20;                             // Columnas del LCD

enum Button{ Unknown, Ok, Left, Right } btnPressed;     // Enumerador con los diferentes botones disponibles
enum Screen{ Menu1, Menu2, Menu3, Menu4, Flag, Number, NumberYear };              // Enumerador con los distintos tipos de submenus disponibles

const char *txMENU[] = {                                // Los textos del menu principal, la longitud maxima = columnsLCD-1, rellenar caracteres sobrantes con espacios.
    "OSAN - GROW 1.0     ",
    "--------------------",
    "PANTALLA            ",
    "CONF. PINES         ",
    "VER HORARIO PINES   ",
    "FECHA & HORA        ",
    "UD. DE TEMPERATURA  ",
    "TEMP. MIN & MAX     ",
    "HUMEDAD MIN & MAX   ",
    "TEMPERATURA IDEAL   ",
    "TEMPERATURA COOLER  ",
    "TEMPERATURA HEATER  ",
    "GUARDAR Y SALIR     ",
    "SALIR               "
};
const byte iMENU = COUNT(txMENU);                       // Numero de items/opciones del menu principal

enum eSMENU1{ Milliseconds, Seconds, Minutes, Hours };  // Enumerador de las opciones disponibles del submenu 1 (tienen que seguir el mismo orden que los textos)
const char *txSMENU1[] = {                              // Textos del submenu 1, longitud maxima = columnsLCD-2, rellenar caracteres sobrantes con espacios
    "   Milisegundos   ",
    "     Segundos     ",
    "     Minutos      ",
    "      Horas       "
};

enum eSMENU2{ GradeC, GradeF };                         // Enumerador de las opciones disponibles del submenu 2 (tienen que seguir el mismo orden que los textos)
const char *txSMENU2[] = {                              // Textos del submenu 1, longitud maxima = columnsLCD-2, rellenar caracteres sobrantes con espacios
    "     Grados C     ",
    "     Grados F     "
};

enum eSMENU3{ HoursScreen, TempScreen };                         // Enumerador de las opciones disponibles del submenu 2 (tienen que seguir el mismo orden que los textos)
const char *txSMENU3[] = {                              // Textos del submenu 1, longitud maxima = columnsLCD-2, rellenar caracteres sobrantes con espacios
    "  Destacar Reloj  ",
    "  Destacar T & H  "
};

enum eSMENU4{ On, Off };                         // Enumerador de las opciones disponibles del submenu 2 (tienen que seguir el mismo orden que los textos)
const char *txSMENU4[] = {                              // Textos del submenu 1, longitud maxima = columnsLCD-2, rellenar caracteres sobrantes con espacios
    "     Encender     ",
    "      Apagar      "
};

/* ESTRUCTURAS CONFIGURACION */
struct MYDATA{      // Estructura STRUCT con las variables que almacenaran los datos que se guardaran en la memoria EEPROM
    int initialized;
    int time_show;
    int time_unit;
    int time_x;
    int time_y;
    int temp_unit;
    int temp_ideal;
    int temp_cooler;
    int temp_heater;
    int temp_x;
    int temp_y;
    int time_hours;
    int time_minutes;
    int time_seconds;
    int time_day;
    int time_month;
    int time_year;
    int type_screen;
    int pin_1_on_h;
    int pin_1_on_m; 
    int pin_1_off_h;
    int pin_1_off_m;  
    int pin_2_on_h;
    int pin_2_on_m; 
    int pin_2_off_h;
    int pin_2_off_m;
    int luz_fondo;    
};
union MEMORY{                                           // Estructura UNION para facilitar la lectura y escritura en la EEPROM de la estructura STRUCT
    MYDATA d;
    byte b[sizeof(MYDATA)];
}
memory;


/**
 * INICIO Y CONFIGURACION DEL PROGRAMA
 */
void setup()
{
    pinMode(pENCO_SW,  INPUT_PULLUP);
    pinMode(pENCO_DT,  INPUT_PULLUP);
    pinMode(pENCO_CLK, INPUT_PULLUP);

    pinMode(rtcSCLK, OUTPUT);
    pinMode(rtcIO,   OUTPUT);
    pinMode(rtcCE,   OUTPUT);

    pinMode(relay1, OUTPUT);
    pinMode(relay2, OUTPUT);
    pinMode(relay3, OUTPUT);
    pinMode(relay4, OUTPUT);

    digitalWrite(relay1, HIGH);
    digitalWrite(relay2, HIGH);
    digitalWrite(relay3, HIGH);
    digitalWrite(relay4, HIGH);

    // Carga la configuracion de la EEPROM, y la configura la primera vez:
    
    readConfiguration();

    // Inicia el LCD:
    dht.begin();
    lcd.begin(columnsLCD, rowsLCD);
    lcd.createChar(0,LT);
    lcd.createChar(1,UB);
    lcd.createChar(2,RT);
    lcd.createChar(3,LL);
    lcd.createChar(4,LB);
    lcd.createChar(5,LR);
    lcd.createChar(6,MB);
    lcd.createChar(7,block);
 
    // Imprime la informacion del proyecto:
    lcd.setCursor(0,0); lcd.print("  OSAN - GROW 1.0 ");
    lcd.setCursor(0,2); lcd.print("                   ");
    lcd.setCursor(4,2);

    char url[11] = "www.osan.cl";
    
    for( int i=0 ; i<11 ; i++ )
    {
        lcd.print(url[i]);
        delay(300);
    }
    
    delay(2000);
    lcd.clear();
}


/**
 * PROGRAMA PRINCIPAL
 */
void loop()
{
    
    myButton.update();

    if (myButton.isSingleClick()) {       
        btnPressed = readButtons();
        if( btnPressed == Button::Ok )
        lcd.backlight();
        LuzFondo=0; 
        openMenu();      
    }

    if (myButton.isLongClick()) {
          if(LuzFondo==0) {
              lcd.noBacklight();
              LuzFondo=1;      
            }
            else if(LuzFondo==1) {
              lcd.backlight();
              LuzFondo=0;      
         }       
    }

    myRTC.updateTime();
    
    relayAction(36, 40, 16, relay1);
    relayAction(44, 48, 17, relay2);

    int h = dht.readHumidity();
    int t = dht.readTemperature();

    if(t > maxi) {maxi = t;} // set max temperature
    if(t < mini) {mini = t;} // set min temperature

    if(h > hmaxi) {hmaxi = h;} // set max humidity
    if(h < hmini) {hmini = h;} // set min humidity

    if(t >= memory.d.temp_cooler ) {
         digitalWrite(relay3, LOW);
    }
    if(t <= memory.d.temp_heater ) {
         digitalWrite(relay4, LOW);
    }
    if(t == memory.d.temp_ideal ) {
         digitalWrite(relay3, HIGH);
         digitalWrite(relay4, HIGH);
    }
     if(memory.d.type_screen == eSMENU3::HoursScreen)
    {
      if(myRTC.hours <= 9) { 
        lcd.setCursor(0,0);
        lcd.print("   ");
        lcd.setCursor(0,1);
        lcd.print("   ");
        printNumber(myRTC.hours, 4);
      }
      else
      {
        printNumber(myRTC.hours, 0);
      }
      lcd.setCursor(8,0);      
      lcd.write(4);
      lcd.setCursor(8,1);      
      lcd.write(4);
      if(myRTC.minutes <= 9) { 
        printNumber(0, 10);
        printNumber(myRTC.minutes, 14);
      }
      else
      {
        printNumber(myRTC.minutes, 10);
      }
      
      lcd.setCursor(18,0);
      if(myRTC.seconds <= 9) {
        lcd.print("0");
        lcd.print(myRTC.seconds);
      }
      else {
        lcd.print(myRTC.seconds);
      }
      lcd.setCursor(18,1);
      if(myRTC.hours >= 1 && myRTC.hours <= 11) {
        lcd.print("AM");
      }
      else
      {
        lcd.print("PM");
      }
            
      lcd.setCursor(0,2);
      lcd.print("--------------------");
      lcd.setCursor(0,3);
      lcd.print("T");
      if(memory.d.temp_unit == eSMENU2::GradeC){
        lcd.print(t);
      }
      else
      {
       lcd.print((int)round(1.8*t+32));
      }
      lcd.print(char(223));
      lcd.print("H");
      lcd.print(h);
      lcd.print("%");
      lcd.print(" ");
      print2digits(myRTC.dayofmonth);
      lcd.print("/");
      print2digits(myRTC.month);
      lcd.print("/");
      print2digits(myRTC.year);
    }
    else 
    {
     // Pinta la pantalla principal cada 1 segundo:
          switch( memory.d.temp_unit )
            {
                case eSMENU2::GradeC:
                    lcd.setCursor(0,2);
                    lcd.print("-------------------");  
                    lcd.setCursor(0,0);           
                    lcd.print("T");
                    lcd.print(char(223));
                    lcd.setCursor(0,1);
                    lcd.print("C");           
                    printNumber(t,2);
                    lcd.setCursor(10,0); 
                    lcd.print("H");
                    lcd.setCursor(10,1); 
                    lcd.print("%");  
                    lcd.setCursor(12,0);       
                    printNumber(h,12);
                    lcd.setCursor(0,3);
                    print2digits(myRTC.hours);
                    lcd.print(":");
                    print2digits(myRTC.minutes);
                    lcd.print(":");
                    print2digits(myRTC.seconds);
                    lcd.print(" ");
                    print2digits(myRTC.dayofmonth);
                    lcd.print("/");
                    print2digits(myRTC.month);
                    lcd.print("/");
                    print2digits(myRTC.year);
                    break;
                case eSMENU2::GradeF:
                    lcd.setCursor(0,2);
                    lcd.print("-------------------");  
                    lcd.setCursor(0,0);           
                    lcd.print("T");
                    lcd.print(char(223));
                    lcd.setCursor(0,1);
                    lcd.print("F");           
                    printNumber((int)round(1.8*t+32),2);
                    lcd.setCursor(10,0); 
                    lcd.print("H");
                    lcd.setCursor(10,1); 
                    lcd.print("%");  
                    lcd.setCursor(12,0);       
                    printNumber(h,12);
                    lcd.setCursor(0,3);
                    print2digits(myRTC.hours);
                    lcd.print(":");
                    print2digits(myRTC.minutes);
                    lcd.print(":");
                    print2digits(myRTC.seconds);
                    lcd.print(" ");
                    print2digits(myRTC.dayofmonth);
                    lcd.print("/");
                    print2digits(myRTC.month);
                    lcd.print("/");
                    print2digits(myRTC.year);
                    break;
        }         
    }
}


void relayAction(int adrON, int adrOF, int pos, int pin) {

  int MinToday = (myRTC.hours * 60) + myRTC.minutes;
  int MinEprON = (EEPROM.read(adrON) * 60) + EEPROM.read(adrON + 2);
  int MinEprOF = (EEPROM.read(adrOF) * 60) + EEPROM.read(adrOF + 2);


  if(MinEprON < MinEprOF){
           if(MinToday >=MinEprON && MinToday < MinEprOF ){             //Start
           digitalWrite(pin, LOW);
           }
           else if(MinToday >= MinEprOF) {
           digitalWrite(pin, HIGH);
           }
           else{
           digitalWrite(pin, HIGH);
          }
    }
    if (MinEprON > MinEprOF){
    
            if(MinToday >= MinEprON && myRTC.hours <= 23){                  //Start
            digitalWrite(pin, LOW);  
            }
            else if(MinToday < MinEprOF){
            digitalWrite(pin, LOW);
            }
            else if(MinToday >= MinEprOF && MinToday < MinEprON){
            digitalWrite(pin, HIGH);  
            }
    }
}

/**
 *  MUESTRA EL MENU PRINCIPAL EN EL LCD.
 */
 
void openMenu()
{
    byte idxMenu       = 2;
    boolean exitMenu   = false;
    boolean forcePrint = true;


    lcd.clear();

    while( !exitMenu )
    {
        btnPressed = readButtons();

        if( btnPressed == Button::Left && idxMenu-1 >= 0 )
        {
            idxMenu--;
        }
        else if( btnPressed == Button::Right && idxMenu+1 < iMENU )
        {
            idxMenu++;
        }
        else if( btnPressed == Button::Ok )
        {
            switch( idxMenu )
            {
                case 2: openSubMenu( idxMenu, Screen::Menu3,  &memory.d.type_screen, 0, COUNT(txSMENU3)-1 ); break;
                case 3: ConfigPines(); break;
                case 4: VerPines(); break;
                case 5: ConfigTime(); break;
                case 6: openSubMenu( idxMenu, Screen::Menu2,  &memory.d.temp_unit, 0, COUNT(txSMENU2)-1 ); break;
                case 7: MaxiMin(); break;
                case 8: MaxiMinh(); break;
                case 9: openSubMenu2( "TEMPERATURA IDEAL", Screen::Number, &memory.d.temp_ideal, 0, 80); break;
                case 10: openSubMenu2( "TEMPERATURA COOLER", Screen::Number, &memory.d.temp_cooler, 0, 80); break;
                case 11: openSubMenu2( "TEMPERATURA HEATER", Screen::Number, &memory.d.temp_heater, 0, 80); break;
                case 12: writeConfiguration(); exitMenu = true; break; //Salir y guardar
                case 13: readConfiguration();  exitMenu = true; break; //Salir y cancelar cambios
            }
            
            forcePrint = true;
        }


        if( !exitMenu && (forcePrint || btnPressed != Button::Unknown) )
        {
            forcePrint = false;

            static const byte endFor1 = (iMENU+rowsLCD-1)/rowsLCD;
            int graphMenu     = 0;

            for( int i=1 ; i<=endFor1 ; i++ )
            {
                if( idxMenu < i*rowsLCD )
                {
                    graphMenu = (i-1) * rowsLCD;
                    break;
                }
            }

            byte endFor2 = graphMenu+rowsLCD;

            for( int i=graphMenu, j=0; i< endFor2 ; i++, j++ )
            {
                lcd.setCursor(1, j);
                lcd.print( (i<iMENU) ? txMENU[i] : "                    " );
            }

            for( int i=0 ; i<rowsLCD ; i++ )
            {
                lcd.setCursor(0, i);
                lcd.print(" ");
            }

            if(idxMenu != 0 && idxMenu != 1){
              lcd.setCursor(0, idxMenu % rowsLCD );
              lcd.print(char(126));
            }
            else {
              idxMenu  = 2;
              lcd.setCursor(0, idxMenu % rowsLCD );
              lcd.print(char(126));
              }
         }
    }

    lcd.clear();
}


/**
 * MUESTRA EL SUBMENU EN EL LCD.
 *
 * @param menuID    ID del menu principal para usarlo como titulo del submenu
 * @param screen    Segun el tipo, se representara el submenu de una forma u otra.
 * @param value     Puntero a la variable que almacena el dato, y que se modificara.
 * @param minValue  Valor minimo que puede tener la variable.
 * @param maxValue  Valor maximo que puede tener la variable.
 */
 
void openSubMenu( byte menuID, Screen screen, int *value, int minValue, int maxValue )
{
    boolean exitSubMenu = false;
    boolean forcePrint  = true;

    lcd.clear();

    while( !exitSubMenu )
    {
        btnPressed = readButtons();

        if( btnPressed == Button::Ok )
        {
            exitSubMenu = true;
        }
        else if( btnPressed == Button::Left && (*value)-1 >= minValue )
        {
            (*value)--;
        }
        else if( btnPressed == Button::Right && (*value)+1 <= maxValue )
        {
            (*value)++;
        }


        if( !exitSubMenu && (forcePrint || btnPressed != Button::Unknown) )
        {
            forcePrint = false;

            lcd.setCursor(0,0);
            lcd.print(txMENU[menuID]);

            lcd.setCursor(0,1);
            lcd.print("<");
            lcd.setCursor(columnsLCD-1,1);
            lcd.print(">");

            if( screen == Screen::Menu1 )
            {
                lcd.setCursor(1,1);
                lcd.print(txSMENU1[*value]);
            }
            else if( screen == Screen::Menu2 )
            {
                lcd.setCursor(1,1);
                lcd.print(txSMENU2[*value]);
            }
            else if( screen == Screen::Menu3 )
            {
                lcd.setCursor(1,1);
                lcd.print(txSMENU3[*value]);
            }
            else if( screen == Screen::Menu4 )
            {
                lcd.setCursor(1,1);
                lcd.print(txSMENU4[*value]);
            }
            else if( screen == Screen::Flag )
            {
                lcd.setCursor(columnsLCD/2-1, 1);
                lcd.print(*value == 0 ? "NO" : "SI");
            }
            else if( screen == Screen::Number )
            {
                lcd.setCursor(columnsLCD/2-1, 1);
                lcd.print(*value);
                lcd.print(char(223));
                lcd.print(" ");
            }
        }

    }

    lcd.clear();
}

void openSubMenu2( String menuID, Screen screen, int *value, int minValue, int maxValue )
{
    boolean exitSubMenu = false;
    boolean forcePrint  = true;

    lcd.clear();

    while( !exitSubMenu )
    {
        btnPressed = readButtons();

        if( btnPressed == Button::Ok )
        {
            exitSubMenu++;
 
        }
        else if( btnPressed == Button::Left)
        {
            if((*value)-1 >= minValue) {
              (*value)--;
            }
            else if((*value)-1 <= minValue) {
              (*value) = maxValue;
            }
            else {
               (*value)--;
            }
            
        }
        else if( btnPressed == Button::Right)
        {
            
            if((*value)+1 <= maxValue) {
              (*value)++;
            }
            else if((*value)+1 >= maxValue) {
              (*value) = minValue;
            }
            else {
               (*value)++;
            }
        }


        if( !exitSubMenu && (forcePrint || btnPressed != Button::Unknown) )
        {
            forcePrint = false;

            lcd.setCursor(1,3);
            lcd.print(menuID);

            lcd.setCursor(0,1);
            lcd.print("<");
            lcd.setCursor(columnsLCD-1,1);
            lcd.print(">");

            if( screen == Screen::Menu1 )
            {
                lcd.setCursor(1,1);
                lcd.print(txSMENU1[*value]);
            }
            else if( screen == Screen::Menu2 )
            {
                lcd.setCursor(1,1);
                lcd.print(txSMENU2[*value]);
            }
            else if( screen == Screen::Flag )
            {
                lcd.setCursor(columnsLCD/2-1, 1);
                lcd.print(*value == 0 ? "NO" : "SI");
            }
            else if( screen == Screen::Number )
            {
                lcd.setCursor(columnsLCD/2-1, 2);
                if((*value) <= 9){
                    lcd.setCursor(6,0);
                    lcd.print("    ");
                    lcd.setCursor(6,1);
                    lcd.print("    ");
                    if(menuID == "TEMPERATURA IDEAL" || menuID == "TEMPERATURA COOLER" || menuID == "TEMPERATURA HEATER") {
                      lcd.setCursor(15,0);
                      lcd.print(char(223));
                    }
                    printNumber(*value, 11);
                  }
                  else {
                    if(menuID == "TEMPERATURA IDEAL" || menuID == "TEMPERATURA COOLER" || menuID == "TEMPERATURA HEATER") {
                      lcd.setCursor(15,0);
                      lcd.print(char(223));
                    }
                    printNumber(*value, 7);
                  }
                
            }

            else if( screen == Screen::NumberYear )
            { 
                  if(*value >= 2000){
                   printNumber(*value - 2000 , 7); 
                  }
                  else {
                    printNumber(*value , 7);
                  }
                    
  
            }
        }

    }

    lcd.clear();
}

/**
 *  LEE (Y CONFIGURA LA PRIMERA VEZ) LA MEMORIA EEPROM CON LA CONFIGURACION DE USUARIO
 */

 
void readConfiguration()
{
    for( int i=0 ; i < sizeof(memory.d) ; i++  )
        memory.b[i] = EEPROM.read(i);

    if( memory.d.initialized != 'Y' )
    {
        memory.d.initialized = 'Y';
        memory.d.time_show   = 1;
        memory.d.time_unit   = 1;
        memory.d.time_x      = 0;
        memory.d.time_y      = 0;
        memory.d.temp_ideal  = 26;
        memory.d.temp_cooler = 30;
        memory.d.temp_heater = 17;
        memory.d.temp_y      = 1;
        memory.d.time_day    = 6;
        
        writeConfiguration();
    }
}


/**
 *  ESCRIBE LA MEMORIA EEPROM CON AL CONFIGURACION DE USUARIO
 */
void writeConfiguration()
{
    for( int i=0 ; i<sizeof(memory.d) ; i++  )
        EEPROM.write( i, memory.b[i] );
}


/**
 *  LEE LOS DISTINTOS BOTONES DISPONIBLES Y DEVUELVE EL QUE HAYA SIDO PULSADO
 *      Este bloque de codigo varia dependiendo del tipo de teclado conectado, en mi blog estan
 *      disponibles los ejemplos para teclados digitales, analogicos, y este para encoder rotatorio.
 *      Al cambiar de tipo de teclado hay que adaptar tambien el resto de codigo para que haga uso de cada boton segun queramos.
 */
Button readButtons()
{
    static boolean oldA = HIGH;
    static boolean newA = LOW;
    static boolean newB = LOW;

    btnPressed = Button::Unknown;
    newA = digitalRead(pENCO_DT);
    newB = digitalRead(pENCO_CLK);

    if( !oldA && newA )
    {
        btnPressed = !newB ? Button::Left : Button::Right;
        delay(50);
    }
    else if( !digitalRead(pENCO_SW) )
    {
        while(!digitalRead(pENCO_SW));
        btnPressed = Button::Ok;
        delay(50);
    }

    oldA = newA;
    return btnPressed;
}


 void custom0(int x){ 

  lcd.setCursor(x,0); 
  lcd.write((byte)0);  
  lcd.write(1);  
  lcd.write(2);
  lcd.setCursor(x, 1); 
  lcd.write(3);  
  lcd.write(4);  
  lcd.write(5);
}

void custom1(int x){
  lcd.setCursor(x,0);
  lcd.write(1);
  lcd.write(2);
  lcd.print(" ");
  lcd.setCursor(x,1);
  lcd.write(4);
  lcd.write(7);
  lcd.write(4);
}

void custom2(int x){
  lcd.setCursor(x,0);
  lcd.write(6);
  lcd.write(6);
  lcd.write(2);
  lcd.setCursor(x, 1);
  lcd.write(3);
  lcd.write(4);
  lcd.write(4);
}

void custom3(int x){
  lcd.setCursor(x,0);
  lcd.write(6);
  lcd.write(6);
  lcd.write(2);
  lcd.setCursor(x, 1);
  lcd.write(4);
  lcd.write(4);
  lcd.write(5); 
}

void custom4(int x){
  lcd.setCursor(x,0);
  lcd.write(3);
  lcd.write(4);
  lcd.write(7);
  lcd.setCursor(x, 1);
  lcd.print(" ");
  lcd.print(" ");
  lcd.write(7);
}

void custom5(int x){
  lcd.setCursor(x,0);
  lcd.write(3);
  lcd.write(6);
  lcd.write(6);
  lcd.setCursor(x, 1);
  lcd.write(4);
  lcd.write(4);
  lcd.write(5);
}

void custom6(int x){
  lcd.setCursor(x,0);
  lcd.write((byte)0);
  lcd.write(6);
  lcd.write(6);
  lcd.setCursor(x, 1);
  lcd.write(3);
  lcd.write(4);
  lcd.write(5);
}

void custom7(int x){
  lcd.setCursor(x,0);
  lcd.write(1);
  lcd.write(1);
  lcd.write(2);
  lcd.setCursor(x, 1);
  lcd.print(" ");
  lcd.print(" ");
  lcd.write(7);
}

void custom8(int x){
  lcd.setCursor(x,0);
  lcd.write((byte)0);
  lcd.write(6);
  lcd.write(2);
  lcd.setCursor(x, 1);
  lcd.write(3);
  lcd.write(4);
  lcd.write(5);
}

void custom9(int x){
  lcd.setCursor(x,0);
  lcd.write((byte)0);
  lcd.write(6);
  lcd.write(2);
  lcd.setCursor(x, 1);
  lcd.print(" ");
  lcd.print(" ");
  lcd.write(7);

}

 void printNumber(int val, int inicio){
  
     int col=inicio;        
     
     if( val >= 10){
       printDigits(val/10,col);     
       printDigits(val%10,col+4);
     }
     else{
       printDigits(val,col);
     }
}

void printDigits(int digits, int x){
  // utility function for digital clock display: prints preceding colon and leading 0

  switch (digits) {
  case 0:  
    custom0(x);
    break;
  case 1:  
    custom1(x);
    break;
  case 2:  
    custom2(x);
    break;
  case 3:  
    custom3(x);
    break;
  case 4:  
    custom4(x);
    break;
  case 5:  
    custom5(x);
    break;
  case 6:  
    custom6(x);
    break;
  case 7:  
    custom7(x);
    break;
  case 8:  
    custom8(x);
    break;
  case 9:  
    custom9(x);
    break;

  }
}

void ConfigTime() {
    myRTC.updateTime();
    memory.d.time_hours = myRTC.hours;
    openSubMenu2( "SELECCIONE HORA", Screen::Number, &memory.d.time_hours,    0, 23        );
    memory.d.time_minutes = myRTC.minutes;
    openSubMenu2( "SELECCIONE MINUTOS", Screen::Number, &memory.d.time_minutes,    0, 59        );
    memory.d.time_day = myRTC.dayofmonth;
    openSubMenu2( "SELECCIONE DIA", Screen::Number, &memory.d.time_day,    1, 31        );
    memory.d.time_month = myRTC.month;
    openSubMenu2( "SELECCIONE MES", Screen::Number, &memory.d.time_month,    1, 12        );
    memory.d.time_year = myRTC.year;
    openSubMenu2( "SELECCIONE A\356o", Screen::NumberYear, &memory.d.time_year,    2019, 2099        );
    myRTC.setDS1302Time(22, memory.d.time_minutes, memory.d.time_hours, 1, memory.d.time_day, memory.d.time_month, memory.d.time_year);
}

void ConfigPines() {
    
    openSubMenu2( "PIN 1 HORA ON", Screen::Number, &memory.d.pin_1_on_h,    0, 23        );
    
    openSubMenu2( "PIN 1 MINUTOS ON", Screen::Number, &memory.d.pin_1_on_m,    0, 59        );

    openSubMenu2( "PIN 1 HORA OFF", Screen::Number, &memory.d.pin_1_off_h,    0, 23        );
    
    openSubMenu2( "PIN 1 MINUTOS OFF", Screen::Number, &memory.d.pin_1_off_m,    0, 59        );

    openSubMenu2( "PIN 2 HORA ON", Screen::Number, &memory.d.pin_2_on_h,    0, 23        );
    
    openSubMenu2( "PIN 2 MINUTOS ON", Screen::Number, &memory.d.pin_2_on_m,    0, 59        );

    openSubMenu2( "PIN 2 HORA OFF", Screen::Number, &memory.d.pin_2_off_h,    0, 23        );
    
    openSubMenu2( "PIN 2 MINUTOS OFF", Screen::Number, &memory.d.pin_2_off_m,    0, 59        );

    lcd.setCursor(0,0);
    lcd.print("P1-ON : ");
    lcd.setCursor(8,0);
    lcd.print(memory.d.pin_1_on_h);
    lcd.print(":");
    if(memory.d.pin_1_on_m <= 9)
    {
      lcd.print("0");
      lcd.print(memory.d.pin_1_on_m);
    }
    else
    {
      lcd.print(memory.d.pin_1_on_m);
    }

    lcd.setCursor(0,1);
    lcd.print("P1-OFF: ");
    lcd.setCursor(8,1);
    lcd.print(memory.d.pin_1_off_h);
    lcd.print(":");
    if(memory.d.pin_1_off_m <= 9)
    {
      lcd.print("0");
      lcd.print(memory.d.pin_1_off_m);
    }
    else
    {
      lcd.print(memory.d.pin_1_off_m);
    }

    lcd.setCursor(0,2);
    lcd.print("P2-ON : ");
    lcd.setCursor(8,2);
    lcd.print(memory.d.pin_2_on_h);
    lcd.print(":");
    if(memory.d.pin_2_on_m <= 9)
    {
      lcd.print("0");
      lcd.print(memory.d.pin_2_on_m);
    }
    else
    {
      lcd.print(memory.d.pin_2_on_m);
    }

    lcd.setCursor(0,3);
    lcd.print("P2-OFF: ");
    lcd.setCursor(8,3);
    lcd.print(memory.d.pin_2_off_h);
    lcd.print(":");
    if(memory.d.pin_2_off_m <= 9)
    {
      lcd.print("0");
      lcd.print(memory.d.pin_2_off_m);
    }
    else
    {
      lcd.print(memory.d.pin_2_off_m);
    }
    delay(5000);
 }

void VerPines() {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("P1-ON : ");
    lcd.setCursor(8,0);
    lcd.print(memory.d.pin_1_on_h);
    lcd.print(":");
    if(memory.d.pin_1_on_m <= 9)
    {
      lcd.print("0");
      lcd.print(memory.d.pin_1_on_m);
    }
    else
    {
      lcd.print(memory.d.pin_1_on_m);
    }
    lcd.setCursor(0,1);
    lcd.print("P1-OFF: ");
    lcd.setCursor(8,1);
    lcd.print(memory.d.pin_1_off_h);
    lcd.print(":");
    if(memory.d.pin_1_off_m <= 9)
    {
      lcd.print("0");
      lcd.print(memory.d.pin_1_off_m);
    }
    else
    {
      lcd.print(memory.d.pin_1_off_m);
    }
    lcd.setCursor(0,2);
    lcd.print("P2-ON : ");
    lcd.setCursor(8,2);
    lcd.print(memory.d.pin_2_on_h);
    lcd.print(":");
    if(memory.d.pin_2_on_m <= 9)
    {
      lcd.print("0");
      lcd.print(memory.d.pin_2_on_m);
    }
    else
    {
      lcd.print(memory.d.pin_2_on_m);
    }
    lcd.setCursor(0,3);
    lcd.print("P2-OFF: ");
    lcd.setCursor(8,3);
    lcd.print(memory.d.pin_2_off_h);
    lcd.print(":");
    if(memory.d.pin_2_off_m <= 9)
    {
      lcd.print("0");
      lcd.print(memory.d.pin_2_off_m);
    }
    else
    {
      lcd.print(memory.d.pin_2_off_m);
    }
    delay(5000);
}
 
void print2digits(int number) {
  if (number >= 0 && number < 10)
    lcd.print('0');
  lcd.print(number, DEC);
}

void MaxiMin() {
      lcd.clear();
      lcd.setCursor(0,2);
      lcd.print("-------------------");  
      lcd.setCursor(0,0);           
      lcd.print("T");
      lcd.print(char(223));
      lcd.setCursor(0,1);
      lcd.print("C");           
      printNumber(mini,2);
      lcd.setCursor(10,0); 
      lcd.print("T");
      lcd.print(char(223));
      lcd.setCursor(10,1); 
      lcd.print("C");  
      lcd.setCursor(12,0);       
      printNumber(maxi,12);
      lcd.setCursor(0,3);
      lcd.print("  MINIMA  -  MAXIMA");
      delay(5000);
  }

  void MaxiMinh() {
      lcd.clear();
      lcd.setCursor(0,2);
      lcd.print("-------------------");  
      lcd.setCursor(0,0);           
      lcd.print("H");
      lcd.print("%");          
      printNumber(hmini,2);
      lcd.setCursor(10,0); 
      lcd.print("H");
      lcd.print("%"); 
      lcd.setCursor(12,0);       
      printNumber(hmaxi,12);
      lcd.setCursor(0,3);
      lcd.print("  MINIMA  -  MAXIMA");
      delay(5000);
  }
