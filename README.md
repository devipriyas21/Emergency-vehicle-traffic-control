# Emergency-vehicle-traffic-control
An Arduino-based system that detects emergency vehicles using RFID and ultrasonic sensors to control traffic signals in real time.
## Project Overview

This project implements an emergency vehicle detection and real-time traffic signal coordination system using an Arduino Mega. The system is designed to prioritize emergency vehicles such as ambulances and fire trucks at traffic junctions to reduce delays and improve response time.

RFID technology is used to identify authorized emergency vehicles. Each emergency vehicle carries a unique RFID tag, which is detected by an RFID reader installed at the junction. To ensure accurate detection, HC-SR04 ultrasonic sensors are used to detect the physical presence of a vehicle in each lane.

When both the RFID tag and vehicle presence are detected, the system overrides the normal traffic signal cycle and turns the corresponding lane green, allowing the emergency vehicle to pass safely. After a predefined duration, the system resumes its normal traffic light sequence.

This project demonstrates the use of embedded systems, sensor integration, and intelligent traffic control concepts for smart city applications.
