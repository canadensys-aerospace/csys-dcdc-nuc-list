# List DCDC-NUC devices

This program lists all DCDC-NUC (http://www.mini-box.com/DCDC-NUC) devices currently connected to the system in the following format:

```
<bus>,<device>,<type>
bus    - bus number (libusb struct usb_bus.location)
device - device (libusb struct usb_device.filename)
type   - 0: startup pulse disabled, 1: startup pulse enabled
```

This program is used to determine the bus and dev values required by the [DCDC NUC ROS driver](https://github.com/canadensys-aerospace/csys-ros-dcdc-nuc)

## Compiling

```bash
$ make
```

## Installing

```bash
$ sudo make install
```

## Running

```bash
$ sudo dcdc_nuc_list
```
