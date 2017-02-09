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


#endif
