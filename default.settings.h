#ifndef HEADER_FILE
#define HEADER_FILE

// Note no semicolon after a hash define

//Local Wi-Fi information for garduino connection
#define WIFI_SSID "name_of_wifi_network"      //ssid of wifi network espduino should connect to
#define WIFI_PSWD "wifi_network_password"	    //password of above wifi network

//Server information of garduino monitoring station
//basis format: http://HOST_IP:HOST_PORT/HOST_URL
#define HOST_IP "address_of_monitor_server" 	//ip address or domain name of server to send sensor infoto
#define HOST_URL ""                           //path (not required)
#define HOST_PORT 80				                  //port server is listening on

//Server information of Qoncrete for logging/graphing data
#define QONCRETE_HOST_ADDRESS "log.qoncrete.com"  
#define QONCRETE_HOST_URL "/0b058f5a-90b8-478d-89c1-8c7d0d4d5cc9?token=686164d2-327a-4559-8579-c22be4da989f" 
#define QONCRETE_HOST_PORT 443                    

#endif
