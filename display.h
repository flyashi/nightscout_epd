#define ENABLE_GxEPD2_GFX 0

#include <GxEPD2_BW.h>

#define GxEPD2_DISPLAY_CLASS GxEPD2_BW
#define GxEPD2_DRIVER_CLASS GxEPD2_213_BN
#define GxEPD2_BW_IS_GxEPD2_BW true
#define MAX_DISPLAY_BUFFER_SIZE 65536ul // e.g.


//#define GxEPD2_DRIVER_CLASS GxEPD2_213_B74
#define EPD_CS SS
//#define MAX_DISPLAY_BUFFER_SIZE (81920ul-34000ul-5000ul) // ~34000 base use, change 5000 to your application use

//#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8) ? EPD::HEIGHT : MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8))
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8) ? EPD::HEIGHT : MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8))

void fullscreen_message(char *message);
void display_init();
void display_hibernate();
void fullscreen_message_subtitle(char* message, char* subtitle);