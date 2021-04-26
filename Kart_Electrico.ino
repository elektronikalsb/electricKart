
#include <Nextion.h>                                                                //Libreria para usar la pantalla Nextion
#include <SoftwareSerial.h>                                                         //Libreria para añadir otro serial

SoftwareSerial hc06 (4,3);                                                          //Pin serial del modulo bluetooth (RX-TX)


int lectura;                                                                        //Variable donde se guarda la lectura del bluetooth
int contmodo1;
int contmodo2;
int contmodo3;


//////////////////// NEXTION /////////////////////////

NexProgressBar barra = NexProgressBar (0, 1, "j0" );                                //Declaraciones de los elementos que utilizamos en la pantalla
NexNumber number = NexNumber (0 , 2, "n0");                                                                     //                                                                     
NexProgressBar proximidad = NexProgressBar (0, 9, "j5" );                                                       //
NexProgressBar nivelbateria = NexProgressBar (0, 10, "j6" );                                                    //

NexProgressBar modonex1 = NexProgressBar (0, 6, "j1" );                                                         //
NexProgressBar modonex2 = NexProgressBar (0, 7, "j2" );                                                         //
NexProgressBar modonex3 = NexProgressBar (0, 8, "j3" );                                                         //
NexProgressBar stopnex = NexProgressBar (0, 9, "j4" );                                                          //


//////////////////// VELOCIDAD ///////////////////////

#define intPinAc 2                                                                  // intPinAc --> Interrupcion Pin Acelerador
unsigned long Tiempo=0;                                                             //Variable de millis

float vuelta;                                                                       //Variable donde guardamos el tiempo que tarda en dar una vuelta el plato
float Tiempo1;                                                                      //Variable para pasar el tiempo de milisegundos a segundos
float MS;                                                                           //Variable para calcular los metros por segundo (m/s)
int velocidad1;                                                                     //Variable de la velocidad en kilometros por hora (km/h)
bool contInt=false;                                                                 


/////////////////// ACELERADOR //////////////////////

#define pot A0                                                                      //Declaramos el pin de la entrada del potenciometro (acelerador)
#define motor 10                                                                    //Declaramos el pin de salida del acelerador

int valor_pot;                                                                      //Variable del valor que nos da el potenciometro
#define modo_3 3                                                                    //Declaramos el caso modo_3
int modo3nex;                                                                       //Declaramos el valor del potenciometro entre 0-100 en el modo 3
int pwm3;                                                                           //Declaramos el pwm en el modo 3
#define modo_2 2                                                                    //Declaramos el caso modo_2
int modo2nex;                                                                       //Declaramos el valor del potenciometro entre 0-100 en el modo 2
int pwm2;                                                                           //Declaramos el pwm en el modo 2
#define modo_1 1                                                                    //Declaramos el caso modo_1
int modo1nex;                                                                       //Declaramos el valor del potenciometro entre 0-100 en el modo 1
int pwm1;                                                                           //Declaramos el pwm en el modo 1
int pwm;                                                                            //
int modo;                                                                           //Declaramos la variable del switch case
#define modo_start 0                                                                //Declaramos el caso modo_start
#define modo_stop 4                                                                 //Declaramos el caso modo_stop


////////////////////// BATERIA //////////////////////////

int bateria=0;                                                                      //Declaramos la variable de lectura de la bateria
int nivel=0;                                                                        //Declaramos la variable del calculo de la bateria
int media=0;                                                                        //Declaramos la variable del calculo de la bateria
long x=0;                                                                            //Declaramos la variable del calculo de la bateria
int valor_bateria;                                                                  //Declaramos la variable de la media de la bateria


///////////////////// ULTRASONICO ///////////////////////

const int EchoPin = 12;                                                             //Declaramos EchoPin al pin 12
const int TriggerPin = 13;                                                          //Declaramos TriggerPin al pin 13
int distancianex;                                                                   //Declaramos la variable de distancia


void setup() {
  
  Serial.begin (9600);                                                              //Inicializamos el serial

  hc06.begin(9600);                                                                 //Inicializamos el serial hc06

  pinMode ( pot , INPUT );                                                          //Declaramos pot como entrada
  
  pinMode (intPinAc,INPUT_PULLUP);                                                  //Declaramos intPinAc como entrada
  attachInterrupt(digitalPinToInterrupt(intPinAc), intVelocidad, FALLING);          //Declaramos la interrupcion intVelocidad y hacemos que entre cuando descienda

  pinMode (motor, OUTPUT);                                                          //Declaramos motor como salida

  pinMode (A1,INPUT);                                                               //Declaramos A1 (entrada de la bateria) como entrada

  pinMode (TriggerPin, OUTPUT);                                                     //Declaramos TriggerPin como salida
  pinMode (EchoPin, INPUT);                                                         //Declaramos EchoPin como entrada

  delay(200);

}

void loop() {

lectura = hc06.read();                                                              //Leemos el serial del modulo bluetooth

        if ( lectura == '1') modo = modo_1;                                         //Dependiendo de lo que nos mande el bluetooth nos metemos en un modo o en otro
        if ( lectura == '2') modo = modo_2;
        if ( lectura == '3') modo = modo_3;
        if ( lectura == '4') modo = modo_stop;
        if ( lectura == '5') modo = modo_start;

switch (modo){
  
        modonex1.setValue(0);                                                       //Cuando encendemos ponemos todo en cero menos la barra del "stop" para que la 
        modonex2.setValue(0);                                                               //pantalla nos indique que estamos en stop
        modonex3.setValue(0);
        stopnex.setValue(100);
        contmodo1=0;
        contmodo2=0;
        contmodo3=0;

    case modo_1:                                                                    //Caso_1
        if(contmodo1<=1){                                                           //Ponemos un contador para que lo que esta dento del if lo haga solo una vez
          modonex1.setValue(100);
          modonex2.setValue(0);                                                     //Ponemos las barras de los modos en sus respectivas posiciones
          modonex3.setValue(0);
          stopnex.setValue(0);
          }
        acelerador ();                                                              //Entramos en la funcion del acelerador
        velocidad ();                                                               //Entramos en la funcion de la velocidad
        nivel_bateria ();                                                           //Entramos en la funcion del nivel de bateria
//        ultrasonico ();                                                             //Entramos en la funcion del ultrasonico
        barra.setValue(modo1nex);                                                   //Movemos la barra de la pantalla dependiendo del valor del potenciometro
        number.setValue(velocidad1);                                                //Cambiamos el numero dependiendo de la velocidad
        nivelbateria.setValue(valor_bateria);                                       //Movemos la barra de la pantalla dependiendo del nivel de bateria
//        proximidad.setValue(distancianex);                                        //Movemos la barra de la pantalla dependiendo de la distancia del ultrasonico
        analogWrite (motor,pwm1);                                                   //Mandamos el valor pwm al variador para que acelere mas o menos
        contmodo1++;
        contmodo2=0;
        contmodo3=0;
        interrupts();
      break;                                                                        //Terminamos el caso uno

    case modo_2:                                                                    //Caso_2
        if(contmodo2<=1){                                                           //Ponemos un contador para que lo que esta dento del if lo haga solo una vez
          modonex1.setValue(0);
          modonex2.setValue(100);                                                   //Ponemos las barras de los modos en sus respectivas posiciones
          modonex3.setValue(0);
          stopnex.setValue(0);
          }
        acelerador ();                                                              //Entramos en la funcion del acelerador
        velocidad ();                                                               //Entramos en la funcion de la velocidad
        nivel_bateria ();                                                           //Entramos en la funcion del nivel de bateria
//        ultrasonico ();                                                             //Entramos en la funcion del ultrasonico
        barra.setValue(modo2nex);                                                   //Movemos la barra de la pantalla dependiendo del valor del potenciometro
        number.setValue(velocidad1);                                                //Cambiamos el numero dependiendo de la velocidad
        nivelbateria.setValue(valor_bateria);                                       //Movemos la barra de la pantalla dependiendo del nivel de bateria
//        proximidad.setValue(distancianex);                                          //Movemos la barra de la pantalla dependiendo de la distancia del ultrasonico
        analogWrite (motor,pwm2);                                                   //Mandamos el valor pwm al variador para que acelere mas o menos
        contmodo2++;
        contmodo1=0;
        contmodo3=0;
        interrupts();
      break;                                                                        //Terminamos el caso dos

    case modo_3:                                                                    //Caso_3
        if(contmodo3<=1){                                                           //Ponemos un contador para que lo que esta dento del if lo haga solo una vez
           modonex1.setValue(0);
           modonex2.setValue(0);                                                    //Ponemos las barras de los modos en sus respectivas posiciones
           modonex3.setValue(100);
           stopnex.setValue(0);
          }
        acelerador ();                                                              //Entramos en la funcion del acelerador
        velocidad ();                                                               //Entramos en la funcion de la velocidad
        nivel_bateria ();                                                           //Entramos en la funcion del nivel de bateria
//        ultrasonico ();                                                             //Entramos en la funcion del ultrasonico
        analogWrite (motor,pwm3);                                                   //Mandamos el valor pwm al variador para que acelere mas o menos
        barra.setValue(modo3nex);                                                   //Movemos la barra de la pantalla dependiendo del valor del potenciometro
        number.setValue(velocidad1);                                                //Cambiamos el numero dependiendo de la velocidad
        nivelbateria.setValue(valor_bateria);                                       //Movemos la barra de la pantalla dependiendo del nivel de bateria
//        proximidad.setValue(distancianex);                                          //Movemos la barra de la pantalla dependiendo de la distancia del ultrasonico
        contmodo3++;
        contmodo1=0;
        contmodo2=0;
        interrupts();
      break;                                                                        //Terminamos el caso tres 

    case modo_start:                                                                //Caso modo_start
        analogWrite (motor,0);                                                      //Mandamos el valor pwm al variador para que no acelere
        modonex1.setValue(0);                                                       //Ponemos las barras de los modos en sus respectivas posiciones
        modonex2.setValue(0);
        modonex3.setValue(0);
        stopnex.setValue(100);
        contmodo1=0;
        contmodo2=0;
        contmodo3=0;
      break;                                                                        //Terminamos el caso modo_start

    case modo_stop:                                                                 //Caso modo_stop
        analogWrite (motor,0);                                                      //Mandamos el valor pwm al variador para que no acelere
        modonex1.setValue(0);                                                       //Ponemos las barras de los modos en sus respectivas posiciones
        modonex2.setValue(0);
        modonex3.setValue(0);
        stopnex.setValue(100);
        contmodo1=0;
        contmodo2=0;
        contmodo3=0;
      break;                                                                        ////Terminamos el caso modo_stop
}
}


//////////////////////////////////////////// VELOCIDAD //////////////////////////////////////

void velocidad (void){                                                              //Creamos la funcion de velocidad
if ((contInt == HIGH)&& (vuelta>0)){                                                
     Tiempo1 = vuelta / 1000;                                                       //Pasamos el tiempo de milisegundos a segundos
     MS = 0.86/Tiempo1;                                                             //Calculamos los metros por segundo teniendo en cuenta la medida de la rueda
     velocidad1 = MS * 3.6;                                                         //Pasamos los metros por segundo a kilometros por hora
   
     contInt=LOW;
     interrupts();                                                                  //Hacemos una interrupcion para hacer los calculos solo cuando detecta
   }
}


void intVelocidad (){
      noInterrupts();                                                               
      vuelta=millis() - Tiempo;                                                     //Calculamos el tiempo que tarda en dar una vuelta la rueda
      Tiempo = millis();                                                            //Hacemos que "tiempo" sea igual que millis
      contInt=true;
}


//////////////////////////////////////////// ACELERADOR //////////////////////////////////////

void acelerador (void){                                                             //Creamos la funcion del acelerador
valor_pot = analogRead(A0);                                                         //Leemos el valor del potenciometro

  if (valor_pot>190){                                                               //Hacemos que el valor del potenciometro no pase de 190
    valor_pot=190;
  }

//////////////////////// MODO3 ////////////////////

    pwm3 = map ( valor_pot, 0, 190, 50, 135 );                                      //El valor maximo que mandamos es 135 porque sino el motor deja de hacelerar
                                                                                    //El valor minimo ponemos que sea 50 porque es cuando empieza a acelerar
    if (pwm3<51){                                                                   //Hacemos que cuando el valor sea 50 mande un 0 para que cuando no estemos 
      pwm3=0;                                                                       //      acelerando no este haciendo un pequeño esfuerzo el motor
    }
    
    modo3nex = map ( valor_pot, 0, 190, 0, 100 );                                   //Hacemos otro map para mover la barra de la pantalla nextion

/////////////////////// MODO2 //////////////////////////

    pwm2 = map ( valor_pot, 0, 190, 50, 113 );                                      //Valor maximo del potenciometro en el modo_2

    
    if (pwm2<51){
      pwm2=0;
    }

    modo2nex = map ( valor_pot, 0, 190, 0, 100 );                                   //Hacemos otro map para mover la barra de la pantalla nextion

//////////////////////// MODO1 ///////////////////////////
    
    pwm1 = map ( valor_pot, 0, 190, 50, 102 );                                      //Valor maximo del potenciometro en el modo_1

    if (pwm1<51){
      pwm1=0;
    }

    modo1nex = map ( valor_pot, 0, 190, 0, 100 );                                   //Hacemos otro map para mover la barra de la pantalla nextion
} 
    

////////////////////////////////////////////// BATERIA ///////////////////////////////////////////////

void nivel_bateria (void){                                                          //Creamos la funcion del nivel de bateria
  for(x=0; x<30; x++){                                                              //Hacemos un for para hacer una media con los valores leidos
    bateria=analogRead(A1);                                                         //Leemos el valor del divisor de tension
    if (bateria>903)                                                                
      {
       bateria=904;                                                                 //Hacemos que el valor maxiomo que nos mande no pase de 904
      }
    if (bateria<798)
      {
       bateria=797;                                                                 //Hacemos que el valor minimo que nos mande no baje de 797
      }
       media=media+bateria;                                                         //Sumamos los 30 valores leidos para hacer la media
      }
    nivel=media/30;                                                                 //Dividimos entre 30 para sacar la media
    media=0;                                                                        //Ponemos media en 0 para calcular otra vez la media

    valor_bateria = map (nivel, 797, 904, 0, 100);                                  //Hacemos un map para mostrar la vateria en la pantalla
}
