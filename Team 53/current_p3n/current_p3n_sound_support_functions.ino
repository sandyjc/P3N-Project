//This is the code that is utilized for the sound portion of the p3n, such as sound
//processing, sending the data up to google, and then sending the transcription to our api
//to implement the command with the drawing


//function used to record audio at sample rate for a fixed nmber of samples
void record_audio() {
  int sample_num = 0;    // counter for samples
  int enc_index = strlen(PREFIX) - 1;  // index counter for encoded samples
  float time_between_samples = 1000000 / SAMPLE_FREQ;
  int value = 0;
  char raw_samples[3];   // 8-bit raw sample data array
  memset(speech_data, 0, sizeof(speech_data));
  sprintf(speech_data, "%s", PREFIX);
  char holder[5] = {0};
  //Serial.println("starting");
  uint32_t text_index = enc_index;
  uint32_t start = millis();
  time_since_sample = micros();
  
  while (!button_state && sample_num < NUM_SAMPLES) { //read in NUM_SAMPLES worth of audio data
    button_state = digitalRead(PIN_1);
    value = analogRead(AUDIO_IN);  //make measurement
    raw_samples[sample_num % 3] = mulaw_encode(value - 1241); //remove 1.0V offset (from 12 bit reading)
    sample_num++;
    if (sample_num % 3 == 0) {
      base64_encode(holder, raw_samples, 3);
      strncat(speech_data + text_index, holder, 4);
      text_index += 4;
    }
    // wait till next time to read
    while (micros() - time_since_sample <= time_between_samples); //wait...
    time_since_sample = micros();
  }
  //Serial.println(millis() - start);
  sprintf(speech_data + strlen(speech_data), "%s", SUFFIX);
  //Serial.println("out");
}


int8_t mulaw_encode(int16_t sample){
   const uint16_t MULAW_MAX = 0x1FFF;
   const uint16_t MULAW_BIAS = 33;
   uint16_t mask = 0x1000;
   uint8_t sign = 0;
   uint8_t position = 12;
   uint8_t lsb = 0;
   if (sample < 0)
   {
      sample = -sample;
      sign = 0x80;
   }
   sample += MULAW_BIAS;
   if (sample > MULAW_MAX)
   {
      sample = MULAW_MAX;
   }
   for (; ((sample & mask) != mask && position >= 5); mask >>= 1, position--)
        ;
   lsb = (sample >> (position - 4)) & 0x0f;
   return (~(sign | ((position - 5) << 4) | lsb));
}


const char PROGMEM b64_alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";
/* 'Private' declarations */
inline void a3_to_a4(unsigned char * a4, unsigned char * a3);
inline void a4_to_a3(unsigned char * a3, unsigned char * a4);
inline unsigned char b64_lookup(char c);

int base64_encode(char *output, char *input, int inputLen) {
  int i = 0, j = 0;
  int encLen = 0;
  unsigned char a3[3];
  unsigned char a4[4];

  while(inputLen--) {
    a3[i++] = *(input++);
    if(i == 3) {
      a3_to_a4(a4, a3);

      for(i = 0; i < 4; i++) {
        output[encLen++] = pgm_read_byte(&b64_alphabet[a4[i]]);
      }

      i = 0;
    }
  }

  if(i) {
    for(j = i; j < 3; j++) {
      a3[j] = '\0';
    }

    a3_to_a4(a4, a3);

    for(j = 0; j < i + 1; j++) {
      output[encLen++] = pgm_read_byte(&b64_alphabet[a4[j]]);
    }

    while((i++ < 3)) {
      output[encLen++] = '=';
    }
  }
//  output[encLen] = '\0';
  return encLen;
}

int base64_decode(char * output, char * input, int inputLen) {
  int i = 0, j = 0;
  int decLen = 0;
  unsigned char a3[3];
  unsigned char a4[4];


  while (inputLen--) {
    if(*input == '=') {
      break;
    }

    a4[i++] = *(input++);
    if (i == 4) {
      for (i = 0; i <4; i++) {
        a4[i] = b64_lookup(a4[i]);
      }

      a4_to_a3(a3,a4);

      for (i = 0; i < 3; i++) {
        output[decLen++] = a3[i];
      }
      i = 0;
    }
  }

  if (i) {
    for (j = i; j < 4; j++) {
      a4[j] = '\0';
    }

    for (j = 0; j <4; j++) {
      a4[j] = b64_lookup(a4[j]);
    }

    a4_to_a3(a3,a4);

    for (j = 0; j < i - 1; j++) {
      output[decLen++] = a3[j];
    }
  }
  output[decLen] = '\0';
  return decLen;
}

int base64_enc_len(int plainLen) {
  int n = plainLen;
  return (n + 2 - ((n + 2) % 3)) / 3 * 4;
}

int base64_dec_len(char * input, int inputLen) {
  int i = 0;
  int numEq = 0;
  for(i = inputLen - 1; input[i] == '='; i--) {
    numEq++;
  }

  return ((6 * inputLen) / 8) - numEq;
}

inline void a3_to_a4(unsigned char * a4, unsigned char * a3) {
  a4[0] = (a3[0] & 0xfc) >> 2;
  a4[1] = ((a3[0] & 0x03) << 4) + ((a3[1] & 0xf0) >> 4);
  a4[2] = ((a3[1] & 0x0f) << 2) + ((a3[2] & 0xc0) >> 6);
  a4[3] = (a3[2] & 0x3f);
}

inline void a4_to_a3(unsigned char * a3, unsigned char * a4) {
  a3[0] = (a4[0] << 2) + ((a4[1] & 0x30) >> 4);
  a3[1] = ((a4[1] & 0xf) << 4) + ((a4[2] & 0x3c) >> 2);
  a3[2] = ((a4[2] & 0x3) << 6) + a4[3];
}

inline unsigned char b64_lookup(char c) {
  if(c >='A' && c <='Z') return c - 'A';
  if(c >='a' && c <='z') return c - 71;
  if(c >='0' && c <='9') return c + 4;
  if(c == '+') return 62;
  if(c == '/') return 63;
  return -1;
}


void speech_function(){
  Serial.println("listening...");
  tft.fillScreen(BACKGROUND_COLOR);
  tft.setCursor(0,0,1);
  tft.println("listening...");
  record_audio();
  Serial.println("sending...");
  tft.fillScreen(BACKGROUND_COLOR);
  tft.setCursor(0,0,1);
  tft.println("transcribing audio...");
  timer = millis();
  delay(300);
  bool conn = false;
  for (int i = 0; i < 10; i++) {
    int val = (int)client.connect(SERVER, 443);
    if (val != 0) {
      conn = true;
      break;
    }
    delay(300);
  }
  if (!conn) {
    Serial.println("Connection failed!");
    return;
  } 
  else {
    int len = strlen(speech_data); //speech data is the result of what we say 
    // POST to Google API to get value back 
    client.print("POST /v1/speech:recognize?key="); client.print(API_KEY); client.print(" HTTP/1.1\r\n");
    client.print("Host: speech.googleapis.com\r\n");
    client.print("Content-Type: application/json\r\n");
    client.print("cache-control: no-cache\r\n");
    client.print("Content-Length: "); client.print(len);
    client.print("\r\n\r\n");
    int ind = 0;
    int jump_size = 1000;
    char temp_holder[jump_size + 10] = {0};
    Serial.println("sending data");
    while (ind < len) {
      delay(80);
      strncat(temp_holder, speech_data + ind, jump_size);
      client.print(temp_holder);
      ind += jump_size;
      memset(temp_holder, 0, sizeof(temp_holder));
    }
    client.print("\r\n");
    Serial.println("Through send...");
    unsigned long count = millis();
    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") { //got header of response
        Serial.println("headers received");
        break;
      }
      if (millis() - count > RESPONSE_TIMEOUT) break;
    }
    count = millis();
    while (!client.available()) {
      delay(100);
      if (millis() - count > RESPONSE_TIMEOUT) break;
    }
    memset(response, 0, sizeof(response));
    while (client.available()) {
      char_append(response, client.read(), OUT_BUFFER_SIZE);
    }
    char* trans_id = strstr(response, "transcript");
    if (trans_id != NULL) {
      char* foll_coll = strstr(trans_id, ":");
      char* starto = foll_coll + 2; //starting index
      char* endo = strstr(starto + 1, "\""); //ending index
      int transcript_len = endo-starto+1;
      char transcript[100] = {0};
      strncat(transcript,starto,transcript_len);
      tft.fillScreen(BACKGROUND_COLOR);
      tft.setCursor(0,0,1);
      tft.println("Received command");
      tft.println(transcript);
      int transcript_timer = millis();
      memset(thing, 0, 1000);
      memset(request, 0, 1500);
      sprintf(thing,"user=%s&owner=%s&command=%s&type=command",USERNAME,OWNER,transcript);
      memset(positions, 0, 1000);
      sprintf(request,"POST /sandbox/sc/ykinsew/project/608_server.py HTTP/1.1\r\n");
      sprintf(request+strlen(request),"Host: %s\r\n",host);
      strcat(request,"Content-Type: application/x-www-form-urlencoded\r\n");
      sprintf(request+strlen(request),"Content-Length: %d\r\n\r\n",strlen(thing));
      strcat(request,thing);
      do_http_request(host,request,response,OUT_BUFFER_SIZE, RESPONSE_TIMEOUT,true);
      while (millis() - transcript_timer < 1000);
      tft.fillScreen(BACKGROUND_COLOR);
      tft.setCursor(0,0,1);
      tft.print("Username: ");
      tft.println(USERNAME);
      tft.print("State: ");
      tft.println(system_state);
      tft.print("Movement: ");
      tft.println(MOVEMENT);
      tft.print("Speed: ");
      tft.println(SPEED);
      tft.println("");
      tft.println("");
      tft.println("");
      tft.println("");
      tft.println("");
      tft.println("");
      tft.println("");
      tft.println("");
      tft.println("");
      tft.println("");
      tft.println("");
      tft.println("");
      tft.println("You can double click");
      tft.println("to access the menu");
    }
    client.stop();
  }
}
