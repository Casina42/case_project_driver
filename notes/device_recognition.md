# Edit rules
## Shell commands
The vendor id and the product id can be read with **lsusb** and eventually changed on the device to make sure only that device is associated with this port.
1) ```sudo nano /etc/udev/rules.d/99-arduino.rules```
2) Create a rule with ```SUBSYSTEM=="tty", ATTRS{idVendor}=="2341", ATTRS{idProduct}=="0043", ATTRS{serial}=="CPU_MONITOR", SYMLINK+="arduino_cpu"```
3) ```sudo udevadm control --reload-rules```
4) ```sudo udevadm trigger```


# Creating custom boards
1) find ```~/.arduino15/packages/arduino/hardware/avr/_version_/boards.txt```
2) find ```micro.name=Arduino Micro```
3) Add a custom board as follows
```microcpu.name=Arduino Micro CPU monitor

microcpu.upload.tool=avrdude
microcpu.upload.protocol=avr109
microcpu.upload.maximum_size=28672
microcpu.upload.maximum_data_size=2560
microcpu.upload.speed=57600

microcpu.bootloader.tool=avrdude
microcpu.bootloader.low_fuses=0xff
microcpu.bootloader.high_fuses=0xd8
microcpu.bootloader.extended_fuses=0xcb
microcpu.bootloader.file=caterina/Caterina-Micro.hex

microcpu.build.mcu=atmega32u4
microcpu.build.f_cpu=16000000L
microcpu.build.board=AVR_MICRO
microcpu.build.core=arduino
microcpu.build.variant=micro

microcpu.build.vid=0x2341
microcpu.build.pid=0x9001
```
# Driver
The driver can reamin the same even if the device is plugged into different ports or if another Arduino Micro is plugged in.
```#define ARDUINO_PORT "/dev/arduino_cpu"```