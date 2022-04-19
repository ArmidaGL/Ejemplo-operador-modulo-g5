#include <DHTesp.h>

/*
 * ENCENDER UN LED Y PRENDER UN BOTÓN
 * Por: Armida González
 * Fecha: 18 de abril de 2022
 * 
 * Este programa prende un led cuando se presiona un botón
 */

// Bibliotecas

// Constantes
const int BOTON = 4;
const int LED =2;

// Variables
int dato;


// Definición de objetos

// Condiciones iniciales - Se ejecuta sólo una vez al energizar
void setup() {// Inicio de void setup ()
  // Aquí va tu código
  pinMode (BOTON, INPUT_PULLUP); // Configurar el pin del botón como entrada (INPUT Ó OUTPUT)
  pinMode (LED, OUTPUT); //  Configurar el pin del led como salida de voltaje
  digitalWrite(LED, LOW); // Empieza con el led apagado //1, true, HIGH, 0, false, LOW

}// Fin de void setup

// Cuerpo del programa - Se ejecuta constamente
void loop() {// Inicio de void loop
  // put your main code here, to run repeatedly:
  dato = digitalRead(BOTON); // LEER EL PIN DEL BOTÓN
  digitalWrite (LED, !dato); // PRENDER O APAGAR EL LED
}// Fin de void loop

// Funciones del usuario
