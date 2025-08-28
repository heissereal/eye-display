#include <Arduino.h>
#include <math.h>
#include <Wire.h>
#include <WireSlave.h>

#include "eye.hpp"

#define TFT_BL 10

const int image_width = 139;
// const int image_height = 120;
const int image_height = 139;

const char path_image_eyeball[] = "/eyeball.jpg";

// const char path_image_iris_right[] = "/iris_right.jpg";
const char path_image_iris_right[] = "/pupil.jpg";
const char path_image_surprised_iris_right[] = "/iris_surprised_right.jpg";
// const char path_image_upperlid_right[] = "/u.jpg";
//const char path_image_upperlid_right[] = "/iris_surprised_right.jpg";
const char path_image_upperlid_right[] = "/eyelid.jpg";
const char path_image_upperchanger_right[] = "/upper_changer.jpg";
const char path_image_lowerchanger_right[] = "/lower_changer.jpg";

const char path_image_heart_right[] = "/heart.jpg";
const char path_image_namida_right[] = "/namida.jpg";
const char path_image_namida_2_right[] = "/namida_2.jpg";
const char path_image_star_right[] = "/star.jpg";
const char path_image_fear_right[] = "/fear.jpg";
const char path_image_sleepy_right[] = "/sleep.jpg";

const char path_image_angry_upperlid_right[] = "/upperlid_leftside_down.jpg";
const char path_image_sad_upperlid_right[] = "/upperlid_rightside_down.jpg";
const char path_image_happy_upperlid_right[] = "/upperlid_happy_right.jpg";

// const char path_image_iris_left[] = "/iris_left.jpg";
// const char path_image_surprised_iris_left[] = "/iris_surprised_left.jpg";
// const char path_image_upperlid_left[] = "/upperlid.jpg";
// const char path_image_angry_upperlid_left[] = "/upperlid_rightside_down.jpg";
// const char path_image_sad_upperlid_left[] = "/upperlid_leftside_down.jpg";
// const char path_image_happy_upperlid_left[] =  "/upperlid_happy_left.jpg";

// eye_status ... 0: 通常, 1: 瞬き, 2: 驚き, 3: 眠い, 4: 怒る, 5: 悲しむ・困る, 6: 嬉しい...
int eye_status = 0;
// int blink_level = 0; int max_blink_level = 6;
int surprised_level = 0; int max_surprised_level = 16;
// int sleepy_level = 0; int max_sleepy_level = 10;
int angry_level = 0; int max_angry_level = 20;
int sad_level = 0; int max_sad_level = 20;
int happy_level = 0; int max_happy_level = 20;
int heart_level = 0; int max_heart_level = 16;
int star_level = 0; int max_star_level = 16;
int sleepy_level = 0; int max_sleepy_level = 15;

bool effect_locked = false;
int effect_type = -1; // 0: heart, 1: star, 2: namida, 3: fear, 4: sleepy
int effect_use = -1;
static Eye eye;

float look_x = 0.0; // 目の動き
float look_y = 0.0; // 目の動き
float a = 0.0f;
float d = 0.0f; // -1.0〜1.0
float v = 0.0f;   // -1.0〜1.0
// float changer_v = 0.0f; // -1.0〜1.0

constexpr int SDA_PIN = 8; //design pin number
constexpr int SCL_PIN = 9;
// constexpr int I2C_SLAVE_ADDR = 0x43; //I2C slave address left
constexpr int I2C_SLAVE_ADDR = 0x42; //I2C slave address right

void receiveEvent(int howMany); //callback

void I2CTask(void *parameter) {
  bool success = WireSlave.begin(SDA_PIN, SCL_PIN, I2C_SLAVE_ADDR);

  Serial.println("I2C slave start");
  if (!success) {
    // lcd.println("I2C slave init failed");
    Serial.println("I2C slave init failed");
    while (1) delay(100);
  }
  WireSlave.onReceive(receiveEvent);
  while (true) {
    WireSlave.update();
    delay(1);  // let I2C and other ESP32 peripherals interrupts work
  }
}



void setup()
{
  Serial.begin(115200);

  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  SPIFFS.begin(); //manage image resources

  delay(5000);

  // String mode = "right";
  String mode = "left";
  if (mode == "right")
  {
    Serial.printf("aaa\n");
    eye.init(path_image_eyeball, path_image_iris_right,  path_image_upperlid_right, path_image_upperchanger_right, path_image_lowerchanger_right, image_width, image_height, 1);
    Serial.printf("hello\n");
  }
  else
  {
    eye.init(path_image_eyeball, path_image_iris_right,  path_image_upperlid_right, path_image_upperchanger_right, path_image_lowerchanger_right, image_width, image_height, 5);
  }

  eye.update_look();
  xTaskCreatePinnedToCore(I2CTask, "I2C Task", 2048, NULL, 24, NULL, 0);
}

static int i = 0;

void loop()
{
  delay(10);
  i++;
  if (i % 50 == 0){
    i = 0;
  }

  // float look_x = 2. * sin(i * 0.1);
  // float look_y = 2. * cos(i * 0.1) - 2.;
  //float look_x = 0.3 * sin(i * 0.1);
  //float look_y = 0.3 * cos(i * 0.1) ;
  Serial.printf("");
  static bool finished_look = true;

  int new_effect_type = -1;
  if (v >= 0.8f && a > 0.4f && a < 0.6f) new_effect_type = 0;
  else if (v >= 0.8f && a >= 0.6f) new_effect_type = 1;
  else if (v >= -0.5f && v <= -0.25f && a > 0.0f && a <= 0.5f && d < 0.0f) new_effect_type = 2;
  else if (v >= -0.5f && v <= -0.25f && a > 0.5f && d < 0.0f) new_effect_type = 3;
  else if (a <= -0.9f) new_effect_type = 4;

  if (effect_type != new_effect_type) {
    effect_locked = false;
    effect_use = 0;
    effect_type = -1;
  }

  // if (effect_locked && effect_type != new_effect_type) {
  //   effect_locked = false;
  //   effect_use = 0;
  //   effect_type = -1;
  // }

  if (!effect_locked) {
    // アニメーション途中であれば継続
    if (finished_look) {
      eye.reset_update_step();
      finished_look = false;
    }
    if (effect_use == 0){
      eye.ready_for_normal_eye(path_image_iris_right, path_image_upperchanger_right, path_image_lowerchanger_right, path_image_upperlid_right);
      effect_use == -1;
    }
    eye.update_look(look_x, look_y);
    if (eye.isUpdateFinished()) {  // アニメーションが完了した瞬間
      finished_look = true;

      if (new_effect_type != -1) {
        effect_locked = true;
        effect_type = new_effect_type;
        effect_use = 1;
      }
    }
  } else {
    // エフェクトがロックされている場合
    switch (effect_type) {
    case 0:
      if (effect_use == 1) {
        eye.ready_for_heart_eye(path_image_heart_right);
        effect_use = 2;
      } else {
        eye.heart(look_x, look_y, heart_level);
        if (++heart_level >= max_heart_level) heart_level = 0;
      }
      break;
    case 1:
      if (effect_use == 1) {
        eye.ready_for_star_eye(path_image_star_right);
        effect_use = 2;
      } else {
        eye.star(look_x, look_y, star_level);
        if (++star_level >= max_star_level) star_level = 0;
      }
      break;
    case 2:
      if (effect_use == 1) {
        eye.ready_for_namida_eye(path_image_iris_right, path_image_upperlid_right, path_image_lowerchanger_right, path_image_namida_right);
        effect_use = 2;
      } else {
        eye.namida(look_x, look_y);
      }
      break;
    case 3:
      if (effect_use == 1) {
        eye.ready_for_fear_eye(path_image_iris_right, path_image_upperlid_right, path_image_upperchanger_right, path_image_fear_right);
        effect_use = 2;
      } else {
        eye.fear(look_x, look_y);
      }
      break;
    case 4:
      if (effect_use == 1) {
        eye.ready_for_sleepy_eye(path_image_iris_right, path_image_upperlid_right, path_image_upperchanger_right, path_image_sleepy_right);
        effect_use = 2;
      } else {
        eye.sleepy(look_x, look_y, sleepy_level);
        if (++sleepy_level >= max_sleepy_level) sleepy_level = 0;
      }
      break;
    }
  }
  
  // if (eye_status == 0) {
  //   // 通常
  //   // eye.update_look(look_x, look_y);
  //   int new_effect_type = -1;
  //   if (v >= 0.8f && a > 0.4f && a <= 0.6f) {
  //     new_effect_type = 0;
  //   } else if (v >= 0.8f && a > 0.6f) {
  //     new_effect_type = 1;
  //   } else if (v >= -0.5f && v <= -0.25f && a >= 0.0f && a <= 0.5f && d <=0.0f) {
  //     new_effect_type = 2;
  //   } else if (v >= -0.5f && v <= -0.25f && a > 0.5f && d <= 0.0f) {
  //     new_effect_type = 3;
  //   } else if (a <= -0.9f) {
  //     new_effect_type = 4;
  //   }
  //   Serial.printf("use: %d, locked: %d\n", effect_use, effect_locked);
  //   // ロック解除条件（VADが閾値から外れた）
  //   if (effect_locked && effect_type != new_effect_type) {
  //     effect_locked = false;
  //     effect_use = 0;
  //     effect_type = -1;
  //   }

  //   if (!effect_locked) {
  //     if (effect_use == 0){
  //       eye.ready_for_normal_eye(path_image_iris_right, path_image_upperchanger_right, path_image_lowerchanger_right, path_image_upperlid_right);
  //       effect_use == -1;
  //     }
  //     eye.update_look(look_x, look_y);
  //     if (new_effect_type != -1) {
  //       effect_locked = true;
  //       effect_type = new_effect_type;
  //       effect_use = 1;
  //     }
  //   }else {
  //     switch (effect_type) {
  //     case 0:
  //       if (effect_use == 1){
  //         eye.ready_for_heart_eye(path_image_heart_right);
  //         effect_use = 2;
  //       }else{
  //         eye.heart(look_x, look_y, heart_level);
  //         heart_level += 1;
  //         if (heart_level == max_heart_level){
  //           heart_level = 0;
  //         }
  //       }
  //       break;
  //     case 1:
  //       if (effect_use == 1){
  //         eye.ready_for_star_eye(path_image_star_right);
  //         effect_use = 2;
  //       }else{
  //         eye.star(look_x, look_y, star_level);
  //         star_level += 1;
  //         if (star_level == max_star_level){
  //           star_level = 0;
  //         }
  //       }
  //       break;
  //     case 2:
  //       if (effect_use == 1){
  //         eye.ready_for_namida_eye(path_image_iris_right, path_image_upperlid_right, path_image_lowerchanger_right, path_image_namida_right);
  //         effect_use = 2;
  //       }else{
  //         eye.namida(look_x, look_y);
  //       }
  //       break;
  //     case 3:
  //       if (effect_use == 1){
  //         eye.ready_for_fear_eye(path_image_iris_right, path_image_upperlid_right, path_image_upperchanger_right, path_image_fear_right);
  //         effect_use = 2;
  //       }else{
  //         eye.fear(look_x, look_y);
  //       }
  //       break;
  //     case 4:
  //       if (effect_use == 1){
  //         eye.ready_for_sleepy_eye(path_image_iris_right, path_image_upperlid_right, path_image_upperchanger_right, path_image_sleepy_right);
  //         effect_use = 2;
  //       }else{
  //         eye.sleepy(look_x, look_y, sleepy_level);
  //         sleepy_level += 1;
  //         if (sleepy_level == max_sleepy_level){
  //           sleepy_level = 0;
  //         }
  //       }
  //       break;
  //     }}}


  
  // else if (eye_status == 1){
  //   // 瞬き
  //   eye.blink_eye(look_x, look_y, blink_level);
  //   blink_level += 1;
  //   if (blink_level == max_blink_level){
  //     blink_level = 0;
  //     // eye_status = 0;
  //   }
  // }

  // else if (eye_status == 2){
  //   // 驚き
  //   if (surprised_level == 0){
  //     eye.ready_for_surprised_eye(path_image_surprised_iris_right);
  //   }
  //   eye.surprised(look_x, look_y, surprised_level);
  //   surprised_level += 1;
  //   if (surprised_level == max_surprised_level){
  //     surprised_level = 0;
  //     // eye_status = 0;
  //     eye.ready_for_normal_eye(path_image_iris_right, path_image_upperlid_right);
  //   }
  // }

  // else if (eye_status == 3){
  //   // 眠い
  //   eye.sleepy(look_x, look_y, sleepy_level);
  //   sleepy_level += 1;
  //   if (sleepy_level == max_sleepy_level){
  //     sleepy_level = 0;
  //     // eye_status = 0;
  //     eye.ready_for_normal_eye(path_image_iris_right, path_image_upperlid_right);
  //   }
  // }
  // else if (eye_status == 4){
  //   // 怒り
  //   if (angry_level == 0){
  //     // eye.ready_for_heart_eye(path_image_angry_upperlid_right, path_image_heart_right, path_image_namida_right, path_image_star_right);
  //   }
  //   eye.angry(look_x, look_y, angry_level);
  //   angry_level += 1;
  //   if (angry_level == max_angry_level){
  //     angry_level = 0;
  //     eye.ready_for_normal_eye(path_image_iris_right, path_image_upperlid_right);
  //   }
  // }

  // else if (eye_status == 5){
  //   // 悲しむ・困る
  //   if (sad_level == 0){
  //     eye.ready_for_namida_eye(path_image_sad_upperlid_right);
  //   }
  //   eye.sad(look_x, look_y, sad_level);
  //   sad_level += 1;
  //   if (sad_level == max_sad_level){
  //     sad_level = 0;
  //     eye.ready_for_normal_eye(path_image_iris_right, path_image_upperlid_right);
  //   }
  // }

  // else if (eye_status == 6) {
  //   // 喜ぶ
  //   if (happy_level == 0){
  //     eye.ready_for_happy_eye(path_image_happy_upperlid_right);
  //   }
  //   eye.happy(look_x, look_y, happy_level);
  //   happy_level += 1;
  //   if (happy_level == max_happy_level){
  //     happy_level = 0;
  //     eye.ready_for_normal_eye(path_image_iris_right, path_image_upperlid_right);
  //   }
  // }

  // eye.update_upperlid_y(lid_y_a);
  // eye.set_eyelid_offset_x(lid_x_d);
  // eye.set_iris_scale_val(iris_v);
  // eye.set_changer_v(changer_v);
  eye.set_valence_val(v);
  eye.set_arousal_val(a);
  eye.set_dominance_val(d);
  Serial.printf("look_x: %f, look_y: %f\n", look_x, look_y);
  eye.draw_updated_image();
}

void receiveEvent(int howMany) {
  // lastReceiveTime = millis();  // Update the last received time
  String str;
  while (0 < WireSlave.available()) {
    char c = WireSlave.read();  // receive byte as a character
    str += c;
  }
  // if (str.length() == 1 && isDigit(str[0])) {
  //   int val = str.toInt();
  //   if (val >= 0 && val <= 6) {
  //     look_x = 0.0;
  //     eye_status = val;// 0: 通常, 1: 瞬き, 2: 驚き, 3: 眠い, 4: 怒る, 5: 悲しむ・困る, 6: 嬉しい
  //   }
  // }else{
  //   look_x = str.toFloat();
  // }
  if (str.length() > 0) {
    char type = str[0];
    String value_str = str.substring(1);
    float val = value_str.toFloat();
    if (type == 'S')
      {
      int int_val = (int)val;
      if (int_val >= 0 && int_val <= 6)
        {
        eye_status = int_val;
        look_x = 0.0;
        }
      }
    else if (type == 'D')
      {
        d = constrain(val, -1.0f, 1.0f);
      }
    else if (type == 'X')
      {
        look_x = val;
      }
    else if (type == 'A')
      {
        a = constrain(val, -1.0f, 1.0f);
      }
    else if (type == 'V')
      {
        v = constrain(val, -1.0f, 1.0f);
      }
    // else if (type == 'C')
    //   {
    //     changer_v = constrain(val, -1.0f, 1.0f);
    //   }
  }
}
