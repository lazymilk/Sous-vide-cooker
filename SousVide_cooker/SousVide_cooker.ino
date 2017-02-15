#include "config.h"
#if (SOUS_VIDE_CONFIG_LCD_KEYPAD == ENABLE)
#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
#endif

String command;
float current_temperature = 0;
float target_temperature = SOUS_VIDE_TARGET_TEMPERATURE;

void setup() {
  /* initial serial */
  Serial.begin(115200);
  
#if (SOUS_VIDE_CONFIG_LCD_KEYPAD == ENABLE)
  /* initial lcd keypad shield */
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print(SOUS_VIDE_TITLE);
#endif

  /* setup pin */
  pinMode(SOUS_VIDE_RELAY_PIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  static long tm;
  static int init_flag = 1;

  if (init_flag == 1)
  {
    init_flag = 0;
    tm = millis();
  }
#if (SOUS_VIDE_CONFIG_UART_COMM == ENABLE)
  if (Serial.available())
  {
    char c = Serial.read();
    if (c == '\n')
    {
      parseCommand(command);
      command = "";
    }
    else
    {
      command += c;
    }
  }
#endif

  //LOOP control in 0.1S period
  if ((millis() - tm) > 100)
  {
    tm += 100;
    //get temperature
    current_temperature = getTemperature(analogRead(SOUS_VIDE_ADC_PIN));
    //PID calculate
    //Output
    if (current_temperature >= target_temperature)
    {
      digitalWrite(SOUS_VIDE_RELAY_PIN, LOW);
    }
    else if (current_temperature < (target_temperature - SOUS_VIDE_CONTROL_IGNORE_REGION))
    {
      digitalWrite(SOUS_VIDE_RELAY_PIN, HIGH);
    }
  }
}

#if (SOUS_VIDE_CONFIG_LCD_KEYPAD == ENABLE)
int getButtons()
{
  int adc_key_in  = 1023;
  adc_key_in = analogRead(SOUS_VIDE_BUTTON_PIN);
  // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
  // we add approx 50 to those values and check to see if we are close
  // We make this the 1st option for speed reasons since it will be the most likely result
  if (adc_key_in > LCD_KEYPAD_NONE_THRESHOLD) return LCD_KEYPAD_NONE; 
  if (adc_key_in < LCD_KEYPAD_RIGHT_THRESHOLD)   return LCD_KEYPAD_RIGHT;
  if (adc_key_in < LCD_KEYPAD_UP_THRESHOLD)  return LCD_KEYPAD_UP;
  if (adc_key_in < LCD_KEYPAD_DOWN_THRESHOLD)  return LCD_KEYPAD_DOWN;
  if (adc_key_in < LCD_KEYPAD_LEFT_THRESHOLD)  return LCD_KEYPAD_LEFT;
  if (adc_key_in < LCD_KEYPAD_SELECT_THRESHOLD)  return LCD_KEYPAD_SELECT;

  return LCD_KEYPAD_NONE;  // when all others fail, return this...
}
#endif


#if (SOUS_VIDE_CONFIG_UART_COMM == ENABLE)
void parseCommand(String com)
{
  String part1 , part2;
  char op_char = ' ';
  com.toLowerCase();

  if (com.indexOf('=') != -1)
    op_char = '=';
  else   if (com.indexOf('?') != -1)
    op_char = '?';
  if (op_char != ' ')
  {
    part1 = com.substring(0, com.indexOf(op_char));
    part2 = com.substring(com.indexOf(op_char) + 1);
    part1.trim();
    part2.trim();

    if (op_char == '?') //query data
    {
      if (part1 == "t")
      {
        Serial.println(current_temperature);
        Serial.println("ok");
      }
      else if (part1 == "s")
      {
        Serial.println(target_temperature);
        Serial.println("ok");
      }
    }
    else if ((op_char == '=') && (part2.length() != 0)) // set data
    {
      if (part1 == "s")
      {
        target_temperature = part2.toFloat();
        Serial.println("ok");
      }
    }
  }
}
#endif

/*
           3.3V
            |
            R1 (250 ohm)
            +------Analog Out to arduino AD pin
           RTD
            |
           GND

    R to temperature formula
    T = (Rrtd - 100)* 0.3851
*/

#define R1 (250.0)
#define ADC_MAX (1023.0)
float getRTD_Resister( float adc)
{
  float RTD;
  /*
      adc / ADC_MAX = RTD / (R1 + RTD)
      adc * (R1 + RTD) = R_adc * ADC_MAX
      adc * R1 + adc * R_adc = R_adc * ADC_MAX
      adc * R1 = RTD * ADC_MAX - adc * RTD
      adc * R1 = (ADC_MAX - adc) * RTD
      RTD = (adc * R1) / (ADC_MAX - adc)

  */
  RTD = (adc * R1) / (ADC_MAX - adc);
  return RTD;
}

float getTemperature(int adc)
{
  float RTD, tempe;
  RTD = getRTD_Resister(adc);
  tempe = (RTD - 100) / SOUS_VIDE_RESISTOR_DEGREE;
#if (SOUS_VIDE_CONFIG_UART_DEBUG == ENABLE)
  Serial.print("Temperature = ");
  Serial.println(tempe);
#endif
  return tempe;
}

