#include <math.h>
#include <functional>
// #include <vector>
#include <Arduino.h>
#include <SPIFFS.h>

#if defined(STAMPS3)
#include <lgfx_round_lcd_stamp_s3.hpp>
#elif defined(STAMPC3)
#include <lgfx_round_lcd_stamp_c3.hpp>
#endif

class Eye
{
private:
  LGFX_ROUND_LCD lcd;

  // Spriteを設定
  LGFX_Sprite sprite_eye;
  LGFX_Sprite sprite_eyeball;
  LGFX_Sprite sprite_iris;
  LGFX_Sprite sprite_pupil;
  LGFX_Sprite sprite_reflex;
  LGFX_Sprite sprite_upperlid;
  LGFX_Sprite sprite_upperchanger;
  LGFX_Sprite sprite_lowerchanger;

  float zoom_ratio;

  int image_width;
  int image_height;

  // std::vector<int> upperlid_y_arr;
  // int blink_level = 0;
  // int max_blink_level = 5;

  float current_dx = 0.0f;
  float current_dy = 0.0f;
  float current_upperlid_val = 0.0f; // a
  float current_eyelid_offset_x = 0.0f; //d
  float current_iris_scale_val = 0.0f; // v
  float current_changer_v = 0.0f; //v
    // static float current_changer_x = 0.0f; //
  
  float eyelid_offset_x = 0.0f;  // -1.0〜1.0でx方向にまぶたをずらす
  float iris_scale_val = 1.0f;    // 虹彩のスケール
  float upperlid_val = 0.0f;
  float iris_zoom = 1.0f;
  float changer_v = 0.0f;
  int eyelid_x = 70;
  int eyelid_y = -30;
  int dx_changer = 105, dy_changer_u = 70, dy_changer_l = 60;

  float arousal_val = 0.0f;
  float valence_val = 0.0f;
  float dominance_val = 0.0f;

  int update_step = 0;
  static constexpr int num_steps = 5;

public:
  void init(const char *path_jpg_eyeball, const char *path_jpg_iris, const char *path_jpg_upperlid,
            const char *path_jpg_upperchanger, const char *path_jpg_lowerchanger,
            const int image_width, const int image_height, int rotation = 0)

  {
    this->image_width = image_width;
    this->image_height = image_height;

    lcd.init();
    lcd.setRotation(rotation);

    // 目の部位を描写するSpriteを準備
    sprite_eye.createSprite(image_width, image_height);
    sprite_eye.fillScreen(TFT_WHITE);
    sprite_eye.drawJpgFile(SPIFFS, path_jpg_eyeball);

    // 目玉を描写するSpriteを準備
    sprite_eyeball.createSprite(image_width, image_height);
    sprite_eye.fillScreen(TFT_WHITE);
    sprite_eyeball.drawJpgFile(SPIFFS, path_jpg_eyeball);
    
    // 上瞼を描写するSpriteを準備
    sprite_upperlid.createSprite(image_width, image_height);
    sprite_eye.fillScreen(TFT_WHITE);
    // const bool success_load_upperlid_image= sprite_upperlid.drawPngFile(SPIFFS, path_png_upperlid);
    // if (!success_load_upperlid_image){
    //   Serial.println("cant upload upperlid");
    // }
    const bool success_load_upperlid_image = sprite_upperlid.drawJpgFile(SPIFFS, path_jpg_upperlid); 
    Serial.println(success_load_upperlid_image); // 上瞼の画像がloadできていることを確認

    // 虹彩を描写するSpriteを準備
    sprite_iris.createSprite(image_width, image_height);
    sprite_eye.fillScreen(TFT_WHITE);
    const bool success_load_iris_image = sprite_iris.drawJpgFile(SPIFFS, path_jpg_iris);
    Serial.println(success_load_iris_image);
 
    // 上の目の変化を描写するSpriteを準備
    sprite_upperchanger.createSprite(image_width, image_height);
    sprite_eye.fillScreen(TFT_WHITE);
    const bool success_load_upperchanger_image = sprite_upperchanger.drawJpgFile(SPIFFS, path_jpg_upperchanger);
    Serial.println(success_load_upperchanger_image);

    // 下の目の変化を描写するSpriteを準備
    sprite_lowerchanger.createSprite(image_width, image_height);
    sprite_eye.fillScreen(TFT_WHITE);
    const bool success_load_lowerchanger_image = sprite_lowerchanger.drawJpgFile(SPIFFS, path_jpg_lowerchanger);
    Serial.println(success_load_lowerchanger_image);
   
    // 瞳孔を描写するSpriteを準備
    sprite_pupil.createSprite(image_width, image_height);
    sprite_pupil.fillScreen(TFT_WHITE);
    sprite_pupil.fillCircle(55, 55, 6, TFT_LIGHTGRAY);

    // 光の反射を描画するSpriteを準備
    // sprite_reflex.createSprite(image_width, image_height);
    // sprite_eye.fillScreen(TFT_WHITE);
    // sprite_reflex.fillCircle(50, 50, 6, TFT_LIGHTGRAY);
    
    // lcdを準備
    lcd.setPivot(lcd.width() >> 1, lcd.height() >> 1);
    lcd.fillScreen(TFT_WHITE);

    // zoom率を指定
    zoom_ratio = (float)lcd.width() / image_width;
    float ztmp = (float)lcd.height() / image_height;
    
    if (zoom_ratio > ztmp)
    {
      zoom_ratio = ztmp;
    }
  }

  // 通常の目を描画する準備
  void ready_for_normal_eye(const char *path_jpg_iris = "/white.jpg", const char *path_jpg_upperchanger = "/white.jpg", const char *path_jpg_lowerchanger = "/white.jpg", const char *path_jpg_upperlid = "/white.jpg")
  {
    // 虹彩を描写するSpriteを準備
    sprite_iris.createSprite(image_width, image_height);
    sprite_eye.fillScreen(TFT_WHITE);
    const bool success_load_iris_image = sprite_iris.drawJpgFile(SPIFFS, path_jpg_iris);
    Serial.println(success_load_iris_image);
 
    // 上の目の変化を描写するSpriteを準備
    sprite_upperchanger.createSprite(image_width, image_height);
    sprite_eye.fillScreen(TFT_WHITE);
    const bool success_load_upperchanger_image = sprite_upperchanger.drawJpgFile(SPIFFS, path_jpg_upperchanger);
    Serial.println(success_load_upperchanger_image);

    // 下の目の変化を描写するSpriteを準備
    sprite_lowerchanger.createSprite(image_width, image_height);
    sprite_eye.fillScreen(TFT_WHITE);
    const bool success_load_lowerchanger_image = sprite_lowerchanger.drawJpgFile(SPIFFS, path_jpg_lowerchanger);
    Serial.println(success_load_lowerchanger_image);
   
    // 瞳孔を描写するSpriteを準備
    sprite_pupil.createSprite(image_width, image_height);
    sprite_pupil.fillScreen(TFT_WHITE);
    sprite_pupil.fillCircle(55, 55, 6, TFT_LIGHTGRAY);
    
  }

  // 星目を描画する準備
  void ready_for_star_eye(const char *path_jpg_star= "/white.jpg")
  {
    // sprite_star.createSprite(image_width, image_height);
    // sprite_effect.createSprite(image_width, image_height);
    sprite_eye.fillScreen(TFT_WHITE);
    const bool success_load_effect_image = sprite_iris.drawJpgFile(SPIFFS, path_jpg_star);
    Serial.println(success_load_effect_image);

    // sprite_iris.fillScreen(TFT_WHITE);
  //   const bool success_load_iris_image = sprite_iris.drawJpgFile(SPIFFS, path_jpg_surprised_iris);
  //   Serial.println(success_load_iris_image);

  //   sprite_pupil.fillScreen(TFT_WHITE);
  //   sprite_pupil.fillCircle(image_height / 2, image_width / 2, 15, TFT_BLACK);

  //   sprite_reflex.fillScreen(TFT_WHITE);
  //   sprite_reflex.fillCircle(image_height / 2, image_width / 2, 7, TFT_LIGHTGRAY);
  }
  
  // ハート目を描画する準備
  void ready_for_heart_eye(const char *path_jpg_heart= "/white.jpg")
  {
    // sprite_heart.createSprite(image_width, image_height);
    // sprite_effect.createSprite(image_width, image_height);
    sprite_eye.fillScreen(TFT_WHITE);
    const bool success_load_effect_image = sprite_iris.drawJpgFile(SPIFFS, path_jpg_heart);
    Serial.println(success_load_effect_image);
  }

  // 涙目を描画する準備
  void ready_for_namida_eye(const char *path_jpg_iris = "/white.jpg", const char *path_jpg_upperlid = "/white.jpg", const char *path_jpg_lowerchanger = "/white.jpg", const char *path_jpg_namida= "/white.jpg")
  {
    // 上瞼を描写するSpriteを準備
    sprite_upperlid.createSprite(image_width, image_height);
    sprite_eye.fillScreen(TFT_WHITE);
    const bool success_load_upperlid_image = sprite_upperlid.drawJpgFile(SPIFFS, path_jpg_upperlid); 
    Serial.println(success_load_upperlid_image); // 上瞼の画像がloadできていることを確認

    // 虹彩を描写するSpriteを準備
    sprite_iris.createSprite(image_width, image_height);
    sprite_eye.fillScreen(TFT_WHITE);
    const bool success_load_iris_image = sprite_iris.drawJpgFile(SPIFFS, path_jpg_iris);
    Serial.println(success_load_iris_image);

    // 下の目の変化を描写するSpriteを準備
    sprite_lowerchanger.createSprite(image_width, image_height);
    sprite_eye.fillScreen(TFT_WHITE);
    const bool success_load_lowerchanger_image = sprite_lowerchanger.drawJpgFile(SPIFFS, path_jpg_lowerchanger);
    Serial.println(success_load_lowerchanger_image);

    sprite_eye.fillScreen(TFT_WHITE);
    const bool success_load_namida_image = sprite_upperchanger.drawJpgFile(SPIFFS, path_jpg_namida);
    Serial.println(success_load_namida_image);

    // 瞳孔を描写するSpriteを準備
    sprite_pupil.createSprite(image_width, image_height);
    sprite_pupil.fillScreen(TFT_WHITE);
    sprite_pupil.fillCircle(55, 55, 6, TFT_LIGHTGRAY);
    // sprite_upperlid.fillScreen(TFT_WHITE);
    // const bool success_load_sad_upperlid_image = sprite_upperlid.drawJpgFile(SPIFFS, path_jpg_sad_upperlid);
    // Serial.println("sad_eye");
    // Serial.println(success_load_sad_upperlid_image);
  }
  // 眠たいを描画する準備
  void ready_for_sleepy_eye(const char *path_jpg_iris = "/white.jpg", const char *path_jpg_upperlid = "/white.jpg", const char *path_jpg_upperchanger = "/white.jpg",const char *path_jpg_sleepy= "/white.jpg")
  {
    // 上瞼を描写するSpriteを準備
    sprite_upperlid.createSprite(image_width, image_height);
    sprite_eye.fillScreen(TFT_WHITE);
    const bool success_load_upperlid_image = sprite_upperlid.drawJpgFile(SPIFFS, path_jpg_upperlid); 
    Serial.println(success_load_upperlid_image); // 上瞼の画像がloadできていることを確認

    // 虹彩を描写するSpriteを準備
    sprite_iris.createSprite(image_width, image_height);
    sprite_eye.fillScreen(TFT_WHITE);
    const bool success_load_iris_image = sprite_iris.drawJpgFile(SPIFFS, path_jpg_iris);
    Serial.println(success_load_iris_image);

    // 下の目の変化を描写するSpriteを準備
    sprite_lowerchanger.createSprite(image_width, image_height);
    sprite_eye.fillScreen(TFT_WHITE);
    const bool success_load_lowerchanger_image = sprite_lowerchanger.drawJpgFile(SPIFFS, path_jpg_sleepy);
    Serial.println(success_load_lowerchanger_image);

    sprite_eye.fillScreen(TFT_WHITE);
    const bool success_load_namida_image = sprite_upperchanger.drawJpgFile(SPIFFS, path_jpg_upperchanger);
    Serial.println(success_load_namida_image);
  }

    void ready_for_fear_eye(const char *path_jpg_iris = "/white.jpg", const char *path_jpg_upperlid = "/white.jpg", const char *path_jpg_upperchanger = "/white.jpg",const char *path_jpg_fear= "/white.jpg")
  {
    // 上瞼を描写するSpriteを準備
    sprite_upperlid.createSprite(image_width, image_height);
    sprite_eye.fillScreen(TFT_WHITE);
    const bool success_load_upperlid_image = sprite_upperlid.drawJpgFile(SPIFFS, path_jpg_upperlid); 
    Serial.println(success_load_upperlid_image); // 上瞼の画像がloadできていることを確認

    // 虹彩を描写するSpriteを準備
    sprite_iris.createSprite(image_width, image_height);
    sprite_eye.fillScreen(TFT_WHITE);
    const bool success_load_iris_image = sprite_iris.drawJpgFile(SPIFFS, path_jpg_iris);
    Serial.println(success_load_iris_image);

    // 下の目の変化を描写するSpriteを準備
    sprite_lowerchanger.createSprite(image_width, image_height);
    sprite_eye.fillScreen(TFT_WHITE);
    const bool success_load_lowerchanger_image = sprite_lowerchanger.drawJpgFile(SPIFFS, path_jpg_fear);
    Serial.println(success_load_lowerchanger_image);

    sprite_eye.fillScreen(TFT_WHITE);
    const bool success_load_namida_image = sprite_upperchanger.drawJpgFile(SPIFFS, path_jpg_upperchanger);
    Serial.println(success_load_namida_image);
    
    // sprite_upperlid.fillScreen(TFT_WHITE);
    // const bool success_load_sad_upperlid_image = sprite_upperlid.drawJpgFile(SPIFFS, path_jpg_sad_upperlid);
    // Serial.println("sad_eye");
    // Serial.println(success_load_sad_upperlid_image);
  }

  void update_look(float dx = 0.0, float dy = 0.0, float scale = 10.0, float random_scale = 5.0)
  {
    // const int num_steps = 5;
    // max_blink_level = sizeof(upperlid_y_arr);

    // for (int i = 1; i <= num_steps; ++i) {
      // float t = i / (float)num_steps;

    // if (update_step == 0) update_finished = false;
    if (update_step > num_steps) update_step = num_steps;
    float t = update_step / (float)num_steps;
    float interp_dx = current_dx * (1.0f - t) + dx * t;
    float interp_dy = current_dy * (1.0f - t) + dy * t;
    float interp_upperlid_val = current_upperlid_val * (1.0f - t) + upperlid_val * t;
    float interp_offset_x = current_eyelid_offset_x * (1.0f - t) + eyelid_offset_x * t;
    float interp_iris_scale = current_iris_scale_val * (1.0f - t) + iris_scale_val * t;
    float interp_changer_v = current_changer_v * (1.0f - t) + changer_v * t;
    // float interp_changer_x = current_changer_x * (1.0f - t) + changer_x * t;

    // int eyelid_y = calcYfromLidValue(interp_upperlid_val);
    // int eyelid_y;
    eyelid_x = (int)(interp_offset_x * 30 + 70);
    iris_zoom = 1.2f + interp_iris_scale * 0.3f;
    // int dx_changer = 60;
    // int dy_changer_u = 70;
    // int dy_changer_l = 60;
    if (interp_changer_v < 0.0f) {
      dx_changer = (int)((interp_changer_v + 0.5f) * 5 + 20);
      dy_changer_u = (int)(-interp_changer_v * 32 + 70);
      if (dominance_val <= 0.0){
        dy_changer_l = (int)(interp_changer_v * 40 + 55);
      }else{
        dy_changer_l = 55;
      }
    } else if (interp_changer_v >= 0.0f) {
      dx_changer = (int)((interp_changer_v - 0.5f) * 5 + 105);
      dy_changer_u = (int)(interp_changer_v * 40 + 70);
      dy_changer_l = (int)(-interp_changer_v * 40 + 70);
    }

    long rx = (int)(random_scale * random(100) / 300);
    long ry = (int)(random_scale * random(100) / 300);

    sprite_eye.clear();
    sprite_eye.fillScreen(TFT_WHITE);
    sprite_eyeball.pushSprite(&sprite_eye, 0, 0, TFT_WHITE);

    sprite_iris.setSwapBytes(true);
    int iris_cx = image_width / 2 + (int)(scale * interp_dx) - 10;
    int iris_cy = image_height / 2 + (int)(scale * interp_dy);
    sprite_iris.pushRotateZoom(&sprite_eye, iris_cx, iris_cy, 0, iris_zoom, iris_zoom, TFT_WHITE);
    sprite_pupil.pushSprite(&sprite_eye, (int)(scale * interp_dx) + rx, (int)(scale * interp_dy) + ry, TFT_WHITE);
    sprite_upperchanger.pushRotateZoom(&sprite_eye, dx_changer, dy_changer_u, 0, 1.4f, 1.5f, TFT_WHITE);
    sprite_lowerchanger.pushRotateZoom(&sprite_eye, dx_changer, dy_changer_l, 0, 1.4f, 1.5f, TFT_WHITE);

    eyelid_y = calcYfromLidValue(interp_upperlid_val);
    sprite_upperlid.pushRotateZoom(&sprite_eye, eyelid_x, eyelid_y, 0, 1.5, 1.5, TFT_WHITE);
      // blink_level++;
      // if (blink_level == max_blink_level){
      //   blink_level = 0;
      // }
    sprite_eye.pushRotateZoom(&lcd, lcd.width() >> 1, lcd.height() >> 1, 0, zoom_ratio, zoom_ratio, TFT_WHITE);
    update_step++;
    // }

    // update current values
    if (update_step >= num_steps){
      update_step = 0;
      current_dx = dx;
      current_dy = dy;
      current_upperlid_val = upperlid_val;
      current_eyelid_offset_x = eyelid_offset_x;
      current_iris_scale_val = iris_scale_val;
      current_changer_v = changer_v;
      // update_finished = true;
    }
  }

  void reset_update_step() {
    update_step = 0;
  }

  bool isUpdateFinished() const {
    return update_step == 0;
  }

  int calcYfromLidValue(float val) {
    val = constrain(upperlid_val, 0.0f, 1.0f);
    int min_y = 102;  // 完全に開いたときのY座標
    int max_y = -25;     // 完全に閉じたときのY座標
    // 線形補間: 値が -1.0 なら min_y, 1.0 なら max_y
    int y = (int)((1.0f - val) * min_y + val * max_y);
    return y;
  }

  // std::vector<int> generate_upperlid_y_arr(float a) {
  //   std::vector<int> arr;

  //   // 共通の開閉シーケンス（端両側：閉じる）
  //   arr.push_back(0);
  //   arr.push_back(102-30-eyelid_y);

  //   // aに応じて開いた状態(0)の個数を変える
  //   int num;
  //   if (a <= -0.7f) {
  //     num = 9;
  //   } else if (a < -0.7f && a <= -0.4f) {
  //     num = 7;
  //   } else if (a < -0.4f && a <= 0.0f) {
  //     num = 5;
  //   } else if (a < 0.0f && a <= 0.4f) {
  //     num = 4;
  //   } else if (a < 0.4f && a <= 0.7f) {
  //     num = 3;
  //   } else {
  //     num = 1;
  //   }

  //   for (int i = 0; i < num; ++i) {
  //     arr.push_back(102-eyelid_y);
  //   }

  //   // 閉じる側の中間値とクローズ
  //   arr.push_back(102-10-eyelid_y);
  //   arr.push_back(102-40-eyelid_y);
  //   arr.push_back(0);

  //   return arr;
  // }

  void set_arousal_val(float a) {
    arousal_val = constrain(a, -1.0f, 1.0f);
    if (arousal_val <= -1.0f) {
      upperlid_val = 0.0f;
    } else if (arousal_val <= -0.5f) {
      // -1.0 → 0.0, -0.5 → 0.65
      float t = (arousal_val + 1.0f) / 0.5f;
      upperlid_val = 0.0f * (1 - t) + 0.65f * t;
    } else if (arousal_val <= 0.0f) {
      // -0.5 → 0.65, 0.0 → 0.75
      float t = (arousal_val + 0.5f) / 0.5f;
      upperlid_val = 0.65f * (1 - t) + 0.75f * t;
    } else if (arousal_val <= 0.4f) {
      // 0.0 → 0.75, 0.4 → 0.8
      float t = (arousal_val) / 0.4f;
      upperlid_val = 0.75f * (1 - t) + 0.8f * t;
    } else if (arousal_val <= 1.0f) {
      // 0.4 → 0.8, 1.0 → 1.0
      float t = (arousal_val - 0.4f) / 0.6f;
      upperlid_val = 0.8f * (1 - t) + 1.0f * t;
    } else {
      upperlid_val = 1.0f;
    }
    // upperlid_y_arr = generate_upperlid_y_arr(arousal_val);
  }
  void set_valence_val(float v) {
    valence_val = constrain(v, -1.0f, 1.0f);
    if (v >= -0.5f && v <= 0.5f) {
      iris_scale_val = v * 2.0f;
      changer_v = 0.0f;
    }
    else if (v > 0.5f) {
      // changer_v: v ∈ (0.5, 1.0] 00 1.0]
      changer_v = v / 0.5f - 1.0f;
      iris_scale_val = 1.0f;
    }
    else if (v < -0.5f) {
      // changer_v: v -1.0, -0.5) → changer_v-1.0,0.
      changer_v = v / 0.5f + 1.0f;
      iris_scale_val = 0.0f;
    }
  }
  void set_dominance_val(float d) {
    dominance_val = constrain(d, -1.0f, 1.0f);
    eyelid_offset_x = d;
  }
  // void set_eyelid_offset_x(float d) {
  //   eyelid_offset_x = constrain(d, -1.0f, 1.0f);
  // }

  // void set_iris_scale_val(float v) {
  //   iris_scale_val = constrain(v, -1.0f, 1.0f);
  // }

  // void set_changer_v(float val) {
  //   changer_v = constrain(val, -1.0f, 1.0f);
  // }

  // 瞬きの描画
  void blink_eye(float dx = 0.0, float dy = 0.0, int blink_level = 0 /*何コマ目か*/, float scale = 10.0, float random_scale = 5.0)
  {
    int upperlid_y_arr[] = {-130, -130, 0, 0, -130, -130}; // 上瞼のコマ送り時のy座標の配列
    long rx = (int)(random_scale * random(100) / 100);
    long ry = (int)(random_scale * random(100) / 100);
    
    sprite_eye.clear();
    sprite_eye.fillScreen(TFT_WHITE);
    sprite_eyeball.pushSprite(&sprite_eye, 0, 0, TFT_WHITE);
    // sprite_iris.pushSprite(&sprite_eye, 0, 0, TFT_WHITE);
    
    sprite_pupil.pushSprite(&sprite_eye, (int)(scale * dx), (int)(scale * dy), TFT_WHITE); // 瞳孔をランダムに動かす
    sprite_reflex.pushSprite(&sprite_eye, (int)(scale * dx) + rx, (int)(scale * dy) + ry, TFT_WHITE); // 光の反射をランダムに動かす
    
    sprite_upperlid.pushSprite(&sprite_eye, 0, upperlid_y_arr[blink_level], TFT_WHITE); // 上瞼を動かす

    sprite_eye.pushRotateZoom(&lcd, lcd.width() >> 1, lcd.height() >> 1, 0, zoom_ratio, zoom_ratio, TFT_WHITE); // sprite_eye をlcdに一括転送
  }


  // 驚きの目の描画
  void surprised(float dx = 0.0, float dy = 0.0, int surprised_level = 0 /*何コマ目か*/,  float scale = 10.0, float random_scale = 5.0)
  {
    int upperlid_y_arr[] = {- 130, - 130, -130, -130, 0, 0, -130, -130, -130, -130, 0, -130, -130, 0, -130, -130};
    long rx = (int)(random_scale * random(100) / 100);
    long ry = (int)(random_scale * random(100) / 100);

    sprite_eye.clear();
    sprite_eye.fillScreen(TFT_WHITE);
    sprite_eyeball.pushSprite(&sprite_eye, 0, 0, TFT_WHITE);

    sprite_iris.pushSprite(&sprite_eye, 0, 5, TFT_WHITE);
    sprite_pupil.pushSprite(&sprite_eye, (int)(scale * dx), (int)(scale * dy), TFT_WHITE);
    sprite_reflex.pushSprite(&sprite_eye, (int)(scale * dx) + rx, (int)(scale * dy) + ry, TFT_WHITE);
    sprite_upperlid.pushSprite(&sprite_eye, 0, upperlid_y_arr[surprised_level], TFT_WHITE);
    
    sprite_eye.pushRotateZoom(&lcd, lcd.width() >> 1, lcd.height() >> 1, 0, zoom_ratio, zoom_ratio, TFT_WHITE);
  }

  // 眠い目の描画
  // void sleepy(float dx = 0.0, float dy = 0.0, int sleepy_level = 0, float scale = 10.0, float random_scale = 5.0)
  // {
  //   int upperlid_y_arr[] = {- 70, - 70, -60, 0, 0, 0, -60, -70, -70, - 70};
  //   long rx = (int)(random_scale * random(100) / 100);
  //   long ry = (int)(random_scale * random(100) / 100);
    
  //   sprite_eye.clear();
  //   sprite_eye.fillScreen(TFT_WHITE);
  //   sprite_eyeball.pushSprite(&sprite_eye, 0, 0, TFT_WHITE);
  //   sprite_iris.pushSprite(&sprite_eye, -10, 15, TFT_WHITE);
    
  //   sprite_pupil.pushSprite(&sprite_eye, (int)(scale * dx) - 10, (int)(scale * dy) + 15, TFT_WHITE);
  //   sprite_upperlid.pushSprite(&sprite_eye, 0, upperlid_y_arr[sleepy_level]);

  //   sprite_eye.pushRotateZoom(&lcd, lcd.width() >> 1, lcd.height() >> 1, 0, zoom_ratio, zoom_ratio, TFT_WHITE);
  // }

  // // 怒った目の描画
  // void angry(float dx = 0.0, float dy = 0.0, int angry_level = 0, float scale = 10.0, float random_scale = 5.0)
  // {
  //   long rx = (int)(random_scale * random(100) / 100);
  //   long ry = (int)(random_scale * random(100) / 100);
    
  //   sprite_eye.clear();
  //   sprite_eye.fillScreen(TFT_WHITE);
  //   sprite_eyeball.pushSprite(&sprite_eye, 0, 0, TFT_WHITE);
  //   sprite_iris.pushSprite(&sprite_eye, 0, 0, TFT_WHITE);
    
  //   sprite_pupil.pushSprite(&sprite_eye, (int)(scale * dx), (int)(scale * dy), TFT_WHITE); // 瞳孔をランダムに動かす
  //   sprite_reflex.pushSprite(&sprite_eye, (int)(scale * dx) + rx, (int)(scale * dy) + ry + 10, TFT_WHITE); // 光の反射をランダムに動かす
    
  //   sprite_upperlid.pushSprite(&sprite_eye, 0, -10, TFT_WHITE);

  //   sprite_eye.pushRotateZoom(&lcd, lcd.width() >> 1, lcd.height() >> 1, 0, zoom_ratio, zoom_ratio, TFT_WHITE);
  // }

  // // 悲しい目の描画
  // void sad(float dx = 0.0, float dy = 0.0, int sad_level = 0, float scale = 10.0, float random_scale = 5.0)
  // {
  //   long rx = (int)(random_scale * random(100) / 100);
  //   long ry = (int)(random_scale * random(100) / 100);
    
  //   sprite_eye.clear();
  //   sprite_eye.fillScreen(TFT_WHITE);
  //   sprite_eyeball.pushSprite(&sprite_eye, 0, 0, TFT_WHITE);
  //   sprite_iris.pushSprite(&sprite_eye, 0, 0, TFT_WHITE);
    
  //   sprite_pupil.pushSprite(&sprite_eye, (int)(scale * dx), (int)(scale * dy), TFT_WHITE); // 瞳孔をランダムに動かす
  //   sprite_reflex.pushSprite(&sprite_eye, (int)(scale * dx) + rx, (int)(scale * dy) + ry + 10, TFT_WHITE); // 光の反射をランダムに動かす
    
  //   sprite_upperlid.pushSprite(&sprite_eye, 0, 0, TFT_WHITE);

  //   sprite_eye.pushRotateZoom(&lcd, lcd.width() >> 1, lcd.height() >> 1, 0, zoom_ratio, zoom_ratio, TFT_WHITE);
  // }

  // // 嬉しい目の描画
  // void happy(float dx = 0.0, float dy = 0.0, int happy_level = 0, float scale = 10.0, float random_scale = 5.0)
  // {
  //   long rx = (int)(random_scale * random(100) / 100);
  //   long ry = (int)(random_scale * random(100) / 100);
    
  //   sprite_eye.clear();
  //   sprite_eye.fillScreen(TFT_WHITE);
  //   sprite_eyeball.pushSprite(&sprite_eye, 0, 0, TFT_WHITE);
    
  //   sprite_upperlid.pushSprite(&sprite_eye, rx, ry, TFT_WHITE);

  //   sprite_eye.pushRotateZoom(&lcd, lcd.width() >> 1, lcd.height() >> 1, 0, zoom_ratio, zoom_ratio, TFT_WHITE);
  // }

  // ハート目の描画
  void heart(float dx = 0.0, float dy = 0.0, int heart_level = 0, float scale = 10.0, float random_scale = 5.0)
  {
    float iris_size_arr[] = {1.1f, 1.1f, 1.0f, 1.0f, 0.9f, 0.9f, 1.0f, 1.0f, 1.1f, 1.1f, 1.0f, 1.0f, 0.9f, 0.9f, 1.0f, 1.0f, 1.1f};
    long rx = (int)(random_scale * random(100) / 100);
    long ry = (int)(random_scale * random(100) / 100);
    
    sprite_eye.clear();
    sprite_eye.fillScreen(TFT_WHITE);
    sprite_eyeball.pushSprite(&sprite_eye, 0, 0, TFT_WHITE);
    sprite_iris.pushRotateZoom(&sprite_eye, 65, 70, 0, iris_size_arr[heart_level], iris_size_arr[heart_level], TFT_WHITE);
    sprite_eye.pushRotateZoom(&lcd, lcd.width() >> 1, lcd.height() >> 1, 0, zoom_ratio, zoom_ratio, TFT_WHITE);
  }

  // 星目の描画
  void star(float dx = 0.0, float dy = 0.0, int star_level = 0, float scale = 10.0, float random_scale = 5.0)
  {
    float iris_size_arr[] = {1.1f, 1.1f, 1.0f, 1.0f, 0.9f, 0.9f, 1.0f, 1.0f, 1.1f, 1.1f, 1.0f, 1.0f, 0.9f, 0.9f, 1.0f, 1.0f, 1.1f};
    long rx = (int)(random_scale * random(100) / 100);
    long ry = (int)(random_scale * random(100) / 100);
    
    sprite_eye.clear();
    sprite_eye.fillScreen(TFT_WHITE);
    sprite_eyeball.pushSprite(&sprite_eye, 0, 0, TFT_WHITE);
    sprite_iris.pushRotateZoom(&sprite_eye, 65, 70, 0, iris_size_arr[star_level], iris_size_arr[star_level], TFT_WHITE);
    sprite_eye.pushRotateZoom(&lcd, lcd.width() >> 1, lcd.height() >> 1, 0, zoom_ratio, zoom_ratio, TFT_WHITE);
  }

  // 涙目の表示
  void namida(float dx = 0.0, float dy = 0.0, float scale = 10.0, float random_scale = 5.0)
  {
    const int num_steps = 8;
    
    for (int i = 1; i <= num_steps; ++i) {
      float t = i / (float)num_steps;

      float interp_dx = current_dx * (1.0f - t) + dx * t;
      float interp_dy = current_dy * (1.0f - t) + dy * t;
      float interp_upperlid_val = current_upperlid_val * (1.0f - t) + upperlid_val * t;
      float interp_offset_x = current_eyelid_offset_x * (1.0f - t) + eyelid_offset_x * t;
      float interp_iris_scale = current_iris_scale_val * (1.0f - t) + iris_scale_val * t;
      float interp_changer_v = current_changer_v * (1.0f - t) + changer_v * t;

      eyelid_x = (int)(interp_offset_x * 30 + 70);
      iris_zoom = 1.2f + interp_iris_scale * 0.3f;

      if (interp_changer_v < 0.0f) {
        dx_changer = (int)((interp_changer_v + 0.5f) * 5 + 20);
        dy_changer_u = (int)(-interp_changer_v * 40 + 55);
        dy_changer_l = (int)(interp_changer_v * 40 + 55);
      } else if (interp_changer_v >= 0.0f) {
        dx_changer = (int)((interp_changer_v - 0.5f) * 5 + 105);
        dy_changer_u = (int)(interp_changer_v * 40 + 70);
        dy_changer_l = (int)(-interp_changer_v * 40 + 70);
      }

      long rx = (int)(random_scale * random(100) / 300);
      long ry = (int)(random_scale * random(100) / 300);
      int iris_cx = image_width / 2 + (int)(scale * interp_dx) - 10;
      int iris_cy = image_height / 2 + (int)(scale * interp_dy);

      sprite_eye.clear();
      sprite_eye.fillScreen(TFT_WHITE);
      sprite_eyeball.pushSprite(&sprite_eye, 0, 0, TFT_WHITE);
      sprite_iris.pushRotateZoom(&sprite_eye, iris_cx, iris_cy, 0, iris_zoom, iris_zoom, TFT_WHITE);
      sprite_pupil.pushSprite(&sprite_eye, (int)(scale * interp_dx) + rx, (int)(scale * interp_dy) + ry, TFT_WHITE);

      // namida
      sprite_lowerchanger.pushRotateZoom(&sprite_eye, dx_changer, dy_changer_l, 0, 0.8f, 1.5f, TFT_WHITE);
      sprite_upperchanger.pushRotateZoom(&sprite_eye, 80, 150, -40, 1.0f, 1.0f, TFT_WHITE);
      eyelid_y = calcYfromLidValue(interp_upperlid_val);
      sprite_upperlid.pushRotateZoom(&sprite_eye, eyelid_x, eyelid_y, 0, 1.5, 1.5, TFT_WHITE);
      sprite_eye.pushRotateZoom(&lcd, lcd.width() >> 1, lcd.height() >> 1, 0, zoom_ratio, zoom_ratio, TFT_WHITE);
    }
    current_dx = dx;
    current_dy = dy;
    current_upperlid_val = upperlid_val;
    current_eyelid_offset_x = eyelid_offset_x;
    current_iris_scale_val = iris_scale_val;
    current_changer_v = changer_v;
  }

  void sleepy(float dx = 0.0, float dy = 0.0, int sleepy_level = 0, float scale = 10.0, float random_scale = 5.0)
  {
    const int num_steps = 8;
    int upperlid_y_arr[] = {- 70, - 70, -60, 0, 0, 0, 0, 0, 0, 0, 0, 0,-20, -60, -70, -70};
    for (int i = 1; i <= num_steps; ++i) {
      float t = i / (float)num_steps;
      
      float interp_dx = current_dx * (1.0f - t) + dx * t;
      float interp_dy = current_dy * (1.0f - t) + dy * t;
      float interp_upperlid_val = current_upperlid_val * (1.0f - t) + upperlid_val * t;
      float interp_offset_x = current_eyelid_offset_x * (1.0f - t) + eyelid_offset_x * t;
      float interp_iris_scale = current_iris_scale_val * (1.0f - t) + iris_scale_val * t;
      float interp_changer_v = current_changer_v * (1.0f - t) + changer_v * t;

      eyelid_x = (int)(interp_offset_x * 30 + 70);
      iris_zoom = 1.2f + interp_iris_scale * 0.3f;
      if (interp_changer_v < 0.0f) {
        dx_changer = (int)((interp_changer_v + 0.5f) * 5 + 20);
        dy_changer_u = (int)(-interp_changer_v * 40 + 55);
        dy_changer_l = (int)(interp_changer_v * 40 + 55);
      } else if (interp_changer_v >= 0.0f) {
        dx_changer = (int)((interp_changer_v - 0.5f) * 5 + 105);
        dy_changer_u = (int)(interp_changer_v * 40 + 70);
        dy_changer_l = (int)(-interp_changer_v * 40 + 70);
      }
      
      long rx = (int)(random_scale * random(100) / 300);
      long ry = (int)(random_scale * random(100) / 300);
      int iris_cx = image_width / 2 + (int)(scale * interp_dx) - 10;
      int iris_cy = image_height / 2 + (int)(scale * interp_dy);
      
      sprite_eye.clear();
      sprite_eye.fillScreen(TFT_WHITE);
      sprite_eyeball.pushSprite(&sprite_eye, 0, 0, TFT_WHITE);
      sprite_iris.pushRotateZoom(&sprite_eye, iris_cx, iris_cy, 0, iris_zoom, iris_zoom, TFT_WHITE);
      sprite_upperchanger.pushRotateZoom(&sprite_eye, dx_changer, dy_changer_u, 0, 0.8f, 1.5f, TFT_WHITE);
      eyelid_y = calcYfromLidValue(interp_upperlid_val);
      sprite_upperlid.pushRotateZoom(&sprite_eye, eyelid_x, eyelid_y + upperlid_y_arr[sleepy_level], 0, 1.5, 1.5, TFT_WHITE);
      // sleepy
      sprite_lowerchanger.pushRotateZoom(&sprite_eye, 100, 30, 0, 1.0, 1.0, TFT_WHITE);
      sprite_eye.pushRotateZoom(&lcd, lcd.width() >> 1, lcd.height() >> 1, 0, zoom_ratio, zoom_ratio, TFT_WHITE);
    }
    current_dx = dx;
    current_dy = dy;
    current_upperlid_val = upperlid_val;
    current_eyelid_offset_x = eyelid_offset_x;
    current_iris_scale_val = iris_scale_val;
    current_changer_v = changer_v;
  }

  void fear(float dx = 0.0, float dy = 0.0, float scale = 10.0, float random_scale = 5.0)
  {
    const int num_steps = 8;

    for (int i = 1; i <= num_steps; ++i) {
      float t = i / (float)num_steps;

      float interp_dx = current_dx * (1.0f - t) + dx * t;
      float interp_dy = current_dy * (1.0f - t) + dy * t;
      float interp_upperlid_val = current_upperlid_val * (1.0f - t) + upperlid_val * t;
      float interp_offset_x = current_eyelid_offset_x * (1.0f - t) + eyelid_offset_x * t;
      float interp_iris_scale = current_iris_scale_val * (1.0f - t) + iris_scale_val * t;
      float interp_changer_v = current_changer_v * (1.0f - t) + changer_v * t;

      eyelid_x = (int)(interp_offset_x * 30 + 70);
      iris_zoom = 1.2f + interp_iris_scale * 0.3f;
      if (interp_changer_v < 0.0f) {
        dx_changer = (int)((interp_changer_v + 0.5f) * 5 + 20);
        dy_changer_u = (int)(-interp_changer_v * 40 + 55);
        dy_changer_l = (int)(interp_changer_v * 40 + 55);
      } else if (interp_changer_v >= 0.0f) {
        dx_changer = (int)((interp_changer_v - 0.5f) * 5 + 105);
        dy_changer_u = (int)(interp_changer_v * 40 + 70);
        dy_changer_l = (int)(-interp_changer_v * 40 + 70);
      }

      long rx = (int)(random_scale * random(100) / 300);
      long ry = (int)(random_scale * random(100) / 300);
      int iris_cx = image_width / 2 + (int)(scale * interp_dx) - 10;
      int iris_cy = image_height / 2 + (int)(scale * interp_dy);

      sprite_eye.clear();
      sprite_eye.fillScreen(TFT_WHITE);
      sprite_eyeball.pushSprite(&sprite_eye, 0, 0, TFT_WHITE);
      sprite_iris.pushRotateZoom(&sprite_eye, iris_cx, iris_cy, 0, iris_zoom, iris_zoom, TFT_WHITE);
      sprite_pupil.pushSprite(&sprite_eye, (int)(scale * interp_dx) + rx, (int)(scale * interp_dy) + ry, TFT_WHITE);

      // sprite_upperchanger.pushRotateZoom(&sprite_eye, 50, 100, 0, 1.0f, 1.0f, TFT_WHITE);
      eyelid_y = calcYfromLidValue(interp_upperlid_val);
      sprite_upperlid.pushRotateZoom(&sprite_eye, eyelid_x, eyelid_y, 0, 1.5, 1.5, TFT_WHITE);
      // fear
      sprite_lowerchanger.pushRotateZoom(&sprite_eye, 40, -20, 0, 1.0, 1.0, TFT_WHITE);
      sprite_eye.pushRotateZoom(&lcd, lcd.width() >> 1, lcd.height() >> 1, 0, zoom_ratio, zoom_ratio, TFT_WHITE);
    }
    current_dx = dx;
    current_dy = dy;
    current_upperlid_val = upperlid_val;
    current_eyelid_offset_x = eyelid_offset_x;
    current_iris_scale_val = iris_scale_val;
    current_changer_v = changer_v;
  }

  void draw_updated_image()
  {
    sprite_eye.pushRotateZoom(&lcd, lcd.width() >> 1, lcd.height() >> 1, 0, zoom_ratio, zoom_ratio, TFT_WHITE);
  }
};
