// Napisz program, który będzie działał jako stoper elektroniczny, 
// podając czas na wyświetlaczu LCD. Klawisz zielony uruchamia
// i zatrzymuje stoper.W międzyczasie upływa czas, tak że po ponownym uruchomieniu stopera wyświetla się czas zatrzymania+czas ktory upłynął
// Klawisz czerwony zatrzymuje stoper jeśli działał, 
// i resetuje wartość czasu do wartości 0. Precyzja pomiaru
// to 1 sekunda.

// na rzecz symulatora:
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);

#define RED_BUTTON 2
#define GREEN_BUTTON 4
#define DEBOUNCE_BUTTON 10UL
#define DEBOUNCE_DISPLAY 1000UL

unsigned long time_amplitude = 0UL;
unsigned long before_last_time_break = 0UL;
unsigned long last_time_break = 0UL;
unsigned long last_time_stop = 0UL;
unsigned long previous_time_sum = 0UL;
bool isTimerRunning = false;
bool isTimerReseted = false;

void initButtons()
{
    pinMode(RED_BUTTON, INPUT_PULLUP);
    pinMode(GREEN_BUTTON, INPUT_PULLUP);
}

void initLCD()
{
  lcd.init();
  lcd.clear();
  lcd.backlight();
}

bool isButtonPressedTemplate(int button, int& debounced_button_state, 
int& previous_reading, unsigned long& last_change_time)
{
    bool isPressed = false;

    int current_reading = digitalRead(button);

    if (previous_reading != current_reading)
    {
        last_change_time = millis();
    }

    if (millis() - last_change_time > DEBOUNCE_BUTTON)
    {
        if (current_reading != debounced_button_state)
        {
            if (debounced_button_state == HIGH && current_reading == LOW)
            {
                isPressed = true;
            }
            debounced_button_state = current_reading;
        }
    }

    previous_reading = current_reading;

    return isPressed;
}


bool isGreenButtonPressed() {
    static int debounced_button_state = HIGH;
    static int previous_reading = HIGH;
    static unsigned long last_change_time = 0UL;
    return isButtonPressedTemplate(GREEN_BUTTON, debounced_button_state, 
    previous_reading, last_change_time);
}

bool isRedButtonPressed() {
    static int debounced_button_state = HIGH;
    static int previous_reading = HIGH;
    static unsigned long last_change_time = 0UL;
    return isButtonPressedTemplate(RED_BUTTON, debounced_button_state, 
    previous_reading, last_change_time);
}

bool isButtonReliesedTemplate(int button, int& debounced_button_state, 
int& previous_reading, unsigned long& last_change_time)
{
    bool isReliesed = false;

    int current_reading = digitalRead(button);

    if (previous_reading != current_reading)
    {
        last_change_time = millis();
    }

    if (millis() - last_change_time > DEBOUNCE_BUTTON)
    {
        if (current_reading != debounced_button_state)
        {
            if (debounced_button_state == LOW && current_reading == HIGH)
            {
                isReliesed = true;
            }
            debounced_button_state = current_reading;
        }
    }

    previous_reading = current_reading;

    return isReliesed;
}


bool isGreenButtonReliesed() {
    static int debounced_button_state = LOW;
    static int previous_reading = LOW;
    static unsigned long last_change_time = 0UL;
    return isButtonReliesedTemplate(GREEN_BUTTON, debounced_button_state, 
    previous_reading, last_change_time);
}

bool isRedButtonReliesed() {
    static int debounced_button_state = LOW;
    static int previous_reading = LOW;
    static unsigned long last_change_time = 0UL;
    return isButtonReliesedTemplate(RED_BUTTON, debounced_button_state, 
    previous_reading, last_change_time);
}


void setup() {
    initButtons();
    initLCD();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("0:00");
}

void displayString(String str_val) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(str_val);
}

void displayValue(unsigned long current_time) {
  static unsigned long last_displayed = 0UL;
  if (last_displayed != current_time) {
    lcd.clear();
    lcd.setCursor(0,0);
    int secs = current_time/1000;
    int mins = secs/60;
    secs = secs - 60*mins;
    lcd.print(mins);
    lcd.print(":");
    if (secs < 10) {
      lcd.print(0);
    }
    lcd.print(secs);
    last_displayed = current_time;
  }
  
}

void displayUpdatedTime() {
   if (isTimerReseted)
 {
    last_time_break = 0UL;
    last_time_stop = 0UL;
    time_amplitude = 0UL;
    previous_time_sum = 0UL;
    isTimerReseted = false;
 }
  static unsigned long temp_measurement = 0UL;
  if (millis() - temp_measurement > DEBOUNCE_DISPLAY) {
    temp_measurement = millis();
    if (isTimerRunning) {
      displayValue(millis() - last_time_break + previous_time_sum + time_amplitude);
    
    }else if (isTimerRunning==false){
      displayValue(previous_time_sum+time_amplitude);
    } //f (isTimerReseted==true){
      //displayValue(previous_time_sum);
      //isTimerReseted = false;
    //}
  }
}

void startTimer() {
 // before_last_time_break = last_time_break;

    last_time_break = millis();
 
  
  if (last_time_stop == 0UL)
   {
      time_amplitude = 0UL;
  } else
  {
      time_amplitude += millis() - last_time_stop;
  }

  isTimerRunning = true;
}
 
void stopTimer() {
  last_time_stop = millis();
  unsigned long last_measurement = millis() - last_time_break;
  previous_time_sum += last_measurement;
  isTimerRunning = false;
}

void resetTimer() {
  isTimerRunning = false;
  isTimerReseted = true;
  previous_time_sum = 0UL;
  //time_amplitude = 0UL;
}

void loop() {
  static bool wasGreenReliesed = true;
  static bool wasRedReliesed = true;

  if (isGreenButtonPressed() && wasGreenReliesed) {
    isTimerRunning = !isTimerRunning;
    wasGreenReliesed = false;
    if(isTimerRunning) {
      startTimer();
      displayString("start");
    }
    else {
      stopTimer();
      displayString("stop");
    }
  }
  if (isGreenButtonReliesed()){
    wasGreenReliesed = true;
  }
  

  if (isRedButtonPressed() && wasRedReliesed) {
    wasRedReliesed = false;
    resetTimer();
    displayString("reset");
  }
  if (isRedButtonReliesed()) {
    wasRedReliesed = true;
  }
  
  displayUpdatedTime();
}
