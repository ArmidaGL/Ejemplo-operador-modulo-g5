/*
 * Estructura básica
 * Por: Armida González Lorence
 * Fecha: 8 de abril de 2022
 * 
 * Esto es una muestra de laestructura básica de un programa
 */

// Bibliotecas
//# include <Serial.h> //No es necesario incluirla

// Constantes

// Variables
int dato=0;

// Definición de objetos

// Condiciones iniciales - Se ejecuta sólo una vez al energizar
void setup() {// Inicio de void setup ()
  // Aquí va tu código
  Serial.begin(115200); //Inicialización del monitor serial para el ESP32Cam
  Serial.println("Conexión serial iniciada");// mensaje de prueba
  delay(5000);//Espera
}// Fin de void setup

// Cuerpo del programa - Se ejecuta constamente
void loop() {// Inicio de void loop                                                   
  // put your main code here, to run repeatedly:
  //dato= dato+1;
  //contador de anillo con comparación  
  //if (dato > 8) {
  //  dato = 0;
  //}

  //contador de anillo con operador
  dato++;
  dato%=8;
  
  Serial.print("Este es el Dato: ");
  Serial.println(dato);
  delay(1000);//Espera
}// Fin de void loop

// Funciones del usuario
