## OS
Fedora Workstation 44
## Arduino installation
```curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh```

sudo mv bin/arduino-cli /usr/local/bin/

### Libraries installation
- ```sudo arduino-cli core update-index```
- ```sudo arduino-cli core install arduino:avr```
- ```sudo arduino-cli lib install "HID-Project"```

### Port identification
```ls /dev/tty*```
### checking available FQBNs
```arduino-cli board listall | grep -i micro```
### Sketch compilation
```sudo arduino-cli compile --fqbn arduino:avr:_boardname_ ~/Documents/case_project_driver/case_project/```
### Sketch upload
```sudo arduino-cli upload -p /dev/_boardport_ --fqbn arduino:avr:_boardname_ ~/Documents/case_project_driver/case_project/```
### Serial port monitoring
```sudo arduino-cli monitor -p /dev/_boardport_ -b arduino:avr:_boardname_```