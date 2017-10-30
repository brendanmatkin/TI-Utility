# TI-Utility
Utility apps, sketches, snippets, etc. Things that help me to other things and are not restricted to single projects. 

## Arduino
###  OTA_serverCheck
This is a sketch for ESP8266. It just checks an HTTP server for a newer firmware version (it's version is 0).

The folder that it checks must have a file called fw.version with a single number in it (the firmware version). Copy the new fw.bin into that folder before updating the version number in the file. 
