
/*
    Simple Touch Drawing sample for WT32-SC01
*/
#define LGFX_WT32_SC01  // Autodetect board
#define LGFX_USE_V1      // set to use new version of library

#include <LovyanGFX.hpp>
#include "wt32setup.h"
#include <LGFX_AUTODETECT.hpp>  // クラス"LGFX"を準備します
static LGFX lcd;  // declare display variable


/*
    Simple Touch Drawing sample for WT32-SC01-Plus_ESP32-S3
    Requirements:
    - Development board : WT32-SC01-Plus_ESP32-S3
    - Arduino Library - Display/Touch : LovyanGFX
    - Board selected in Arduino : ESP32S3 Dev Module
*/

#define LGFX_USE_V1         // set to use new version of library
#include <LovyanGFX.hpp>    // main library
#include "esp32-hal-psram.h"

// Portrait
#define TFT_WIDTH   320
#define TFT_HEIGHT  480

class LGFX : public lgfx::LGFX_Device
{
  lgfx::Panel_ST7796  _panel_instance;  // ST7796UI
  lgfx::Bus_Parallel8 _bus_instance;    // MCU8080 8B
  lgfx::Light_PWM     _light_instance;
  lgfx::Touch_FT5x06  _touch_instance;

public:
  LGFX(void)
  {
    {
      auto cfg = _bus_instance.config();
      cfg.freq_write = 40000000;    
      cfg.pin_wr = 47;             
      cfg.pin_rd = -1;             
      cfg.pin_rs = 0;              

      // LCD data interface, 8bit MCU (8080)
      cfg.pin_d0 = 9;              
      cfg.pin_d1 = 46;             
      cfg.pin_d2 = 3;              
      cfg.pin_d3 = 8;              
      cfg.pin_d4 = 18;             
      cfg.pin_d5 = 17;             
      cfg.pin_d6 = 16;             
      cfg.pin_d7 = 15;             

      _bus_instance.config(cfg);   
      _panel_instance.setBus(&_bus_instance);      
    }

    { 
      auto cfg = _panel_instance.config();    

      cfg.pin_cs           =    -1;  
      cfg.pin_rst          =    4;  
      cfg.pin_busy         =    -1; 

      cfg.panel_width      =   TFT_WIDTH;
      cfg.panel_height     =   TFT_HEIGHT;
      cfg.offset_x         =     0;
      cfg.offset_y         =     0;
      cfg.offset_rotation  =     0;
      cfg.dummy_read_pixel =     8;
      cfg.dummy_read_bits  =     1;
      cfg.readable         =  false;
      cfg.invert           = true;
      cfg.rgb_order        = false;
      cfg.dlen_16bit       = false;
      cfg.bus_shared       = false;

      _panel_instance.config(cfg);
    }

    {
      auto cfg = _light_instance.config();    

      cfg.pin_bl = 45;              
      cfg.invert = false;           
      cfg.freq   = 44100;           
      cfg.pwm_channel = 7;          

      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance);  
    }

    { 
      auto cfg = _touch_instance.config();

      cfg.x_min      = 0;
      cfg.x_max      = 319;
      cfg.y_min      = 0;  
      cfg.y_max      = 479;
      cfg.pin_int    = 7;  
      cfg.bus_shared = true; 
      cfg.offset_rotation = 0;

      cfg.i2c_port = 1;//I2C_NUM_1;
      cfg.i2c_addr = 0x38;
      cfg.pin_sda  = 6;   
      cfg.pin_scl  = 5;   
      cfg.freq = 400000;  

      _touch_instance.config(cfg);
      _panel_instance.setTouch(&_touch_instance);  
    }

    setPanel(&_panel_instance); 
  }
};

static LGFX lcd;            // declare display variable

// Variables for touch x,y
static int32_t x,y;

void setup(void)
{
  lcd.init();

  // Setting display to landscape
  if (lcd.width() < lcd.height()) lcd.setRotation(lcd.getRotation() ^ 1);

  lcd.setCursor(0,0);
  lcd.printf("Ready to touch & draw!\n");

  lcd.setCursor(0,300);
  // Get flash size and display
  lcd.printf("FLASH size: %d bytes",ESP.getFlashChipSize());

  lcd.setCursor(0,310);
  // Initialize PSRAM (if available) and show the total size
  if (psramFound()) {
    lcd.printf("PSRAM size: %d bytes",heap_caps_get_total_size(MALLOC_CAP_SPIRAM));
    
  } else {
    lcd.printf("No PSRAM found.");
  }
}

void loop()
{
  if (lcd.getTouch(&x, &y)) {
    lcd.fillRect(x-2, y-2, 5, 5, TFT_RED);
    lcd.setCursor(380,0);
    lcd.printf("Touch:(%03d,%03d)", x,y);
  }
}