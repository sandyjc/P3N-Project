#include <SPI.h>
#include <mpu9255_esp32.h>
#include <TFT_eSPI.h>
#include <WiFiClientSecure.h>
#include<math.h>
#include<string.h>
//WiFiClientSecure is a big library. It can take a bit of time to do that first compile

char system_state[100] = "UI";
char old_system_state[100];


/**
 * These are global variables that 
 * will be used for the UI
 */ 
TFT_eSPI tft = TFT_eSPI();
const int SCREEN_HEIGHT = 160;
const int SCREEN_WIDTH = 128;
const int BUTTON_PIN = 5;
char old_input[100]; //char array buffer to hold HTTP request
char input[100]; //char array buffer to hold user's username

int state = 0;
int writing_state = 0;
int selector = 0;
int old_selector = -1;
char USERNAME[100];
char OWNER[100];
char LOCATION[100];
//-----------------------------------

char network[] = "6s08";  //SSID for 6.08 Lab
char password[] = "iesc6s08"; //Password for 6.08 Lab
//char network[] = "MIT GUEST";
//char password[] = "";
char host[] = "608dev.net";
char thing[1000];
char request[1500];


const int DELAY = 1000;
const int SAMPLE_FREQ = 8000;                          // Hz, telephone sample rate
const int SAMPLE_DURATION = 5;                        // duration of fixed sampling (seconds)
const int NUM_SAMPLES = SAMPLE_FREQ * SAMPLE_DURATION;  // number of of samples
const int ENC_LEN = (NUM_SAMPLES + 2 - ((NUM_SAMPLES + 2) % 3)) / 3 * 4;  // Encoded length of clip

const uint16_t RESPONSE_TIMEOUT = 6000;

const uint16_t IN_BUFFER_SIZE = 1000; //size of buffer to hold HTTP request
const uint16_t OUT_BUFFER_SIZE = 1000; //size of buffer to hold HTTP response
char request_buffer[IN_BUFFER_SIZE]; //char array buffer to hold HTTP request
char response_buffer[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP response

char response[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP request

/* CONSTANTS */
//Prefix to POST request for Google API:
const char PREFIX[] = "{\"config\":{\"encoding\":\"MULAW\",\"sampleRateHertz\":8000,\"languageCode\": \"en-US\",\"speechContexts\": [{\"phrases\": [\"red\", \"maroon\", \"brown\", \"tan\", \"peach\", \"black\", \"white\", \"green\", \"yellow\", \"blue\",\"orange\",\"indigo\",\"violet\",\"change background\", \"change color\", \"lock\", \"unlock\", \"x\", \"y\", \"z\", \"rotate\", \"delete\", \"place ball size\", \"place box size\", \"change line color\", \"change line width\", \"change line size\", \"show grid\", \"show axis\", \"hide grid\", \"hide axis\", \"change background color\", \"done\", \"ready\", \"next\", \"place\", \"sphere\", \"circle\", \"ball\", \"box\", \"square\", \"cube\", \"size\", \"0\", \"1\", \"2\", \"3\", \"4\", \"5\", \"6\", \"7\", \"8\", \"9\", \"10\", \"look at\", \"help\", \"increase\", \"decrease\", \"line weight\",  \"line thickness\",  \"weight\",  \"thickness\", \"ready\", \"done\"]}]}, \"audio\": {\"content\":\"";
const char SUFFIX[] = "\"}}"; //suffix to POST request
const int AUDIO_IN = A0; //pin where microphone is connected
const char API_KEY[] = "AIzaSyD6ETx_Ammh1jgbfxG_wggm8eGa08yzQzQ"; //don't change this

char USER[] = "violetta";
char PROJECT[] = "test";

const uint8_t PIN_1 = 16; //button 1
const uint8_t PIN_2 = 5; //button 1

const uint8_t LOOP_PERIOD = 10; //milliseconds
uint32_t primary_timer = 0;

/* Global variables*/
uint8_t button_state; //used for containing button state and detecting edges
int old_button_state; //used for detecting button edges
uint32_t time_since_sample;      // used for microsecond timing

char speech_data[ENC_LEN + 200] = {0}; //global used for collecting speech data
//const char* NETWORK     = "MIT GUEST";     // your network SSID (name of wifi network) 6s08
//const char* PASSWORD = ""; // your network password iesc6s08
const char* NETWORK     = "6s08";     // your network SSID (name of wifi network) 6s08
const char* PASSWORD = "iesc6s08"; // your network password iesc6s08
const char*  SERVER = "speech.google.com";  // Server URL

uint8_t old_val;
uint32_t timer;

MPU9255 imu; //imu object called, appropriately, imu

float x_pos, y_pos, z_pos;
float x_data[10], y_data[10], z_data[10];
int X_LOCK, Y_LOCK, Z_LOCK;
char positions[1000];

int SPEED = 3;

char MOVEMENT[100] = "CURVY";
char DRAWING[100] = "DRAW";

int data_state = 0;
int http_state = 0;
int count = 0;

WiFiClientSecure client; //global WiFiClient Secure object
WiFiClient clientx;

int beginning_post_time = millis();
int ending_post_time = millis();

/**
 * The button class expands upon the typical button
 * usage and allows us to use the button for special inputs
 * like long presses and double presses
 */
class Button {
  public:
  uint32_t t_of_state_2;
  uint32_t t_of_button_change;    
  uint32_t debounce_time;
  uint32_t long_press_time;
  uint8_t pin;
  uint8_t flag;
  bool button_pressed;
  bool is_double;
  bool is_triple;
  uint8_t state; // This is public for the sake of convenience
  Button(int p) {
  flag = 0;  
    state = 0;
    pin = p;
    t_of_state_2 = millis(); //init
    t_of_button_change = millis(); //init
    debounce_time = 10;
    long_press_time = 500;
    button_pressed = 0;
    is_double = false;
    is_triple = false;
  }
  void read() {
    uint8_t button_state = digitalRead(pin);  
    button_pressed = !button_state;
  }
  int update() {
    read();
    flag = 0;
    if (state==0) { // Unpressed, rest state
      if (millis() - t_of_button_change >= 20000) {
        digitalWrite(14,0);
      }
      if (button_pressed) {
        digitalWrite(14,1);
        if (millis() - t_of_button_change <= 100) {
          is_double = true;
        } else {
          is_double = false;
        }
        state = 1;
        t_of_button_change = millis();
      }
    } else if (state==1) { //Tentative pressed
      if (!button_pressed) {
        state = 0;
        t_of_button_change = millis();
      } else if (millis()-t_of_button_change >= debounce_time) {
        state = 2;
        t_of_state_2 = millis();
      }
    } else if (state==2) { // Short press
      if (!button_pressed) {
        state = 4;
        t_of_button_change = millis();
      } else if (millis()-t_of_state_2 >= long_press_time) {
        state = 3;
      }
    } else if (state==3) { //Long press
      if (!button_pressed) {
        state = 4;
        t_of_button_change = millis();
      } else {
        flag = 3;
      }
    } else if (state==4) { //Tentative unpressed
      if (button_pressed && millis()-t_of_state_2 < long_press_time) {
        state = 2; // Unpress was temporary, return to short press
        t_of_button_change = millis();
      } else if (button_pressed && millis()-t_of_state_2 >= long_press_time) {
        state = 3; // Unpress was temporary, return to long press
        t_of_button_change = millis();
      } else if (millis()-t_of_button_change >= debounce_time) { // A full button push is complete
        state = 0;
        if (millis()-t_of_state_2 < long_press_time) { // It is a short press
          flag = 1;
          if (is_double) flag = 4;
        } else {  // It is a long press
          flag = 2;
        }
      }
    }
    return flag;
  }
};

/**
 * The selector class is used to create a selector among choices
 * using the imu to navigate among different choices
 */
class Selector {
    public:
    char alphabet[50] = " ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    char message[400] = {0}; //contains previous query response
    char query_string[50] = {0};
    int char_index;
    int state;
    unsigned long scrolling_timer;
    const int scrolling_threshold = 150;
    const float angle_threshold = 0.3;

    Selector() {
      state = 0;
      memset(message, 0, sizeof(message));
      strcat(message, "Long Press to Start!");
      char_index = 0;
      scrolling_timer = millis();
    }
    void update(float angle, int button, char* output) {
      if (button == 1) {
        strncat(query_string, alphabet + char_index, 1);
        char_index = 0;
        memset(output, 0, sizeof(output));
        strcat(output, query_string);
        strncat(output, alphabet, 1);
        return;
      } else {
        if (angle < -1 * angle_threshold && millis() - scrolling_timer > scrolling_threshold) {
          if (char_index == 0) char_index = strlen(alphabet);
          char_index--;
          scrolling_timer = millis();
        } else if (angle > angle_threshold && millis() - scrolling_timer > scrolling_threshold) {
          char_index++;
          char_index %= strlen(alphabet);
          scrolling_timer = millis();
        }
        memset(output, 0, sizeof(output));
        strcat(output, query_string);
        strncat(output, alphabet + char_index, 1);
        return; 
      }
    }
   int update_selector(int selector, float y, int range) {
      if (y < -.3 && millis() - scrolling_timer > 2*scrolling_threshold) {
        selector = (selector + 1)%range;
        scrolling_timer = millis();
      }
      if (y > .3 && millis() - scrolling_timer > 2*scrolling_threshold) {
        selector = selector - 1;
        if (selector == -1) selector = range-1;
        scrolling_timer = millis();
      }
      return selector;
   }
};

Selector wg; //selector object
Button button(PIN_2); //button object!

bool WIFI = false;
bool WIFI_NOT_DRAWN = true;
bool BATTERY = true;
bool GPS = false;
bool GPS_NOT_DRAWN = true;
int STATUS_COLOR = TFT_BLUE;
int TEXT_COLOR = TFT_PINK;
int BACKGROUND_COLOR = TFT_BLACK;

void setup() {
  Serial.begin(115200);               // Set up serial port
  tft.init();  //init screen

  tft.setRotation(2); //adjust rotation
  tft.setTextSize(1); //default font size
  tft.fillScreen(BACKGROUND_COLOR); //fill background
  tft.setTextColor(TEXT_COLOR, BACKGROUND_COLOR); //set color of font to green foreground, black background
  
  Serial.begin(115200); //begin serial comms
  delay(100); //wait a bit (100 ms)
  pinMode(PIN_1, INPUT_PULLUP);

  WiFi.begin(NETWORK, PASSWORD); //attempt to connect to wifi
  uint8_t count = 0; //count used for Wifi check times
  Serial.print("Attempting to connect to ");
  Serial.println(NETWORK);
  while (WiFi.status() != WL_CONNECTED && count < 12) {
    delay(500);
    Serial.print(".");
    count++;
  }
  delay(2000);
  if (WiFi.isConnected()) { //if we connected then print our IP, Mac, and SSID we're on
    Serial.println("CONNECTED!");
    WIFI = true;
    Serial.println(WiFi.localIP().toString() + " (" + WiFi.macAddress() + ") (" + WiFi.SSID() + ")");
    delay(500);
  } else { //if we failed to connect just Try again.
    Serial.println("Failed to Connect :/  Going to restart");
    Serial.println(WiFi.status());
    ESP.restart(); // restart the ESP (proper way)
  }
  if (imu.setupIMU(1)){
    imu.initMPU9255();
    Serial.println("IMU Connected!");
  }else{
    Serial.println("IMU Not Connected :/");
    Serial.println("Restarting");
    ESP.restart(); // restart the ESP (proper way)
  }
  timer = millis(); //SPECIFIC 
  old_val = digitalRead(PIN_1); //SPECIFIC 
  analogSetAttenuation(ADC_6db); //set to 6dB attenuation for 3.3V full scale reading.
  pinMode(14, OUTPUT);
  digitalWrite(14,1);
}

void loop() {
  //This main loop function acts as a passway between
  //different state machines depending on what state the
  //system is currently in
  if (strcmp(system_state, "UI") == 0) {
    do_ui();
  } 
  if (strcmp(system_state, "DRAW") == 0
      || strcmp(system_state, "MOVE") == 0) {
    do_drawing();
  }
  if (strcmp(system_state, "MENU") == 0) {
    do_menu();
  }
  while (millis()-primary_timer<LOOP_PERIOD); //wait for primary timer to increment
  primary_timer =millis();
}
