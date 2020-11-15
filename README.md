# Arduino Static Web Server
This is a very simple and basic (and shitty) static web server for Arduino using the Ethernet shield which also includes an SD card slot (in which the served files are stored).

### Supports:
- GET requests
- 404 error pages
- default page (/ -> /index.html)
- text/html content type

### Doesn't support:
- POST and all other requests
- all status codes except 200 and 404
- any HTTP version other than 1.1
- almost all HTTP header fields and those ones which are supported have very basic support
- caching
- redirection
- multiple simultaneous connections

## Dependencies
- built in SPI library
- built in Ethernet library for WIZnet SPI ethernet chips (used on the ethernet shield)
- [SdFat](https://github.com/greiman/SdFat) library by Bill Greiman
- [Regexp](https://github.com/nickgammon/Regexp) library by Nick Gammon
