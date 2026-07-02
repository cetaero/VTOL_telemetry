# VTOL Telemetry

## Overview
VTOL Telemetry allows establishing the link between the VTOL aircraft and the Ground Control Station that ensures its reliable work. The main objective of this subsystem is to develop a highly reliable telemetry communication system based on embedded systems, LoRa technology, and custom-designed antennas.

## Objectives
- Development of a reliable telemetry communication system for VTOL UAV.
- Implementation of long-range wireless communication through LoRa.
- Implementation of embedded firmware for telemetry communication.
- Development of custom antennas that provide better communication.

## Key Components
- ESP32-WROOM-32E
- SX1262 LoRa Transceiver
- Sleeve Dipole Antenna (air side)
- Bi-Quad Antenna with reflector (ground station)
- Power regulation and protection circuitry

## Technologies Used
- ESP-IDF
- Embedded C
- ESP32
- LoRa
- MAVLink

## Future Work
- Full testing and validation of the telemetry system.
- Range optimization and testing of the telemetry through flights.
- Integration with GCS.
- Future work for dual-band communication and more telemetry capabilities.
