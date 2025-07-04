#include "wifi_board.h"
#include "audio_codec.h"
#include "audio_codecs/no_audio_codec.h"
#include "display/lcd_display.h"
#include "application.h"
#include "button.h"
#include "config.h"
#include "led/single_led.h"
#include "iot/thing_manager.h"
#include "i2c_device.h"

#include <wifi_station.h>
#include <esp_log.h>
#include <driver/i2c_master.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>
#include <esp_lcd_panel_io.h>
#include <esp_lcd_panel_vendor.h>
#include <esp_lcd_panel_ops.h>

#define TAG "df-k20"

LV_FONT_DECLARE(font_puhui_20_4);
LV_FONT_DECLARE(font_awesome_20_4);

class DF_K20Board : public WifiBoard {
private:
    Button boot_button_;
    void InitializeButtons() {
        boot_button_.OnClick([this]() {
            auto& app = Application::GetInstance();
            if (app.GetDeviceState() == kDeviceStateStarting && !WifiStation::GetInstance().IsConnected()) {
                ResetWifiConfiguration();
            }
        });
        boot_button_.OnPressDown([this]() {
            Application::GetInstance().StartListening();
        });
        boot_button_.OnPressUp([this]() {
            Application::GetInstance().StopListening();
        });
    }

    // 物联网初始化，添加对 AI 可见设备
    void InitializeIot() {
        auto& thing_manager = iot::ThingManager::GetInstance();
        thing_manager.AddThing(iot::CreateThing("Speaker"));
        //thing_manager.AddThing(iot::CreateThing("Screen"));
    }

public:
    DF_K20Board() : boot_button_(BOOT_BUTTON_GPIO) {
        //InitializeI2c();
        //InitializeATK_ST7789_80_Display();
        InitializeButtons();
        InitializeIot();
    }

    virtual AudioCodec* GetAudioCodec() override {
        static NoAudioCodecSimplexPdm audio_codec(AUDIO_INPUT_SAMPLE_RATE, AUDIO_OUTPUT_SAMPLE_RATE,
            AUDIO_SPK_BCLK, AUDIO_SPK_WS, AUDIO_SPK_DOUT, AUDIO_MIC_CLK, AUDIO_MIC_DIN);
        return &audio_codec;
    }
};

DECLARE_BOARD(DF_K20Board);
