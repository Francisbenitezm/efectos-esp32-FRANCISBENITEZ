#define LED_PWM 14
#define BOTON 4
#define LED1 15
#define LED2 2
#define LED3 5
#define LED4 18
#define LED5 19

int leds[5] = {LED1, LED2, LED3, LED4, LED5};

volatile int counter = 0;

hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

int estado = 1;
int ultimoEstadoBoton = HIGH;

int pwm = 0;
int direccionPWM = 1;

int ledActual = 0;

void IRAM_ATTR onTimer()
{
  portENTER_CRITICAL_ISR(&timerMux);
  counter++;
  portEXIT_CRITICAL_ISR(&timerMux);
}

void setup()
{
  Serial.begin(9600);

  pinMode(LED_PWM, OUTPUT);
  pinMode(BOTON, INPUT_PULLUP);

  for (int i = 0; i < 5; i++)
  {
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], LOW);
  }

  timer = timerBegin(1000000);
  timerAttachInterrupt(timer, &onTimer);
  timerAlarm(timer, 10000, true, 0); 
}

void loop()
{
  int estadoBoton = digitalRead(BOTON);

  if (ultimoEstadoBoton == HIGH && estadoBoton == LOW)
  {
    if (estado == 1)
    {
      estado = 2;
      counter = 0;
      apagarTodos();
      analogWrite(LED_PWM, 0);
      Serial.println("Cambio a Estado 2: Ruleta Rusa");
    }
    else
    {
      estado = 1;
      counter = 0;
      apagarTodos();
      Serial.println("Cambio a Estado 1: Respiracion PWM");
    }
  }

  ultimoEstadoBoton = estadoBoton;

  if (estado == 1)
  {
    respiracionPWM();
  }
  else if (estado == 2)
  {
    ruletaRusa();
  }
}

void respiracionPWM()
{
  if (counter >= 2)
  {
    counter = 0;

    analogWrite(LED_PWM, pwm);

    Serial.print("PWM: ");
    Serial.println(pwm);

    pwm = pwm + direccionPWM;

    if (pwm >= 255)
    {
      pwm = 255;
      direccionPWM = -1;
    }

    if (pwm <= 0)
    {
      pwm = 0;
      direccionPWM = 1;
    }
  }
}

void ruletaRusa()
{
  if (counter >= 50)
  {
    counter = 0;

    int mascara = 1 << ledActual;

    for (int i = 0; i < 5; i++)
    {
      if (mascara & (1 << i))
      {
        digitalWrite(leds[i], HIGH);
      }
      else
      {
        digitalWrite(leds[i], LOW);
      }
    }

    Serial.print("LED encendido: ");
    Serial.println(ledActual + 1);

    ledActual++;

    if (ledActual >= 5)
    {
      ledActual = 0;
    }
  }
}

void apagarTodos()
{
  for (int i = 0; i < 5; i++)
  {
    digitalWrite(leds[i], LOW);
  }
}
