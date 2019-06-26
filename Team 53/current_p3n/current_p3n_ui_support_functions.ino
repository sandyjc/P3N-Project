//This code is used for the p3n in terms of UI and drawing


int sign(int value) {
  if (value > 0) return 1;
  if (value < 0) return -1;
  return 0;
}

void update_x() {
  for (int i = 9; i > 0; i--) {
    x_data[i] = x_data[i-1];
  }
}

void update_y() {
  for (int i = 9; i > 0; i--) {
    y_data[i] = y_data[i-1];
  }
}

void update_z() {
  for (int i = 9; i > 0; i--) {
    z_data[i] = z_data[i-1];
  }
}

float avg_x() {
  float sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += x_data[i];
  }
  return sum/10.0;
}

float avg_y() {
  float sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += y_data[i];
  }
  return sum/10.0;
}

float avg_z() {
  float sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += z_data[i];
  }
  return sum/10.0;
}

void initialize_user() {
    memset(thing, 0, 1000);
    memset(request, 0, 1500);
    sprintf(thing,"user=%s&owner=%s&command=\"initialize\"&type=command",USERNAME,OWNER);
    memset(positions, 0, 1000);
    sprintf(request,"POST /sandbox/sc/ykinsew/project/608_server.py HTTP/1.1\r\n");
    sprintf(request+strlen(request),"Host: %s\r\n",host);
    strcat(request,"Content-Type: application/x-www-form-urlencoded\r\n");
    sprintf(request+strlen(request),"Content-Length: %d\r\n\r\n",strlen(thing));
    strcat(request,thing);
    do_http_request(host,request,response,OUT_BUFFER_SIZE, RESPONSE_TIMEOUT,true);
}

void do_ui() {
  float x, y;
  imu.readAccelData(imu.accelCount);
  x = imu.accelCount[0] * imu.aRes;
  y = imu.accelCount[1] * imu.aRes;
  int bv = button.update(); //get button value
  if (bv != 0) Serial.println(bv);
  if (bv == 2) {
    if (state == 0) {
      state = 1;
    } else if (state == 1) {
      if (writing_state == 0) {
      strcat(USERNAME, old_input);
      USERNAME[strlen(USERNAME)-1] = '\0';
      state = 2;
      } else {
        strcat(OWNER, old_input+1);
        OWNER[strlen(OWNER)-1] = '\0';
        state = 3;
        tft.fillScreen(BACKGROUND_COLOR);
        tft.setCursor(0,0,1);
        tft.println("Go to bit.ly/thep3n");
        tft.println("enter your friend's");
        tft.print("username which is ");
        tft.println(USERNAME);
        tft.println("and press drawing");
        tft.println("");
        tft.println("Long press when you're ready...");
        initialize_user();
      }
    } else if (state == 3) {
      state = 4;
      memset(system_state, 0, 100);
      strcat(system_state,"DRAW");
      tft.fillScreen(BACKGROUND_COLOR);
      tft.setCursor(0,0,1);
      tft.print("Username: ");
      tft.println(USERNAME);
      tft.print("State: ");
      tft.println(DRAWING);
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
  }
  if (bv == 1) {
    if (state == 2) {
      tft.fillScreen(BACKGROUND_COLOR);
      tft.setCursor(0,0,1);
      switch (selector) {
        case 0:
          strcat(OWNER, USERNAME);
          tft.println("Go to bit.ly/thep3n");
          tft.println("enter your username");
          tft.print("which is ");
          tft.println(USERNAME);
          tft.println("and press drawing");
          tft.println("");
          tft.println("Long press when");
          tft.println("you're ready...");
          state = 3;
          initialize_user();
          break;
        case 1:
          strcat(OWNER, "tutorial");
          tft.println("Go to bit.ly/thep3n");
          tft.println("enter your username");
          tft.print("which is ");
          tft.println(USERNAME);
          tft.println("and press tutorial");
          tft.println("");
          tft.println("Long press when");
          tft.println("you're ready...");
          state = 3;
          initialize_user();
          break;
        case 2:
          state = 1;
          writing_state = 1;
          memset(input, 0, 100);
          memset(old_input, 0, 100);
          memset(wg.query_string,0,sizeof(wg.query_string));
          break;
      }
    }
  }
  if (state == 0) {
    tft.setCursor(0, 0, 1);
    tft.println("Welcome to the P3N!");
    tft.println("Long press to start");
  }
  if (state == 1) {
    wg.update(-y, bv, input);//input: angle and button, output String to display on this timestep
    if (strcmp(input, old_input) != 0) {//only draw if changed!
      tft.fillScreen(BACKGROUND_COLOR);
      tft.setCursor(0, 0, 1);
      if (writing_state == 0) {
        tft.println("Please enter a");
        tft.println("username:");
        tft.println(input);
      }
      if (writing_state == 1) {
        tft.println("Please enter the");
        tft.println("owner's username:");
        tft.println(input+1);
      }
      tft.println("");
      tft.println("Long press when");
      tft.println("you're done...");
    }
    memset(old_input, 0, sizeof(old_input));
    strcat(old_input, input);
  }
  if (state == 2) {
    if (old_selector != selector) {
      tft.fillScreen(BACKGROUND_COLOR);
      tft.setCursor(0,0,1);
      tft.println("How would you like to");
      tft.print("draw today, ");
      tft.print(USERNAME);
      tft.println("?");
      tft.println("");
      if (selector == 0) tft.println("> 1. Your own drawing");
      else tft.println("  1. Your own drawing");
      if (selector == 1) tft.println("> 2. Tutorial");
      else tft.println("  2. Tutorial");
      if (selector == 2) tft.println("> 3. Join a friend");
      else tft.println("  3. Join a friend");
    }
    old_selector = selector;
    selector = wg.update_selector(selector, y, 3);
  }
}

void do_menu() {
  int bv = button.update();
  float x, y;
  imu.readAccelData(imu.accelCount);
  x = imu.accelCount[0] * imu.aRes;
  y = imu.accelCount[1] * imu.aRes;
  if (selector != old_selector) {
    tft.fillScreen(BACKGROUND_COLOR);
    tft.setCursor(0,0,1);
    tft.print("Username: ");
    tft.println(USERNAME);
    tft.print("State: ");
    tft.println(DRAWING);
    tft.print("Movement: ");
    tft.println(MOVEMENT);
    tft.print("Speed: ");
    tft.println(SPEED);
    tft.println("");
    tft.println("");
    tft.println("");
    if (selector == 0) tft.println("> draw");
    else tft.println("  draw");
    if (selector == 1) tft.println("> move");
    else tft.println("  move"); 
    if (selector == 2) tft.println("> straight lines");
    else tft.println("  straight lines");
    if (selector == 3) tft.println("> curvy lines");
    else tft.println("  curvy lines");
    if (selector == 4) tft.println("> increase speed");
    else tft.println("  increase speed");
    if (selector == 5) tft.println("> decrease speed");
    else tft.println("  decrease speed");
    if (selector == 6) tft.println("> change username");
    else tft.println("  change username");
    if (selector == 7) tft.println("> change drawing mode");
    else tft.println("  change drawing mode");
    tft.println("");
    tft.println("long press to go back");
  }
  if (bv == 2) {
    memset(system_state, 0, 100);
    strcat(system_state, DRAWING);
    memset(old_system_state, 0, 100);
    strcat(old_system_state, "MENU");
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
  if (bv == 1) {
    switch (selector) {
      case 0: 
        memset(DRAWING, 0, 100);
        strcat(DRAWING, "DRAW");
        break;
      case 1:
        memset(DRAWING, 0, 100);
        strcat(DRAWING, "MOVE");
        break;
      case 2:
        memset(MOVEMENT, 0, 100);
        strcat(MOVEMENT, "STRAIGHT");
        break;
      case 3:
        memset(MOVEMENT, 0, 100);
        strcat(MOVEMENT, "CURVY");
        break;
      case 4:
        SPEED += 1;
        if (SPEED > 10) SPEED = 10; 
        tft.println("> increase speed");
        tft.println("  decrease speed"); 
        break;
      case 5:
        SPEED -= 1;
        if (SPEED < 1) SPEED = 1;
        tft.println("  increase speed");
        tft.println("> decrease speed");
        break;
      case 6:
        memset(old_system_state, 0, 100);
        strcat(old_system_state, system_state); 
        memset(system_state, 0, 100);
        strcat(system_state, "UI");
        memset(USERNAME, 0, 100);
        memset(OWNER, 0, 100);
        memset(old_input, 0, sizeof(old_input));
        memset(input, 0, sizeof(input));
        memset(wg.query_string,0,sizeof(wg.query_string));
        state = 1;
        selector = 0;
        old_selector = -1;
        break;
      case 7:
        memset(old_system_state, 0, 100);
        strcat(old_system_state, system_state); 
        memset(system_state, 0, 100);
        strcat(system_state, "UI");
        memset(OWNER, 0, 100);
        memset(old_input, 0, sizeof(old_input));
        memset(input, 0, sizeof(input));
        memset(wg.query_string,0,sizeof(wg.query_string));
        selector = 0;
        old_selector = -1;
        state = 2;
        break;
    }
    tft.fillScreen(BACKGROUND_COLOR);
    tft.setCursor(0,0,1);
    tft.print("Username: ");
    tft.println(USERNAME);
    tft.print("State: ");
    tft.println(DRAWING);
    tft.print("Movement: ");
    tft.println(MOVEMENT);
    tft.print("Speed: ");
    tft.println(SPEED);
    tft.println("");
    tft.println("");
    tft.println("");
    if (selector == 0) tft.println("> draw");
    else tft.println("  draw");
    if (selector == 1) tft.println("> move");
    else tft.println("  move"); 
    if (selector == 2) tft.println("> straight lines");
    else tft.println("  straight lines");
    if (selector == 3) tft.println("> curvy lines");
    else tft.println("  curvy lines");
    if (selector == 4) tft.println("> increase speed");
    else tft.println("  increase speed");
    if (selector == 5) tft.println("> decrease speed");
    else tft.println("  decrease speed");
    if (selector == 6) tft.println("> change username");
    else tft.println("  change username");
    if (selector == 7) tft.println("> change drawing mode");
    else tft.println("  change drawing mode");
    tft.println("");
    tft.println("long press to go back");
  }
  old_selector = selector;
  selector = wg.update_selector(selector, y, 8);
}

float make_straight(float num) {
  float ret = 0.0;
  if (num < -.5) ret = -1.0;
  if (num >= -.5 && num < .5) ret = 0.0;
  if (num >= .5) ret = 1.0;
  return ret;
}

void do_drawing() {
  button_state = digitalRead(PIN_1);
  if (!button_state && button_state != old_button_state) {
      speech_function();
  }
  old_button_state = button_state;
  imu.readAccelData(imu.accelCount);
  int bv = button.update();
  //int button1 = digitalRead(PIN_2);
  if (bv == 4) {
    selector = 0;
    old_selector = 0;
    tft.fillScreen(BACKGROUND_COLOR);
    tft.setCursor(0,0,1);
    tft.print("Username: ");
    tft.println(USERNAME);
    tft.print("State: ");
    tft.println(DRAWING);
    tft.print("Movement: ");
    tft.println(MOVEMENT);
    tft.print("Speed: ");
    tft.println(SPEED);
    tft.println("");
    tft.println("");
    tft.println("");
    tft.println("> draw");
    tft.println("  move");
    tft.println("  straight lines");
    tft.println("  curvy lines");
    tft.println("  increase speed");
    tft.println("  decrease speed");
    tft.println("  change username");
    tft.println("  change drawing mode");
    tft.println("");
    tft.println("long press to go back");
    memset(old_system_state, 0, 100);
    strcat(old_system_state, system_state); 
    memset(system_state, 0, 100);
    strcat(system_state, "MENU");
  }
  if (bv == 2) {
    sprintf(positions+strlen(positions),"%s ","stop");
  }
  if (bv == 3) {
    bool straight = false;
    if (strcmp(MOVEMENT,"STRAIGHT") == 0) straight = true;
    
    update_x();
    x_data[0] = imu.accelCount[2]*imu.aRes*9.81;
    x_data[0] = (abs(x_data[0]) < 1) ? 0 : (abs(x_data[0]) > 9.81) ? sign(x_data[0])*9.81 : x_data[0];
    float x_avg = avg_x();
    float x_angle = 90*x_avg/9.81;
    if (straight) x_avg = make_straight(x_avg/9.81)*9.81;
    x_pos += SPEED*sign(x_avg)*x_avg*x_avg*LOOP_PERIOD/1000*LOOP_PERIOD/1000*10;

    update_y();
    y_data[0] = -1*imu.accelCount[0]*imu.aRes*9.81;
    y_data[0] = (abs(y_data[0]) < 1) ? 0 : (abs(y_data[0]) > 9.81) ? sign(y_data[0])*9.81 : y_data[0];
    float y_avg = avg_y();
    float y_angle = 90*y_avg/9.81;
    if (straight) y_avg = make_straight(y_avg/9.81)*9.81;
    y_pos += SPEED*sign(y_avg)*y_avg*y_avg*LOOP_PERIOD/1000*LOOP_PERIOD/1000*10;

    update_z();
    z_data[0] = -1*imu.accelCount[1]*imu.aRes*9.81;
    z_data[0] = (abs(z_data[0]) < 1) ? 0 : (abs(z_data[0]) > 9.81) ? sign(z_data[0])*9.81 : z_data[0];
    float z_avg = avg_z();
    float z_angle = 90*z_avg/9.81;
    if (straight) z_avg = make_straight(z_avg/9.81)*9.81;
    z_pos += SPEED*sign(z_avg)*z_avg*z_avg*LOOP_PERIOD/1000*LOOP_PERIOD/1000*10;
    
    if (data_state == 0) {
      //Serial printing:
      char output[40];
      x_pos = (abs(x_pos) >= 100.) ? sign(x_pos)*100. : x_pos; 
      y_pos = (abs(y_pos) >= 100.) ? sign(y_pos)*100. : y_pos;
      z_pos = (abs(z_pos) >= 100.) ? sign(z_pos)*100. : z_pos; 
      sprintf(output,"%4.2f,%4.2f,%4.2f",x_pos,y_pos,z_pos); //render numbers with %4.2 float formatting
      //Serial.println(output); //print to serial for plotting
      sprintf(positions+strlen(positions),"%s ",output);
      data_state = 1;
    } else if (data_state == 1) {
      data_state = 2;
    } else if (data_state == 2) {
      data_state = 0;
    }
    
    if (http_state == 0) {
      memset(thing, 0, 1000);
      memset(request, 0, 1500);
      sprintf(thing,"user=%s&owner=%s&points=%s&state=%s&type=position",USERNAME,OWNER,positions,system_state);
      Serial.println(thing);
      memset(positions, 0, 1000);
      sprintf(request,"POST /sandbox/sc/ykinsew/project/608_server.py HTTP/1.1\r\n");
      sprintf(request+strlen(request),"Host: %s\r\n",host);
      strcat(request,"Content-Type: application/x-www-form-urlencoded\r\n");
      sprintf(request+strlen(request),"Content-Length: %d\r\n\r\n",strlen(thing));
      strcat(request,thing);
      beginning_post_time = millis();
    }
    do_nb_http_request(host,request,response,OUT_BUFFER_SIZE, RESPONSE_TIMEOUT,true);
  }
}
