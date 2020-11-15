#include <SPI.h>
#include <Ethernet.h>
#include <SdFat.h>
#include <sdios.h>
#include <Regexp.h>

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
		String http_response_header;
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
		
		// handle space URL encoding
		requested_file.replace("%20", " ");

		// check if the file exist, if yes, send a 200 and the file, otherwise send a 404
		if (sd.exists(requested_file)) {
			http_response_header = build_http_response_header(true, requested_file);
			file = sd.open(requested_file);
		} else {
			http_response_header = build_http_response_header(false, "/404.html");
			file = sd.open("/404.html");
		}

		// send the HTTP header and content to the client
		client.print(http_response_header);
		// this way of reading the file and sending it to the client is super slow and should be replaced with something faster
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

String build_http_response_header(bool success, String filename) {
	String extension = filename.substring(filename.lastIndexOf('.') + 1);
	String mime_type = "text/plain"; // default value is for plaintext
	
	//converting the file extension to the appropriate MIME type
	// text
	if (extension == "html" || extension == "htm") mime_type = "text/html";
	else if (extension == "css")                   mime_type = "text/css";
	else if (extension == "js")                    mime_type = "text/javascript";
	else if (extension == "json")                  mime_type = "text/json";
	// image
	else if (extension == "png")                        mime_type = "image/png";
	else if (extension == "jpg" || extension == "jpeg") mime_type = "image/jpeg";
	else if (extension == "ico")                        mime_type = "image/vnd.microsoft.icon";
	else if (extension == "svg")                        mime_type = "image/svg+xml";
	else if (extension == "gif")                        mime_type = "image/gif";
	// application
	else if (extension == "rar") mime_type = "application/vnd.rar";
	else if (extension == "tar") mime_type = "application/x-tar";
	else if (extension == "zip") mime_type = "application/zip";
	else if (extension == "7z")  mime_type = "application/x-7z-compressed";
	else if (extension == "swf") mime_type = "application/x-shockwave-flash";
	
	// construct the final response header
	return
		String("HTTP/1.1 ")
		+ (success ? "200 OK" : "404 Not Found")
		+ "\r\n"
		"Content-Type: "
		+ mime_type
		+ "\r\n"
		"Connection: close\r\n"
		"\r\n";
}
