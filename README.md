# Wireless Motion Detector

###### Copyright © 2025 Elijah Day

### About

In short, the Wireless Motion Detector is a device made to observe motion at a
mount point, from which it may wirelessly report any motion to a server or
other device.

The main board uses Espressif Systems's ESP32-C3 for its MCU, largely due to its
Wi-Fi connectivity and thorough documentation.  Motion detection is done through
the use of an infrared LED and receiver pair.  (This means that the detection
has a somewhat short range.)

Development on this project was started without a clear application in mind.  It
was moreso done as an exercise in PCB and firmware design rather than a way to
solve any particular problem.  Though, you may be able to find some utility in
it.

One potential application might be something like home security, but note that
there are not any measures in place (yet) to deal with issues like power
failures or device tampering.

### Credits

* EasyEDA for their free PCB design software.
* Espressif Systems for ESP32-C3 documentation and the ESP-IDF, used for
developing firmware for their devices.

### License

All data, software, documentation, and other files in this project are
distributed under the MIT License.  See "LICENSE" (located in this directory)
for more information or visit [www.mit-license.org](https://mit-license.org/).
