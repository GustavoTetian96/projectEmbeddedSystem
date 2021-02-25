
//Librerias
#include <Wire.h> //comms
#include <LiquidCrystal_I2C.h> //Pantalla
#include <DS3231.h> //Reloj
#include <SR04.h> //Ultrasonico
#include "pitches.h" //Buzzer

#define DS1621_ADDRESS  0x48

//Definicion de Pines
#define Echo 11 //Echo del Ultrasonico
#define Trig 10 //Trig del Ultrasonico
#define LP 9 //Salida LED
#define buzz 3
#define abre 1  //para portero
#define cierra 2  //para portero

//Inicializar
//Adafruit_MLX90614 mlx = Adafruit_MLX90614(); //Sensor Termico
LiquidCrystal_I2C lcd(0x20,20,4);
DS3231 clock; //Reloj
RTCDateTime dt; //Fecha
SR04 sr04=SR04(Echo,Trig); //Ultrasonico


//Variables
int Espera1=500; //Espera en el loop
float sinVal;
int toneVal;
const int ledRasp=6;
const int ledD5=5;


//Fecha
String fDia;
String fMes; //Mes en texto
String fAno;
String fHora;
String fMin;
String fSeg;
String fTime;

//Distancia
int Dist; //Distancia del ultrasonico
int DistMin=10; //Distancia minima para detectar al sujeto (mm)
int Presente=0; //Si hay alguien frente al Termometro
int Espera=3000; //Tiempo de espera para verificar sujeto
unsigned long Tiempo=0; //Tiempo que lleva detectado para Millis
int Ahora=0; //Millis en el momento que se inicia

//Temperatura
//float TempObj; //Temperatura del sujeto
float TempMax=370; //Temperatura maxima permitida
int TpoAlarma=200; //Tiempo de Alarma por alta temperatura
int tempC;
int tempAnte=0;



void setup() 
{
  pinMode(buzz, OUTPUT);
  
  pinMode(LP,OUTPUT); //LED
  pinMode(ledRasp,OUTPUT);
  pinMode(ledD5,OUTPUT);
  //mlx.begin(); //Termico
  Serial.begin(9600);

  clock.begin(); //Reloj
  clock.setDateTime(2021,1,22,14,17,0); //Quitar comentaro y ajustar a la hora deseada en primer run.

  digitalWrite(LP,HIGH);
  delay(1000);
  digitalWrite(LP,LOW);
  
//Mensaje inicial
  lcd.init(); //inicio lcd
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(" TERMOMETRO / RELOJ");
  lcd.setCursor(0,1);
  lcd.print("AMB: ");
  lcd.setCursor(0,2);
  lcd.print("PER: ");



  Wire.begin();           // join i2c bus
  //  REGISTRO DE CONTROL
   //==================================================================
   Wire.beginTransmission(DS1621_ADDRESS);//Condicion de INICIO y direcciono el SLAVE
   Wire.write(0xAC); //Mando comando para escribir en el registro de control
   Wire.write(0x02);
   Wire.endTransmission();
   //Configuro para una adquisicion continua
   //==================================================================
   //  REGISTRO ALARMA TERMOSTATO TH-TL
   //==================================================================
   Wire.beginTransmission(DS1621_ADDRESS); //Condicion de INICIO y direcciono el SLAVE
   Wire.write(0xA1); //Mando comando para escribir en el registro TH
   Wire.write(0x1D); //Configuro T maxima del termostato
   Wire.write(0x00); 
   Wire.endTransmission();
   delay(500);
   Wire.beginTransmission(DS1621_ADDRESS); //Condicion de INICIO y direcciono el SLAVE
   Wire.write(0xA2); //Mando comando para escribir en el registro TL
   Wire.write(0x19); //Configuro T minima del termostato
   Wire.write(0x00);
   Wire.endTransmission();
   //==================================================================
   //  REGISTRO DE COMIENZO DE CONVERSION
   //==================================================================
   Wire.beginTransmission(DS1621_ADDRESS); //COndicion de INICIO REPETIDA
   Wire.write(0xEE); //Mando comando para que de comienzo la conversion
   Wire.endTransmission(); //Condicion de STOP // no hace falta utilizar esta funcion




}



// variables
char c_buffer[8], f_buffer[8];
void loop() 
{
   delay(100); 
   Wire.beginTransmission(DS1621_ADDRESS); //Condicion de INICIO
   Wire.write(0xAA); //Mando comando para leer la ultima conversion
   Wire.endTransmission(); // STOP
   Wire.requestFrom(0x48, 1); //Realizo una peticion de lectura al SLAVE y de un Byte
   //tempC =  Wire.read(); //Guardo el resultado
   tempC = get_temperature();
   int datoRasp=Serial.read()-'0';
   
   if(datoRasp==abre){
      digitalWrite(ledRasp,HIGH);
    }
   else{
      digitalWrite(ledRasp,LOW);
    }
    if(datoRasp==cierra){
      digitalWrite(ledD5,HIGH);
    }
   else{
      digitalWrite(ledD5,LOW);
    }
    
   //int tempWhole = tempC >> 8;
   //int tempFrac = (tempC & 0xFF) / 256;
   //float temp = tempWhole + tempFrac;

//Distancia
Dist=sr04.Distance();
if(Dist>DistMin) //No hay nadie
{
  Presente=0;
  Tiempo=millis();
}
if(Dist<=DistMin && Presente==0) //Llego alguien, tomemos el tiempo
{
  Presente=1;
  Tiempo=millis();
}

if(Presente==1)
{
  if(millis()-Tiempo>Espera) //Se completo el tiempo
  {
    Presente=2;
  }
}

//Actualizar pantalla
lcd.setCursor(12,3);

  //Temperaturas
lcd.setCursor(4,2);
lcd.print(c_buffer);
lcd.setCursor(8,2);
//lcd.print("C");


//TempObj=mlx.readObjectTempC();
switch(Presente)
{
  case 0: //No hay nadie
  lcd.setCursor(4,2);
  lcd.print("--.--c");
  lcd.setCursor(0,3);
  lcd.print("BUSCANDO...  ");
  break;

  case 1: //Llego alguien
  lcd.setCursor(4,2);
  lcd.print("--.--c");
  lcd.setCursor(0,3);
  lcd.print("LEYENDO...");
  //tone(12,NOTE_C5,TpoAlarma);
  break;

  case 2: //Se Completo el tiempo
  lcd.setCursor(4,2);
  lcd.print(c_buffer);
  lcd.setCursor(8,2);
  //lcd.print("C");

  if(tempC > TempMax) {   // if temperature < 0 °C
    tempC = abs(tempC);  // absolute value
    sprintf(c_buffer, "%02u.%1u%cC", tempC / 10, tempC % 10, 223);
    //tone(12,NOTE_G5,TpoAlarma);
    lcd.setCursor(0,3);
    lcd.print("TEMP ALTA!!!");
    digitalWrite(LP,HIGH);
    delay(TpoAlarma);
    digitalWrite(LP,LOW);
    //Serial.println(tempC);
    tempC=tempC/10;
    if(tempAnte!=tempC){
      Serial.println(tempC);
      tempAnte=tempC;     
    }
    for (int x=0; x<180; x++) {
  // pasamos de grados a radianes
       sinVal = (sin(x*(3.1412/180)));
  // Generamos el tono, o mejor dicho la frecuencia
       toneVal = 2000+(int(sinVal*1000));
       tone(3, toneVal,2);
     }
   
  }
  else {
    //if (tempC > 128)    // if temperature >= 100.0 °C
      //sprintf(c_buffer, "%03u.%1u%cC", c_temp / 10, c_temp % 10, 223);
    //else
      sprintf(c_buffer, " %02u.%1u%cC", tempC / 10, tempC % 10, 223);
      lcd.setCursor(0,3);
      lcd.print("NORMAL        ");
      
      tempC=tempC/10;
      if(tempAnte!=tempC){
        Serial.println(tempC);
        tempAnte=tempC;     
      }
      
  }
  break;


}


//Hora y Fecha
dt=clock.getDateTime();
fDia=String(dt.day);
if(dt.day<10)fDia="0"+fDia; //Para mantener en dos digitos

fAno=String(dt.year-2000);

lcd.setCursor(11,1);
lcd.print(fDia+"/"+ Mes()+"/"+fAno); //Uso de la funcion Mes()

fHora=String(dt.hour);
if(dt.hour<10)fHora="0"+fHora; //Para mantener en dos digitos

fMin=String(dt.minute);
if(dt.minute<10)fMin="0"+fMin; //Para mantener en dos digitos

fSeg=String(dt.second);
if(dt.second<10)fSeg="0"+fSeg; //Para mantener en dos digitos

fTime=fHora+":" + fMin + ":" + fSeg;
lcd.setCursor(11,2);
lcd.print(fTime);

delay(Espera1);

}

/////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
/// F U N C I O N E S 
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////


String Mes()
{
  //Serial.println(dt.month);
  switch (dt.month)
  {
    case 1:
    fMes="Ene";
    break;

    case 2:
    fMes="Feb";
    break;

    case 3:
    fMes="Mar";
    break;

    case 4:
    fMes="Abr";
    break;

    case 5:
    fMes="May";
    break;

    case 6:
    fMes="Jun";
    break;

    case 7:
    fMes="Jul";
    break;

    case 8:
    fMes="Ago";
    break;

    case 9:
    fMes="Sep";
    break;

    case 10:
    fMes="Oct";
    break;

    case 11:
    fMes="Nov";
    break;

    case 12:
    fMes="Dic";
    break;
  }

return fMes;
}

int16_t get_temperature() {
  Wire.beginTransmission(DS1621_ADDRESS); // connect to DS1621 (send DS1621 address)
  Wire.write(0xAA);                       // read temperature command
  Wire.endTransmission(false);            // send repeated start condition
  Wire.requestFrom(DS1621_ADDRESS, 2);    // request 2 bytes from DS1621 and release I2C bus at end of reading
  uint8_t t_msb = Wire.read();            // read temperature MSB register
  uint8_t t_lsb = Wire.read();            // read temperature LSB register
 
  // calculate full temperature (raw value)
  int16_t raw_t = (int8_t)t_msb << 1 | t_lsb >> 7;
  // convert raw temperature value to tenths °C
  raw_t = raw_t * 10/2;
  return raw_t;
}
