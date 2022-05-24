/*
 * 
 * Probado por Armida González
 * 
  SisRefri: Sistema de Refrigeración Manual y Automática

  EJERCICIO DE CLASE PARA OPERADORES LÓGICOS

  Ejercicio Código IoT
  Diplomado Samsung

  Elaboró: Alberto Pacheco (apacheco@uach.mx)
  Fecha: 20/Abril/2021


  CONTEXTO (HIPOTÉTICO):
  Sistema de refrigeración de un site (servidores)

  PROBLEMA:
      Implementar la lógica de control necesaria (variables y función control)
    para que a partir de 3 botones y un sensor de temperatura (4 entradas)
    se determinen los dos valores de salida (LEDs), para indicar si se 
    activan o no los sistemas de refrigeración manual y automático del site.

  PREMISAS:
  1. La refrigeración manual se enciende cada vez que se presione el botón manual
  2. La refrigeración manual tiene prioridad a la refrigeracion automática
  3. Si la temperatura es alta (mayor a 28, por ejemplo), se activa la refrigeración automática
  4. Si tengo alta demanda o sobrecarga, se activa la refrigeración automática
  5. Si tengo alta demanda o sobrecarga de funcionamiento y ademas temperatura alta, se activan ambas refrigeraciones

  ESTRATEGIAS / TÉCNICAS:
  - Definir variables-control tipo booleano y crear tabla de verdad (mapeo entradas/salidas)
  - Simplificar la expresión booleana usando técnicas Mapas K (online tool)
  - Simplificar el código definiendo APIs para cada dispositivo usando C++
    - Definir sinónimo de tipo seguro (entero positivo 8-bits, u8) para datos de puertos y valores del hardware  
    - Definir constructores para inicializar sensores y puertos
    - Estandarizar API de lectura (read) que lee, convierte, valida y temporiza los valores de lectura de puertos
    - Redefinir operador-consola (insertor << de cout) y operador-asignación (operator=) para simplificar código
    - Definir proceso de la aplicación en 3 etapas (stages): leer entradas, procesar datos y reportar resultados
    - Zero-cost abstraction technique: inline functions
  - Simplificar puertos de entrada usando un banco de interruptores (DIP switch IC) 
    http://ardupiclab.blogspot.com/2020/04/a-dip-switch-for-arduino.html
  - Simplificar puertos de salida usando un IC driver ULN2003 para los LEDs 
    https://randomnerdtutorials.com/esp32-stepper-motor-28byj-48-uln2003/

  CONEXIONES ESP32:
  - ESP32 -> INPUT DHT11 SENSOR https://www.makerguides.com/wp-content/uploads/2019/08/DHT11-3-pin-with-Arduino-UNO-wiring-diagram-schematic-768x369.jpg
    - VCC/VIN -> VCC
    - GND-PIN -> GND
    - GPIO-13 -> SIGNAL
  - ESP32 INPUT BUTTONS
    - BTN-1: GPIO-25  (MANUAL)
    - BTN-2: GPIO-26  (ALTA-DEMANDA)
    - BTN-3: GPIO-27  (SOBRE-CARGA)
  - ESP32 -> ULN2003 DRIVER OUTPUT LEDS
    - LED-1: GPIO-12 -> DRIVER-IN1
    - LED-2: GPIO-14 -> DRIVER-IN2
    - VCC/VIN -> DRIVER-VCC
    - GND-PIN -> DRIVER-GND    

  CONEXIONES RASPBERRY NANO CP2040:
  - ESP32 -> INPUT DHT11 SENSOR
    - VCC/VIN -> VCC
    - GND-PIN -> GND
    - GPIO-13 -> SIGNAL
  - ESP32 INPUT BUTTONS
    - BTN-1: GPIO-25  (MANUAL)
    - BTN-2: GPIO-26  (ALTA-DEMANDA)
    - BTN-3: GPIO-27  (SOBRE-CARGA)
  - ESP32 -> ULN2003 DRIVER OUTPUT LEDS
    - LED-1: GPIO-12 -> DRIVER-IN1
    - LED-2: GPIO-14 -> DRIVER-IN2
    - VCC/VIN -> DRIVER-VCC
    - GND-PIN -> DRIVER-GND    

*/

#include <iostream>
#include <cassert>
#include "DHT.h"

#define _SIMU_



/// Pseudo Types (alias)
using namespace std; // using cout only
using u8 = uint8_t;  // alias for Arduino param type 

/// CONSTANTES

// Valor crítico activación de refrigeración
const float UMBRAL_TEMP = 29.0; 

//// INPUTS
const u8   DHTPIN = 12; // DHT sensor input pin
const u8   B1_MAN = 13; // Button-1 input pin
const u8  B2_ALTA = 15; // Button-2 input pin
const u8 B3_SOBRE = 14; // Button-3 input pin

//// OUTPUTS
const u8 LED_MANUAL = 2; // LED: Refri Manual output
const u8 LED_AUTO   = 4; // LED: Refri Auto output

////--> API

//// Macro to convert boolean to string
inline string b2s(bool val) { return (val? "on " : "off"); }

//// Temp sensor API
struct Temp {
  const u8 pin;
  float val;
  DHT dht;
inline Temp(u8 p): pin(p), dht(p,DHT11) { dht.begin(); } 
inline float read() {
    this->val = dht.readTemperature();
    assert(!isnan(this->val)); // Failed to read from DHT sensor!
    delay(150);
    return this->val;
  }
inline bool verif() { return val > UMBRAL_TEMP; }
  friend ostream& operator <<(ostream &os, const Temp &t) {
    os << t.val << "C";
    return os; 
  }
}; // end-Temp-API

//// LED API
struct Led {
  const u8 pin;
  bool val;
inline Led(u8 p): pin(p) { pinMode(p, OUTPUT); }
inline bool operator=(bool state) {
    this->val = state;
    digitalWrite(this->pin, this->val);
    delay(30);
    return this->val;
  }
inline friend ostream& operator <<(ostream &os, const Led &d) {
    os << b2s(d.val);
    return os; 
  }
}; // end-LED-API

//// Button API
struct Btn {
  const u8 pin;
  bool val = false;
inline Btn(u8 p): pin(p) { pinMode(p, INPUT_PULLUP); }
inline bool read() {
    this->val = digitalRead(this->pin)==LOW;  // inverse-logic
    delay(100);
    return this->val;
  }
inline friend ostream& operator <<(ostream &os, const Btn &b) {
    os << b2s(b.val);
    return os; 
  }
}; // end-Btn API

//// <-- API


//// APPLICATION

/// Device Objects
#ifndef _SIMU_
Btn  btn1(B1_MAN);     // Create manual button-1
Btn  btn2(B2_ALTA);    // Create alta-demanda button-2
Btn  btn3(B3_SOBRE);   // Create sobrecarga button-3
Temp temp(DHTPIN);     // Create temperature sensor
float old_temp = 0.0; // for temp changes
#endif
Led  led1(LED_MANUAL); // Create manual-ref led-1 
Led  led2(LED_AUTO);   // Create auto-ref led-2 

//// Global variables
bool a, b, c, d; // control variables

//// Application 3-stages

#ifndef _SIMU_
// Read and show temperature if changed 
inline void read_temp() {
 if (old_temp != temp.read()) {
    old_temp = temp.val; // update temp buffer value
    cout << "Temp = " << temp << endl; // display current temperature
  }
}

// Read all button inputs and test for changes
inline bool test_for_changes() {
  const bool ch1 = (a != temp.verif());
  const bool ch2 = (b != btn1.read());
  const bool ch3 = (c != btn2.read());
  const bool ch4 = (d != btn3.read());
  return (ch1 || ch2 || ch3 || ch4);
}

// Stage-1: get input & set control variables
inline void read_input(bool start=false) { 
  if (start) {
    old_temp = temp.read(); // at start read temperature
  }
  // set control variables
  a = temp.verif();
  b = btn1.val;
  c = btn2.val;
  d = btn3.val;
} // end-read-input
#endif

// Stage-2: control-actions using boolean expressions
inline void control() { 
    led1 = (a && d) || (a && c) || b; // Minimal Form: ad + ac + b
    led2 = a || c || d;               // Minimal Form: a + c + d
} // end-control

// Stage-3: report status
inline void report() {
#ifdef _SIMU_
  cout  << " a=" << b2s(a)
        << " b=" << b2s(b)
        << " c=" << b2s(c)
        << " d=" << b2s(d)
        << " MN="  << led1
        << " AT=" << led2 << endl;
#else
  cout  << endl
        << "High Temp? "        << b2s(a) << endl
        << "Btn-Manual = "      << btn1 << endl
        << "Btn-Alta-Dem = "    << btn2 << endl
        << "Btn-Sobrecarga = "  << btn3 << endl
        << ">> MANUAL REFR. = " << led1 << endl
        << ">> AUTOM. REFR. = " << led2 << endl
        << endl;
#endif
} // end-report

//// Arduino user-application (setup & loop)

/// INITIALIZATION
void setup() {
  led1 = led2 = false; // turn-off leds
  cout << "** Sistema de Refrigeración **" << endl; // screen title
  cout << "Umbral de temperatura: " << UMBRAL_TEMP << "C" << endl;
  delay(500);
#ifdef _SIMU_
  for (const auto &va : { false, true }) {
    a = va;
    for (const auto &vb : { false, true }) {
      b = vb;
      for (const auto &vc : { false, true }) {
        c = vc;
        for (const auto &vd : { false, true }) {
          d = vd;
          control();
          report();
          delay(250);
        }
      }
    }
  }
#else
  read_input(true); // read initial values
  report(); // initial report
#endif
} // end-setup

/// CONTROL-LOOP
void loop() {
#ifndef _SIMU_
  read_temp(); // Read and show temperature if changed
  auto any_change = test_for_changes(); // Read button-inputs & test for changes
  if (any_change) { // // Test if inputs have any changes to execute all-3-stages 
    read_input(); // stage-1: read & update control variables
    control();    // stage-2: take control action
    report();     // stage-3: show status
  }
  delay(500);
#endif
} // end-loop
