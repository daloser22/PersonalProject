
 //DEFINICION E INICIALIZACIÓN DE TODOS LOS PINES QUE USAREMOS

int ledRojo = 12; //Asignamos el pin para encender y apagar el led
int ledIR = 11; //Asignamos el pin para encender y apagar el led
int ValorSensorRojo = A0; // Asignamos el pin para la recogida de datos CONTROL A
int ValorSensorIR = A1; // Asignamos el pin para la recogida de datos   CONTROL B
int controlMUX=7; 
int alimentacionVdd= 4;
int LectR = 0; //Inicializamos los valores que estamos leyendo del circuito Rojo
int LectIR = 0; //Inicializamos los valores que estamos leyendo del circuito Infrarrojo
int InterruptorRojo = 6; //Controlaremos el circuito del led Rojo
int InterruptorIR = 5; // controlaremos el circuito del led IR
int Contador=0; //Inicializamos el contador de milisegundos
int Cont_Ciclos=0; //Inicializamos el contador de ciclos de encendido de los LEDS
int Cont_Muestras=0; //Inicializamos el contador de muestras 
float Media=0; //Inicializamos el valor de la media aritmetica
float LectR_ANT = 0; 
float LectIR_ANT=0;
float Time_pulso=0; //Tiempo en el que se produce un cruce
float Time_pulso_ANT=0; //Tiempo en el que se produjo un cruce
float Cont_Cruces=0; //Contador de cruces
float Media_Periodos=0; // media de los periodos
float Periodo=0; //Valor de un periodo
float BPM=0; //Valor donde guardaremos el pulso cardiaco
float dA_R=0; // Absorcion delta Roja
float dA_IR=0; // Absorcion delta IR
float R=0; // Ratio de absorcion
float R_Temporal=0; //Variable de uso temporal donde iremos guardando el valor del ratio temporal
float Media_R=0; //Media de ratios calculados
float SpO2=0; //Valor donde guardaremos la saturacion de oxigeno en sangre


void setup() {

  //CONFIGURACION DE LOS PINES COMO ENTRADA/SALIDA
  // put your setup code here, to run once:
  pinMode(ledRojo, OUTPUT);
  pinMode(ledIR, OUTPUT);
  pinMode(ValorSensorIR,INPUT);
  pinMode(ValorSensorRojo,INPUT);
  pinMode(InterruptorRojo,OUTPUT);
  pinMode(InterruptorIR,OUTPUT);
  pinMode(controlMUX,OUTPUT);
  pinMode(alimentacionVdd,OUTPUT);
  digitalWrite(alimentacionVdd,HIGH);
  analogReference (DEFAULT); // Configuración de la tensión de referencia por defecto (5 V)
  Serial.begin(9600); // transmision por el puerto serie
}

void loop() {
  //el ciclo tarda un total de 2milisegundos en desarrollarse, lo repetiremos unas 100 veces 
  
  for (Cont_Ciclos=0; Cont_Ciclos<=100; Cont_Ciclos++){
 

 
    //Control del encendido alterno del led Rojo
 
 digitalWrite(ledRojo,HIGH);  //Encendemos el led Rojo
 delayMicroseconds(10);  //Esperamos 10 microsegundos
 digitalWrite(controlMUX,LOW);
 digitalWrite(InterruptorIR,LOW);
 digitalWrite(InterruptorRojo,HIGH); //Cerramos el interruptor de Sample & Hold  (CONTROL A)
 delayMicroseconds(960); // Esperamos 460 para la estabilizacion de la señal
 LectR = analogRead(ValorSensorRojo);  //Leemos el valor de salida del circuito Rojo
 digitalWrite(InterruptorRojo,LOW);  //Abrimos el interruptor de Sample & Hold    (CONTROL A)
 delayMicroseconds(10); // Esperamos 10 microsegundos
 digitalWrite(ledRojo,LOW); //Apagamos el led Rojo
   
    
 delayMicroseconds (20);
   
 //Control del encendido alterno del led IR
 digitalWrite(ledIR,HIGH);  //Encendemos el led Rojo
 delayMicroseconds(10);  //Esperamos 10 microsegundos
 digitalWrite(controlMUX,LOW);
 digitalWrite(InterruptorRojo,LOW);
 digitalWrite(InterruptorIR,HIGH); //Cerramos el interruptor de Sample & Hold  (CONTROL B)
 delayMicroseconds(960); // Esperamos 960 para la estabilizacion de la señal
 LectIR = analogRead(ValorSensorIR);  //Leemos el valor de salida del circuito InfraRojo
 digitalWrite(InterruptorIR,LOW);  //Abrimos el interruptor de Sample & Hold PIN    (CONTROL B)
 delayMicroseconds(10); // Esperamos 10 microsegundos
 digitalWrite(ledIR,LOW); //Apagamos el led Rojo

 delayMicroseconds (20);   
    
    }
 
Cont_Ciclos=0; // Reiniciamos el valor del contador de ciclos

Cont_Muestras=Cont_Muestras+1; //incrementaremos el contador cada vez que se compelte el ciclo principal, cada 200ms

 //cada 7 segundos mostraremos los valores de SpO2 y de BPM

 if (Cont_Muestras<=35){
  Media=((Media*(Cont_Muestras-1))+LectR)/Cont_Muestras;
// cuando hayan pasado 3 segundos, comenzaremos a buscar los cruces por la media y calcular el ratio
   if(Cont_Muestras>15){
      if(LectR_ANT>Media&&LectR<Media){  //Condicion que detecta un cruce
         Time_pulso=millis(); //Capturamos el tiempo en el que se ha producido el cruce

          if (Cont_Cruces==0){  //Si es el primer cruce que detectamos
            Time_pulso_ANT=Time_pulso;
            }
          if (Cont_Cruces>=1){  //Si no es el primer cruce, calcularemos el periodo entre este y el anterior
            Periodo=Time_pulso-Time_pulso_ANT;  //Calculamos el valor del periodo
            Time_pulso_ANT=Time_pulso; //Actualizamos el valor de ant
            //Calculamos una media entre los periodos que vamos encontrando por el momento
            Media_Periodos=(Media_Periodos*(Cont_Cruces-1)+Periodo)/Cont_Cruces;
            }
      Cont_Cruces=Cont_Cruces+1;
      }

      //METODO DE ABSORCION DE DELTAS----> PARA EL CALCULO DEL RATIO
      dA_R=((LectR-LectR_ANT)*2)/(LectR+LectR_ANT);
      dA_IR=((LectIR-LectIR_ANT)*2)/(LectIR+LectIR_ANT);
      R_Temporal=dA_IR/dA_R;   //valor temporal del Ratio que vamos calculando
      
      //TENEMOS QUE CONTROLAR QUE PASA SI EL RATIO TEMPORAL ES INFINITO
    if(R_Temporal!=INFINITY){
      R=R_Temporal;
      }
    //hacemos la media entre los ratios calculados hasta el momento
    Media_R=(Media_R*(Cont_Muestras-16)+R)/(Cont_Muestras-15);
    LectR_ANT=LectR;
    LectIR_ANT=LectIR;
    
    }
 }

 else{
  //Calculamos y mostramos por pantalla los valores de pulso y de SpO2
    BPM=60000/Media_Periodos;
    //SpO2=(115-(30*Media_R));
    SpO2=(4+(115-(30*Media_R)));
    if (SpO2>100){
    Serial.println("Saturacion de oxigeno en sangre = 99.00 ");
    }
    else{
    Serial.print("Saturacion de oxigeno en sangre = ");
    Serial.println(SpO2);
      }
    if (60<BPM>210){
      Serial.print("Pulso Cardiaco = Error en la medida");
    }
    else{
    Serial.print("Pulso Cardiaco = ");
    Serial.println(BPM);  
    }    
//VOLVERMOS A INICIALIZAR TODAS LAS VARIABLES NECESARIAS PARA LOS CALCULOS
    Cont_Muestras=0;
    Media=0;
    LectIR_ANT=0;
    LectR_ANT=0;
    Time_pulso=0;
    Time_pulso_ANT=0;
    Cont_Cruces=0;
    Media_Periodos=0;
    Periodo=0;
    Media_R=0;
    R=0;
    R_Temporal=0;
}
}

  

