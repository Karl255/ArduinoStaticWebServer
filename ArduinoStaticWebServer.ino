#include <SPI.h>
#include <Ethernet.h>
#include <SdFat.h>
#include <sdios.h>
#include <Regexp.h>

// very basic success and failure response headers
const char* http_response_200 =
"HTTP/1.1 200 OK\r\n"
"Content-Type: text/html\r\n"
"Connection: close\r\n"
"\r\n";

const char* http_response_404 =
"HTTP/1.1 404 Not Found\r\n"
"Content-Type: text/html\r\n"
"Connection: close\r\n"
"\r\n";

String requested_file; // global buffer for the requested file name
byte mac[]{ 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // default MAC address from the Arduino Ethernet examples, should probably change this
IPAddress ip(192, 168, 137, 177); // network ip address of the ethernet NIC
EthernetServer server(80); // initializing the web server on port 80
SdFat32 sd;

void setup() {
	Ethernet.init(10);   // ethernet chip's SS is on pin 10
	Ethernet.begin(mac, ip);
	server.begin();      // starting the server
	// initializing the SD card library with the settings: SS on pin 4, SPI is shared 
	sd.begin(SdSpiConfig(4, SHARED_SPI));
}

void loop() {
	// get the connected client (if none, then client is a value that evaluates to false)
	EthernetClient client = server.available();

	if (client) {
		String get_request = "";
		const char* selected_response;
		File32 file;
		
		// read the first line of the HTTP header into get_request
		// example of what it will read: GET / HTTP/1.1\r\n
		while (client.connected() && client.available()) {
			char c = client.read();
			get_request += c;
			if (c == '\n') break;
		}
		
		// capture the requested file path
		MatchState ms;
		ms.Target(get_request.c_str());
		ms.GlobalMatch("GET (.+) HTTP/", request_matched_callback);
		
		// default file is index.html
		if (requested_file[requested_file.length() - 1] == '/')
			requested_file += "index.html";
		
		// check if the file exist, if yes, send a 200 and the file, otherwise send a 404
		if (sd.exists(requested_file)) {
			selected_response = http_response_200;
			file = sd.open("/index.html");
		} else {
			selected_response = http_response_404;
			file = sd.open("/404.html");
		}
		
		// send the HTTP header and file to the client
		client.print(selected_response);
		while (file.available())
			client.write((char)file.read());
		
		file.close();
		client.flush();
		client.stop();
	}
}

// callback which stores the captured file path into the global variable requested_file
// the Regexp library only takes function pointers so using a lambda that captures a scoped variable isn't possible
void request_matched_callback(const char* match, const unsigned int length, const MatchState& ms) {
	char capture_buf[64];
	ms.GetCapture(capture_buf, 0);
	requested_file = String(capture_buf);
}
