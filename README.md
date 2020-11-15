# Arduino Static Web Server
This is a very simple and basic (and shitty) static web server for Arduino using the Ethernet shield which also includes an SD card slot (in which the served files are stored). I barely use C++ so don't expect this code to be written in the best way possible.

### Supports:
- GET requests
- 404 error pages
- default page (/ -> /index.html)
- MIME types, a couple are implemented already and adding more is easy (unless the file contents affect the MIME type)
- URL encoding code for spaces (only)

### Doesn't support:
- POST and all other HTTP requests
- all status codes except 200 and 404
- any HTTP version other than 1.1
- almost all HTTP header fields and those ones which are supported have very basic support
- caching
- redirection
- multiple simultaneous connections
- URL encoding/decoding (except spaces)


## Dependencies
- built in SPI library
- built in Ethernet library for WIZnet SPI ethernet chips (used on the ethernet shield)
- [SdFat](https://github.com/greiman/SdFat) library by Bill Greiman
- [Regexp](https://github.com/nickgammon/Regexp) library by Nick Gammon
