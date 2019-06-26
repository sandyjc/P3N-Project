//This code is used for the p3n in terms of working with GET and POST requests


uint8_t char_append(char* buff, char c, uint16_t buff_size) {
  int len = strlen(buff);
  if (len > buff_size) return false;
  buff[len] = c;
  buff[len + 1] = '\0';
  return true;
}

void do_http_request(char* host, char* request, char* response, uint16_t response_size, uint16_t response_timeout, uint8_t serial){
  WiFiClient client; //instantiate a client object
  if (client.connect(host, 80)) { //try to connect to host on port 80
    client.print(request);
    memset(response, 0, response_size); //Null out (0 is the value of the null terminator '\0') entire buffer
    uint32_t count = millis();
    while (client.connected()) { //while we remain connected read out data coming back
      client.readBytesUntil('\n',response,response_size);
      if (strcmp(response,"\r")==0) { //found a blank line!
        break;
      }
      memset(response, 0, response_size);
      if (millis()-count>response_timeout) break;
    }
    memset(response, 0, response_size);  
    count = millis();
    while (client.available()) { //read out remaining text (body of response)
      char_append(response,client.read(),OUT_BUFFER_SIZE);
    }
    client.stop();
  }else{
    client.stop();
}
}

void format_POST(char* col, char* val){
  //create a post request 
  char body[200];
  sprintf(body,"user=%s&project=%s&%s=%s&post=%s",USER, PROJECT, col, val, "POST");
  int body_len = strlen(body);

  //not sure about the final http part of this post guy
  sprintf(request_buffer,"POST http://608dev.net/sandbox/sc/vjusiega/p3n/speech_commands.py HTTP/1.1\r\n"); 
  strcat(request_buffer,"Host: 608dev.net\r\n");
  strcat(request_buffer,"Content-Type: application/x-www-form-urlencoded\r\n");
  sprintf(request_buffer+strlen(request_buffer),"Content-Length: %d\r\n", body_len); //append string formatted to end of request buffer
  strcat(request_buffer,"\r\n"); //new line from header to body
  strcat(request_buffer,body); //body
  strcat(request_buffer,"\r\n"); //header

  //this final part posts
  do_http_request("608dev.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT,true);
}

void do_nb_http_request(char* host, char* request, char* response, uint16_t response_size, uint16_t response_timeout, uint8_t serial) {
  if (http_state == 0) {
    if (clientx.connect(host,80)) {
      http_state = 1;
    } else {
      if (serial) Serial.println("connection failed :/");
      if (serial) Serial.println("wait 0.5 sec...");
      clientx.stop();
    }
  } else if (http_state == 1) {
    clientx.print(request);
    memset(response, 0, response_size);
    count = 0;
    http_state = 2;
  } else if (http_state == 2) {
    if (clientx.connected()) {
      if (count == 0) count = millis();
      clientx.readBytesUntil('\n', response, response_size);
      if (strcmp(response, "\r") == 0) {
          http_state = 3;
      } else {
        memset(response, 0, response_size);
        if (millis() - .5*count > response_timeout) {
            http_state = 3;
        }
      }
    } else {
      http_state = 3;
    }
  } else if (http_state == 3) {
    memset(response, 0, response_size);
    count = millis();
    http_state = 4;
  } else if (http_state == 4) {
    while (clientx.available()) {
      char_append(response, clientx.read(), OUT_BUFFER_SIZE);
    }
    http_state = 5;
  } else if (http_state == 5) {
    if (serial) {
    }
    clientx.stop();
    ending_post_time = millis();
    http_state = 0;
  }
}
