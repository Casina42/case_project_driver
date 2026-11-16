## Arduino installation
curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh

sudo mv bin/arduino-cli /usr/local/bin/

### Libraries installation
- sudo arduino-cli core update-index
- sudo arduino-cli core install arduino:avr
- sudo arduino-cli lib install "HID-Project"

### Sketch compilation
sudo arduino-cli compile --fqbn arduino:avr:micro ~/Documents/case_project_driver/case_project/
### Sketch upload
sudo arduino-cli upload -p /dev/ttyACM0 --fqbn arduino:avr:micro ~/Documents/case_project_driver/case_project/
### Serial port monitoring
sudo arduino-cli monitor -p /dev/ttyACM0 -b arduino:avr:micro