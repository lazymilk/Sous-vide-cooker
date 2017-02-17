#if (SOUS_VIDE_CONFIG_LCD_KEYPAD == ENABLE)

#define MENU_STR_TITLE      "Attic Sous Vide "
#define MENU_STR_MENU       "      MENU      "
#define MENU_STR_TIME       "    Set Time    "
#define MENU_STR_TEMP       "Set Temperature "
#define MENU_STR_START      "    Get Start   "
#define MENU_STR_YES_NO     " ( )NO   ( )YES "

enum {
  MENU_STATE_MAIN = 0,
  MENU_STATE_TIME,
  MENU_STATE_TIME_SET,
  MENU_STATE_TEMPERATURE,
  MENU_STATE_TEMPERATURE_SET,
  MENU_STATE_START,
  MENU_STATE_START_SET
};

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

int keyTask()
{
  static int key_state = 0;
  static int key_hold = LCD_KEYPAD_NONE;
  static long timestamp;
  switch (key_state) {
    default:
    case KEY_STATE_IDLE:
      key_hold = getButtons();
      if (key_hold != LCD_KEYPAD_NONE) {
        key_state = KEY_STATE_PUSH;
        /* stote timestamp */
        timestamp = millis();
      }
      break;

    case KEY_STATE_PUSH:
      if (key_hold != getButtons()) {
        key_state = KEY_STATE_IDLE;
      } else if (millis() >= timestamp + SOUS_VIDE_BUTTON_DELAY) {
        key_state = KEY_STATE_HOLD;
      }
      break;

    case KEY_STATE_HOLD:
      key_event = key_hold;
      key_state = KEY_STATE_RELEASE;
      /* store timestamp */
      timestamp = millis();
      break;

    case KEY_STATE_RELEASE:
      if (key_hold != getButtons()) {
        key_state = KEY_STATE_IDLE;
      }
      break;
  }
}

void calcRestTime(long target, char *str)
{
  long temp_tm = (target - millis()) / 1000;   /* in sec */
  int temp_min, temp_sec;
  
  if (str == NULL) {
    return;
  }

  /* rest min =  */
  temp_min = temp_tm / 60;
  temp_sec = temp_tm % 60;
  sprintf(str, "%03d:%02d", temp_min, temp_sec);
  return;
}

int menuTask()
{
  static int menu_state = MENU_STATE_MAIN;
  static int menu_tm = millis();
  const char degree = 0xdf;
  float f = 0;
  char set_value[4];
  static int curosr;
  char str[16];

  keyTask();

  switch (menu_state) {
    default:
    case MENU_STATE_MAIN:
      /* refresh screen */
      if ((millis() - menu_tm) >= 1000) {
        menu_tm = millis();
        lcd.setCursor(0, 1);
        if (fahrenheit) {
          f = current_temperature * 9 / 5 + 32;
          lcd.print(f);
          lcd.print(degree);
          lcd.print('F');
        } else {
          lcd.print(current_temperature);
          lcd.print(degree);
          lcd.print('C');
        }
        lcd.setCursor(10, 1);
        if (running_flag) {
          /* print rest of time */
          calcRestTime(target_timestamp, str);
          lcd.print(str);
        } else {
          lcd.print(" Idle ");
        }
      }

      /* handle the key event */
      if (key_event != LCD_KEYPAD_NONE) {
        if (key_event == LCD_KEYPAD_DOWN) {
          fahrenheit = 1;
        } else if (key_event == LCD_KEYPAD_UP) {
          fahrenheit = 0;
        } else if (key_event == LCD_KEYPAD_SELECT) {
          /* chage to Time page */
          menu_state = MENU_STATE_TIME;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(MENU_STR_MENU);
          lcd.setCursor(0, 1);
          lcd.print(MENU_STR_TIME);
        }
        key_event = LCD_KEYPAD_NONE;
      }

      break;

    case MENU_STATE_TIME:
      /* handle the key event */
      if (key_event != LCD_KEYPAD_NONE) {
        if (key_event == LCD_KEYPAD_UP) {
          /* chage to main page */
          menu_state = MENU_STATE_MAIN;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(MENU_STR_TITLE);
        } else if (key_event == LCD_KEYPAD_RIGHT) {
          /* chage to temperature page */
          menu_state = MENU_STATE_TEMPERATURE;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(MENU_STR_MENU);
          lcd.setCursor(0, 1);
          lcd.print(MENU_STR_TEMP);
        } else if (key_event == LCD_KEYPAD_LEFT) {
          /* chage to start page */
          menu_state = MENU_STATE_START;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(MENU_STR_MENU);
          lcd.setCursor(0, 1);
          lcd.print(MENU_STR_START);
        } else if (key_event == LCD_KEYPAD_SELECT) {
          /* chage to set time page */
          menu_state = MENU_STATE_TIME_SET;
          sprintf(set_value, "%03d", heating_time);

          lcd.clear();
          lcd.blink();
          lcd.setCursor(0, 0);
          lcd.print(MENU_STR_TIME);
          lcd.setCursor(7, 1);
          lcd.print(set_value);
          lcd.setCursor(7, 1);
          curosr = 7;
        }
        key_event = LCD_KEYPAD_NONE;
      }
      break;

    case MENU_STATE_TIME_SET:
      /* handle the key event */
      if (key_event != LCD_KEYPAD_NONE) {
        if (key_event == LCD_KEYPAD_UP) {
          /* count up */
          set_value[curosr - 7] += 1;
          if (set_value[curosr - 7] > '9') {
            set_value[curosr - 7] = '0';
          } else if (set_value[curosr - 7] < '0') {
            set_value[curosr - 7] = '9';
          }
        } else if (key_event == LCD_KEYPAD_DOWN) {
          /* count down */
          set_value[curosr - 7] -= 1;
          if (set_value[curosr - 7] > '9') {
            set_value[curosr - 7] = '0';
          } else if (set_value[curosr - 7] < '0') {
            set_value[curosr - 7] = '9';
          }
        } else if (key_event == LCD_KEYPAD_RIGHT) {
          /* move the cursor */
          curosr += 1;
          if (curosr > 9) curosr = 9;
        } else if (key_event == LCD_KEYPAD_LEFT) {
          /* move the cursor */
          curosr -= 1;
          if (curosr < 7) curosr = 7;
        } else if (key_event == LCD_KEYPAD_SELECT) {
          /* save the value and turn back */
          /* FIXME: check the value range */
          heating_time = atoi(set_value);
          menu_state = MENU_STATE_TIME;
          key_event = LCD_KEYPAD_NONE;

          lcd.clear();
          lcd.noBlink();
          lcd.setCursor(0, 0);
          lcd.print(MENU_STR_MENU);
          lcd.setCursor(0, 1);
          lcd.print(MENU_STR_TIME);
          break;
        }
        key_event = LCD_KEYPAD_NONE;

        /* update screen */
        lcd.setCursor(7, 1);
        lcd.print(set_value);
        lcd.setCursor(curosr, 1);
      }
      break;

    case MENU_STATE_TEMPERATURE:
      /* handle the key event */
      if (key_event != LCD_KEYPAD_NONE) {
        if (key_event == LCD_KEYPAD_UP) {
          /* chage to main page */
          menu_state = MENU_STATE_MAIN;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(MENU_STR_TITLE);
        } else if (key_event == LCD_KEYPAD_RIGHT) {
          /* chage to start page */
          menu_state = MENU_STATE_START;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(MENU_STR_MENU);
          lcd.setCursor(0, 1);
          lcd.print(MENU_STR_START);
        } else if (key_event == LCD_KEYPAD_LEFT) {
          /* chage to time page */
          menu_state = MENU_STATE_TIME;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(MENU_STR_MENU);
          lcd.setCursor(0, 1);
          lcd.print(MENU_STR_TIME);
        } else if (key_event == LCD_KEYPAD_SELECT) {
          /* chage to set temperature page */
          menu_state = MENU_STATE_TEMPERATURE_SET;
          sprintf(set_value, "%03d", (int)target_temperature);

          lcd.clear();
          lcd.cursor();
          lcd.blink();
          lcd.setCursor(0, 0);
          lcd.print(MENU_STR_TEMP);
          lcd.setCursor(7, 1);
          lcd.print(set_value);
          lcd.setCursor(7, 1);
          curosr = 7;
        }
        key_event = LCD_KEYPAD_NONE;
      }
      break;

    case MENU_STATE_TEMPERATURE_SET:
      /* handle the key event */
      if (key_event != LCD_KEYPAD_NONE) {
        if (key_event == LCD_KEYPAD_UP) {
          /* count up */
          set_value[curosr - 7] += 1;
          if (set_value[curosr - 7] > '9') {
            set_value[curosr - 7] = '0';
          } else if (set_value[curosr - 7] < '0') {
            set_value[curosr - 7] = '9';
          }
        } else if (key_event == LCD_KEYPAD_DOWN) {
          /* count down */
          set_value[curosr - 7] -= 1;
          if (set_value[curosr - 7] > '9') {
            set_value[curosr - 7] = '0';
          } else if (set_value[curosr - 7] < '0') {
            set_value[curosr - 7] = '9';
          }
        } else if (key_event == LCD_KEYPAD_RIGHT) {
          /* move the cursor */
          curosr += 1;
          if (curosr > 9) curosr = 9;
        } else if (key_event == LCD_KEYPAD_LEFT) {
          /* move the cursor */
          curosr -= 1;
          if (curosr < 7) curosr = 7;
        } else if (key_event == LCD_KEYPAD_SELECT) {
          /* save the value and turn back */
          /* FIXME: check the value range */
          target_temperature = (float)atoi(set_value);
          menu_state = MENU_STATE_TEMPERATURE;
          key_event = LCD_KEYPAD_NONE;

          lcd.clear();
          lcd.noBlink();
          lcd.setCursor(0, 0);
          lcd.print(MENU_STR_MENU);
          lcd.setCursor(0, 1);
          lcd.print(MENU_STR_TEMP);
          break;
        }
        key_event = LCD_KEYPAD_NONE;

        /* update screen */
        lcd.setCursor(7, 1);
        lcd.print(set_value);
        lcd.setCursor(curosr, 1);
      }
      break;

    case MENU_STATE_START:
      /* handle the key event */
      if (key_event != LCD_KEYPAD_NONE) {
        if (key_event == LCD_KEYPAD_UP) {
          /* chage to main page */
          menu_state = MENU_STATE_MAIN;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(MENU_STR_TITLE);
        } else if (key_event == LCD_KEYPAD_RIGHT) {
          /* chage to time page */
          menu_state = MENU_STATE_TIME;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(MENU_STR_MENU);
          lcd.setCursor(0, 1);
          lcd.print(MENU_STR_TIME);
        } else if (key_event == LCD_KEYPAD_LEFT) {
          /* chage to temperature page */
          menu_state = MENU_STATE_TEMPERATURE;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(MENU_STR_MENU);
          lcd.setCursor(0, 1);
          lcd.print(MENU_STR_TEMP);
        } else if (key_event == LCD_KEYPAD_SELECT) {
          /* chage to start page */
          menu_state = MENU_STATE_START_SET;

          lcd.clear();
          lcd.cursor();
          lcd.blink();
          lcd.setCursor(0, 0);
          lcd.print(MENU_STR_TEMP);
          lcd.setCursor(0, 1);
          lcd.print(MENU_STR_YES_NO);
          curosr = 2;
          lcd.setCursor(2, 1);
        }
        key_event = LCD_KEYPAD_NONE;
      }
      break;

    case MENU_STATE_START_SET:
      /* handle the key event */
      if (key_event != LCD_KEYPAD_NONE) {
        if ((key_event == LCD_KEYPAD_RIGHT) || (key_event == LCD_KEYPAD_LEFT)) {
          /* move the cursor */
          curosr = (curosr >= 8) ? (2) : (10);
        } else if (key_event == LCD_KEYPAD_SELECT) {
          if (curosr == 10) {
            /* yes */
            running_flag = (running_flag) ? (0) : (1);
            if (running_flag) {
              target_timestamp = millis() + (long)heating_time * 60 * 1000;
            } else {
              target_timestamp = 0;
            }
          }
          menu_state = MENU_STATE_START;
          key_event = LCD_KEYPAD_NONE;

          lcd.clear();
          lcd.noBlink();
          lcd.setCursor(0, 0);
          lcd.print(MENU_STR_MENU);
          lcd.setCursor(0, 1);
          lcd.print(MENU_STR_START);
          break;
        }
        key_event = LCD_KEYPAD_NONE;

        /* update screen */
        lcd.setCursor(curosr, 1);
      }
      break;
  }
}

#endif

