
#define ENABLE                          (1)
#define DISABLE                         (0)

/* function configuration */
#define SOUS_VIDE_CONFIG_LCD_KEYPAD     ENABLE
#define SOUS_VIDE_CONFIG_UART_DEBUG     DISABLE
#define SOUS_VIDE_CONFIG_UART_COMM      ENABLE
#define SOUS_VIDE_CONFIG_OPAMP          ENABLE

/* hardware configuration */
#define SOUS_VIDE_RELAY_PIN             (3)
#define SOUS_VIDE_ADC_PIN               (1)
#define SOUS_VIDE_BUTTON_PIN            (0)
#define SOUS_VIDE_TITLE                 "Attic Sous Vide"
#define SOUS_VIDE_DIGITAL_FILTER        ENABLE
#define SOUS_VIDE_VOLTAGE_BASIC         (0.105)         /* unit: V */
#define SOUS_VIDE_VOLTAGE_OFFSET        (0.12)          /* unit: V */
#define SOUS_VIDE_VOLTAGE_GAIN          (101)
#define SOUS_VIDE_RESISTOR_DEGREE       (0.3851)        /* raise resistor pre drgree */
#define SOUS_VIDE_R1                    (250.0)
#define SOUS_VIDE_ADC_MAX               (1023.0)
#define SOUS_VIDE_VDD_VOLTAGE           (5.0)
#define SOUS_VIDE_BUTTON_DELAY          (40)            /* unit: ms */

/* software configuration */
#define SOUS_VIDE_DIGITAL_FILTER        ENABLE
#define SOUS_VIDE_BUFFER_SIZE           (10)
#define SOUS_VIDE_TARGET_TEMPERATURE    (60.0)          /* unit: degree C */
#define SOUS_VIDE_CONTROL_IGNORE_REGION (1.2)           /* unit: degree C */
#define SOUS_VIDE_DEFAULT_TIME          (60)            /* unit: minute */

/* LCD keypad shiled definition */
#define LCD_KEYPAD_RIGHT                0
#define LCD_KEYPAD_UP                   1
#define LCD_KEYPAD_DOWN                 2
#define LCD_KEYPAD_LEFT                 3
#define LCD_KEYPAD_SELECT               4
#define LCD_KEYPAD_NONE                 5
#define LCD_KEYPAD_RIGHT_THRESHOLD      50
/* V1.1 threshold*/
#define LCD_KEYPAD_UP_THRESHOLD         250
#define LCD_KEYPAD_DOWN_THRESHOLD       450
#define LCD_KEYPAD_LEFT_THRESHOLD       650
#define LCD_KEYPAD_SELECT_THRESHOLD     850
/* V1.0 threshold*/
/*
#define LCD_KEYPAD_RIGHT_THRESHOLD      50
#define LCD_KEYPAD_UP_THRESHOLD         195
#define LCD_KEYPAD_DOWN_THRESHOLD       380
#define LCD_KEYPAD_LEFT_THRESHOLD       555
#define LCD_KEYPAD_SELECT_THRESHOLD     790
 */
#define LCD_KEYPAD_NONE_THRESHOLD       1000

#define KEY_STATE_IDLE      0
#define KEY_STATE_PUSH      1
#define KEY_STATE_HOLD      2
#define KEY_STATE_RELEASE   3


/* address of EEPROM */
#define SOUS_VIDE_EEPROM_ADDR_TEMPERATURE           0x10
#define SOUS_VIDE_EEPROM_ADDR_TIME                  0x20

#define SOUS_VIDE_TEMPERATURE_MAX                   105
#define SOUS_VIDE_TEMPERATURE_MIN                   1
#define SOUS_VIDE_TIME_MAX                          999
#define SOUS_VIDE_TIME_MIN                          1

