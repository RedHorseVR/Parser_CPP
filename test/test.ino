#include "esp_camera.h" );  //////
#include <WiFi.h>
#include "FS.h" );
#include "SD_MMC.h"
#include <Vector.h>

void callback( )
	{
	Serial.println("==========CALLED=========");
	}
char * readLine(  File file , char * buffer, int buff_size )
	{
	buffer[0]  = NULL;
	
	
	int  len=0;
	if(!file)
	{
		Serial.println("Failed to open file for reading");
		return NULL;
		}
	char c = file.read();
	while( file.available()   &&  len < buff_size  ){
		
		if( c == '\n' )
		{
			buffer[len] = NULL;
			break;
		}else{
			buffer[len++] = c;
			}
		c = file.read();
		}
	
	return buffer;  }
void setupSDCARD()
	{
	Serial.begin(115200);
	if(!SD_MMC.begin())
	{
		Serial.println("Card Mount Failed");
		return;
		}
	uint8_t cardType =SD_MMC.cardType();
	if(cardType == CARD_NONE)
	{
		Serial.println("No SD_MMC card attached");
		return;
		}
	Serial.print("SD_MMC Card Type: ");
	if(cardType == CARD_MMC)
	{
		Serial.println("MMC");
		}
	else
		if(cardType == CARD_SD)
		{
			Serial.println("SDSC");
			}
		else
			    if(cardType == CARD_SDHC)
			{
				Serial.println("SDHC");
				}
			else
				{
				Serial.println("UNKNOWN");
				
			
		}
	uint64_t cardSize =SD_MMC.cardSize() / (1024 * 1024);
	Serial.printf("SD_MMC Card Size: %lluMB\n", cardSize);
	Serial.printf("Total space: %lluMB\n", SD_MMC.totalBytes() / (1024 * 1024));
	Serial.printf("Used space: %lluMB\n", SD_MMC.usedBytes() / (1024 * 1024));
	listDir(SD_MMC, "/", 0);
	
	
	char ssid[100];
	char pwd[100];
	File file =SD_MMC.open( "/network.txt" );
	readLine( file,  ssid, 100 );
	readLine( file,  pwd, 100 );
	Serial.println("\n----------------------------------------------------------------------");
	Serial.println( ssid );
	Serial.println( pwd );
	}
void listDir(fs::FS &fs, const char * dirname, uint8_t levels)
	{
	Serial.printf("Listing directory: %s\n", dirname);
	File root =fs.open(dirname);
	if(!root)
	{
		Serial.println("Failed to open directory");
		return;
		
	}
	if(!root.isDirectory())
	{
		Serial.println("Not a directory");
		return;
		
	} File file =root.openNextFile();
	while(file){
		if(file.isDirectory())
		{
			Serial.print("  DIR : ");
			Serial.println(file.name());
			if(levels)
			{
				listDir(fs, file.name(), levels -1);
				
			}
		} else
			{
			Serial.print("  FILE: ");
			Serial.print(file.name());
			Serial.print("  SIZE: ");
			Serial.println(file.size());
			
		} file =root.openNextFile();
		}
	
	}
void disconnect_bluetooth()
	
	
	
	
	
	
	
	
	
	
	
	
	{
	}
// Partial images will be transmitted if image exceeds buffer size
//#define CAMERA_MODEL_M5STACK_PSRAM // Has PSRAM
//#define CAMERA_MODEL_M5STACK_ESP32CAM // No PSRAM
#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"
void startCameraServer();
void setup()
	{
	Serial.begin(115200);
	camera_config_t config;
	config.ledc_channel =LEDC_CHANNEL_0;
	config.ledc_timer =LEDC_TIMER_0;
	config.pin_d0 =Y2_GPIO_NUM;
	config.pin_d1 =Y3_GPIO_NUM;
	config.pin_d2 =Y4_GPIO_NUM;
	config.pin_d3 =Y5_GPIO_NUM;
	config.pin_d4 =Y6_GPIO_NUM;
	config.pin_d5 =Y7_GPIO_NUM;
	config.pin_d6 =Y8_GPIO_NUM;
	config.pin_d7 =Y9_GPIO_NUM;
	config.pin_xclk =XCLK_GPIO_NUM;
	config.pin_pclk =PCLK_GPIO_NUM;
	config.pin_vsync =VSYNC_GPIO_NUM;
	config.pin_href =HREF_GPIO_NUM;
	config.pin_sscb_sda =SIOD_GPIO_NUM;
	config.pin_sscb_scl =SIOC_GPIO_NUM;
	config.pin_pwdn =PWDN_GPIO_NUM;
	config.pin_reset =RESET_GPIO_NUM;
	config.xclk_freq_hz =20000000;
	config.pixel_format =PIXFORMAT_JPEG;
	if(psramFound())
	{
		Serial.println("Higher Quality" );
		config.frame_size =FRAMESIZE_QVGA;
		config.jpeg_quality =12;
		config.fb_count =2;
		}
	else
		{
		Serial.println("Lower Quality" );
		config.frame_size =FRAMESIZE_SVGA;
		config.jpeg_quality =12;
		config.fb_count =1;
		}
	#if defined(CAMERA_MODEL_ESP_EYE)
	
		pinMode(13, INPUT_PULLUP);
		pinMode(14, INPUT_PULLUP);
		#endif
	esp_err_t err =esp_camera_init(&config);
	if (err != ESP_OK)
	{
		Serial.printf("Camera init failed with error 0x%x", err);
		return;
		
	} sensor_t * s =esp_camera_sensor_get();
	if (s->id.PID == OV3660_PID)
	{
		s->set_vflip(s, 1);
		s->set_brightness(s, 1);
		s->set_saturation(s, -2);
		}
	#if defined(CAMERA_MODEL_M5STACK_WIDE) || defined(CAMERA_MODEL_M5STACK_ESP32CAM)
	
		s->set_vflip(s, 1);
		s->set_hmirror(s, 1);
		#endif
	
	
	
	if(!SD_MMC.begin())
	{
		Serial.println("Card Mount Failed");
		return;
		}
	char ssid[100];
	char pwd[100];
	char staticIP[100];
	File file =SD_MMC.open( "/network.txt" );
	readLine( file,  ssid, 100 );
	readLine( file,  pwd, 100 );
	readLine( file,  staticIP, 100 );
	
	int hostNumber;
	IPAddress local_IP = getIPfromString ( staticIP, &hostNumber  );
	if( hostNumber % 2 == 1 )
	{
		Serial.println("ODD HOST - flipping ");
		s->set_vflip(s, 1);
		s->set_hmirror(s, 1);
		}
	
	IPAddress gateway(192, 168, 0, 1);
	IPAddress subnet(255, 255, 255, 0);
	IPAddress primaryDNS(8, 8, 8, 8);
	IPAddress secondaryDNS(8, 8, 4, 4);
	
	
	if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
	
		Serial.println("STA Failed to configure");
		}
	
	WiFi.begin(ssid, pwd);
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
		}
	
	Serial.println("");
	Serial.println("WiFi connected");
	startCameraServer();
	Serial.print("Camera Ready! Use http: ");
	Serial.print(WiFi.localIP());
	Serial.println("' to connect");
	}
IPAddress getIPfromString (  char * chs , int *hostNumber ){
	String S = String( chs ) ;
	int A, B, C, D;
	int a = S.indexOf( '.' );
	int b = S.indexOf( '.' , a+1 );
	int c = S.indexOf( '.' , b+1 );
	A = S.substring(0, a ).toInt();
	B = S.substring(a+1, b ).toInt();
	C = S.substring(b+1, c ).toInt();
	D = S.substring( c+1 ).toInt();
	*hostNumber = D;
	Serial.println( D );
	IPAddress IP( A, B, C, D );
	return IP ;  }
void loop()
	{
	delay(10000);
	}

//  Export  Date: 07:21:59 PM - 22:Apr:2025;

