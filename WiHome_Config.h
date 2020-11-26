#define SERIAL_DEBUG true

// // Pin config Gosund wall switch V1
// #define PIN_LED     13
// #define PIN_BUTTON  0
// #define PIN_RELAY   12
// #define PIN_LED_ACTIVE_LOW true
// #define PIN_RELAY_ACTIVE_LOW false

// // Pin config Gosund wall switch SW1_V1.2
// // #define PIN_LED     16 // RED
// #define PIN_LED     1 // GREEN but also Serial TX
// #define PIN_BUTTON  0
// #define PIN_RELAY   14
// #define PIN_LED_ACTIVE_LOW true
// #define PIN_RELAY_ACTIVE_LOW false

// // Pin config Gosund wall switch SW1_V1.2
// #define WIFI_LED 1
// #define N_LEDS      2
// const unsigned int LED_PIN[] = {1,16};  // 1 = GREEN (also U0TXD), 16 = RED
// const unsigned int LED_INIT[] = {SLED_OFF,SLED_OFF};
// const unsigned int LED_ACTIVE_LOW[] = {true,true};
// const unsigned int LED_RELAY[] = {0,-1}; // Use -1 if no relay status should be displayed
// #define N_BUTTONS   1
// const unsigned int BUTTON_PIN[] = {0};
// const unsigned int BUTTON_RELAY[] = {0}; // Use -1 if no relay should be impacted by action=1
// #define N_RELAYS    1
// const unsigned int RELAY_PIN[] = {14};
// const unsigned int RELAY_INIT[] = {SLED_OFF};
// const unsigned int RELAY_ACTIVE_LOW[] = {false};

// // Pin config Sonoff Basic RF R2 POWER V1.0
// #define PIN_LED     13
// #define PIN_BUTTON  0
// #define PIN_RELAY   12
// #define PIN_LED_ACTIVE_LOW true
// #define PIN_RELAY_ACTIVE_LOW false

// // Pin config Sonoff Touch EU WIFI V1.0 (2016-8-3)
// #define PIN_LED     13
// #define PIN_BUTTON  0
// #define PIN_RELAY   12
// #define PIN_LED_ACTIVE_LOW true
// #define PIN_RELAY_ACTIVE_LOW false

// // Pin config Sonoff Basic R2 V1.0 (2017-10-11)
// #define PIN_LED     13
// #define PIN_BUTTON  0
// #define PIN_RELAY   12
// #define PIN_LED_ACTIVE_LOW true
// #define PIN_RELAY_ACTIVE_LOW false

// // Pin config Sonoff POW Ver 2.0
// #define PIN_LED     15
// #define PIN_BUTTON  0
// #define PIN_RELAY   12
// #define PIN_LED_ACTIVE_LOW false
// #define PIN_RELAY_ACTIVE_LOW false

// // Test
// #define WIFI_LED 0
// #define N_LEDS      2
// const unsigned int LED_PIN[] = {13,16};
// const unsigned int LED_INIT[] = {SLED_OFF,SLED_OFF};
// const unsigned int LED_ACTIVE_LOW[] = {false,false};
// const unsigned int LED_RELAY[] = {1,0}; // Use -1 if no relay status should be displayed
// #define N_BUTTONS   2
// const unsigned int BUTTON_PIN[] = {0,2};
// const unsigned int BUTTON_RELAY[] = {0,1}; // Use -1 if no relay should be impacted by action=1
// #define N_RELAYS    2
// const unsigned int RELAY_PIN[] = {12,14};
// const unsigned int RELAY_INIT[] = {SLED_OFF,SLED_OFF};
// const unsigned int RELAY_ACTIVE_LOW[] = {false, false};

// // Pin config Gosund wall switch V1
// #define WIFI_LED 0
// #define N_LEDS      1
// const unsigned int LED_PIN[] = {13};  // 13 = GREEN, 14 = RED
// const unsigned int LED_INIT[] = {SLED_OFF};
// const unsigned int LED_ACTIVE_LOW[] = {true};
// const unsigned int LED_RELAY[] = {0}; // Use -1 if no relay status should be displayed
// #define N_BUTTONS   1
// const unsigned int BUTTON_PIN[] = {0};
// const unsigned int BUTTON_RELAY[] = {0}; // Use -1 if no relay should be impacted by action=1
// #define N_RELAYS    1
// const unsigned int RELAY_PIN[] = {12};
// const unsigned int RELAY_INIT[] = {SLED_OFF};
// const unsigned int RELAY_ACTIVE_LOW[] = {false};

// // Pin config Sparkfun Thing Dev Board
// #define WIFI_LED 0
// #define N_LEDS      1
// const unsigned int LED_PIN[] = {13};  // 13 = GREEN, 14 = RED
// const unsigned int LED_INIT[] = {SLED_OFF};
// const unsigned int LED_ACTIVE_LOW[] = {false};
// const unsigned int LED_RELAY[] = {0}; // Use -1 if no relay status should be displayed
// #define N_BUTTONS   2
// const unsigned int BUTTON_PIN[] = {0,2};
// const unsigned int BUTTON_RELAY[] = {0,1}; // Use -1 if no relay should be impacted by action=1
// #define N_RELAYS    2
// const unsigned int RELAY_PIN[] = {16,14};
// const unsigned int RELAY_INIT[] = {SLED_OFF,SLED_OFF};
// const unsigned int RELAY_ACTIVE_LOW[] = {false, false};

// Pin config Sparkfun Thing Dev Board RGBstrip
#define WIFI_LED 0
#define N_LEDS      1
const unsigned int LED_PIN[] = {15};   // 5 is on-board LED
const unsigned int LED_INIT[] = {SLED_OFF};
const unsigned int LED_ACTIVE_LOW[] = {false};
const unsigned int LED_RGBSTRIP[] = {0}; // Use -1 if no rgbstrip status should be displayed
#define N_BUTTONS   1
const unsigned int BUTTON_PIN[] = {0};
const unsigned int BUTTON_RGBSTRIP[] = {0}; // Use -1 if no rgbstrip should be impacted by action=1
#define N_RGBSTRIPS    1
const unsigned int RGBSTRIP_PIN[N_RGBSTRIPS][3] = {{13,4,12}};
const unsigned int RGBSTRIP_ACTIVE_LOW[] = {false};
