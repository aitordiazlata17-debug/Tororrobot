/*
 * ============================================================================
 * Proyecto: TORORROBOT - Robot Mini Sumo 500g (OSHWDem)
 * Equipo: Darío Fernández Trapero y Aitor Díaz Lata
 * Microcontrolador: ATmega328P
 * ============================================================================
 */

// --- ASIGNACIÓN DE PINES ---

// Controlador de Motores (TB6612FNG)
const int PIN_MOTOR_IZQ_IN1 = 2;
const int PIN_MOTOR_IZQ_IN2 = 4;
const int PIN_MOTOR_IZQ_PWM = 3;   // Pin PWM

const int PIN_MOTOR_DER_IN1 = 7;
const int PIN_MOTOR_DER_IN2 = 8;
const int PIN_MOTOR_DER_PWM = 5;   // Pin PWM

const int PIN_STBY = 6;            // Pin Standby del driver TB6612FNG

// Sensores de Oponente (Sharp GP2Y0A21 - Infrarrojos analógicos)
const int PIN_SHARP_IZQ = A0;
const int PIN_SHARP_DER = A1;

// Sensor de Suelo / Borde (TCRT5000)
const int PIN_SENSOR_SUELO = A2;

// Receptor Módulo IR Arranque (OSHWDem) / Botón Start
const int PIN_START_IR = 9;

// --- UMBRALES Y CONFIGURACIÓN ---
const int UMBRAL_SHARP_DETECCION = 250; // Valor analógico (aprox. 20-30 cm)
const int UMBRAL_SUELO_BLANCO   = 600; // Ajustar según color de la superficie del Dohyo

// Velocidades (PWM 0-255)
const int VELOCIDAD_BUSQUEDA = 120;
const int VELOCIDAD_ATAQUE   = 255;
const int VELOCIDAD_GIRO     = 180;

void setup() {
  // Configuración de pines de motores
  pinMode(PIN_MOTOR_IZQ_IN1, OUTPUT);
  pinMode(PIN_MOTOR_IZQ_IN2, OUTPUT);
  pinMode(PIN_MOTOR_IZQ_PWM, OUTPUT);
  
  pinMode(PIN_MOTOR_DER_IN1, OUTPUT);
  pinMode(PIN_MOTOR_DER_IN2, OUTPUT);
  pinMode(PIN_MOTOR_DER_PWM, OUTPUT);
  
  pinMode(PIN_STBY, OUTPUT);
  
  // Configuración de entrada
  pinMode(PIN_START_IR, INPUT_PULLUP);

  // Habilitar driver
  digitalWrite(PIN_STBY, HIGH);
  detenerMotores();

  // ESPERA OBLIGATORIA DE SEGURIDAD / ARRANQUE
  // Si no se usa módulo IR oficial, retardo obligatorio de 5 segundos
  esperarInicio();
}

void loop() {
  int valorSuelo = analogRead(PIN_SENSOR_SUELO);
  int valorSharpIzq = analogRead(PIN_SHARP_IZQ);
  int valorSharpDer = analogRead(PIN_SHARP_DER);

  // 1. PRIORIDAD ABSOLUTA: Detección del borde blanco del Dohyo (Evasión)
  if (valorSuelo > UMBRAL_SUELO_BLANCO) {
    // Retroceder y girar rápidamente para no salir de la pista
    moverMotores(-VELOCIDAD_GIRO, -VELOCIDAD_GIRO);
    delay(250);
    moverMotores(VELOCIDAD_GIRO, -VELOCIDAD_GIRO); // Giro a la derecha
    delay(200);
  } 
  // 2. DETECCIÓN DE OPONENTE (Ataque)
  else if (valorSharpIzq > UMBRAL_SHARP_DETECCION && valorSharpDer > UMBRAL_SHARP_DETECCION) {
    // Rival detectado en el centro -> Ataque a máxima potencia
    moverMotores(VELOCIDAD_ATAQUE, VELOCIDAD_ATAQUE);
  } 
  else if (valorSharpIzq > UMBRAL_SHARP_DETECCION) {
    // Rival detectado a la izquierda -> Girar hacia la izquierda
    moverMotores(-VELOCIDAD_GIRO, VELOCIDAD_GIRO);
  } 
  else if (valorSharpDer > UMBRAL_SHARP_DETECCION) {
    // Rival detectado a la derecha -> Girar hacia la derecha
    moverMotores(VELOCIDAD_GIRO, -VELOCIDAD_GIRO);
  } 
  // 3. BÚSQUEDA
  else {
    // Girar en espiral / búsqueda lenta si no ve al rival
    moverMotores(VELOCIDAD_BUSQUEDA, -VELOCIDAD_BUSQUEDA / 2);
  }

  delay(10); // Estabilidad del bucle
}

// --- FUNCIONES AUXILIARES DE CONTROL ---

void esperarInicio() {
  // Esperar a recibir la señal de arranque o la retención de 5s
  while (digitalRead(PIN_START_IR) == HIGH) {
    // Espera señal IR
  }
  delay(5000); // Retardo obligatorio de 5 segundos según reglamento
}

void moverMotores(int velIzq, int velDer) {
  // Control Motor Izquierdo
  if (velIzq >= 0) {
    digitalWrite(PIN_MOTOR_IZQ_IN1, HIGH);
    digitalWrite(PIN_MOTOR_IZQ_IN2, LOW);
  } else {
    digitalWrite(PIN_MOTOR_IZQ_IN1, LOW);
    digitalWrite(PIN_MOTOR_IZQ_IN2, HIGH);
    velIzq = -velIzq;
  }
  analogWrite(PIN_MOTOR_IZQ_PWM, velIzq);

  // Control Motor Derecho
  if (velDer >= 0) {
    digitalWrite(PIN_MOTOR_DER_IN1, HIGH);
    digitalWrite(PIN_MOTOR_DER_IN2, LOW);
  } else {
    digitalWrite(PIN_MOTOR_DER_IN1, LOW);
    digitalWrite(PIN_MOTOR_DER_IN2, HIGH);
    velDer = -velDer;
  }
  analogWrite(PIN_MOTOR_DER_PWM, velDer);
}

void detenerMotores() {
  digitalWrite(PIN_MOTOR_IZQ_IN1, LOW);
  digitalWrite(PIN_MOTOR_IZQ_IN2, LOW);
  analogWrite(PIN_MOTOR_IZQ_PWM, 0);

  digitalWrite(PIN_MOTOR_DER_IN1, LOW);
  digitalWrite(PIN_MOTOR_DER_IN2, LOW);
  analogWrite(PIN_MOTOR_DER_PWM, 0);
}
