Project Summary

An IoT-based Smart Tank and Thermal Management System built using Arduino and the Blynk IoT platform. The system provides real-time monitoring and automated control of water levels and temperature. It features an automated safety logic to prevent tank overflow or dry running and maintains water temperature within a safe 10°C–35°C range.
Specifications
Controller: Arduino with Ethernet Shield (W5100/W5500).
Sensors: LM35 Temperature Sensor, Analog Water Level Sensor.
Actuators: Heater, Cooler, Water Inlet Valve, Water Outlet Valve (via Serial/Digital Pins).
Display: 16x2 I2C LCD for local status monitoring.
IoT Platform: Blynk IoT (Dashboard includes Gauges, Buttons, and Terminal for logs).
Key Features:
Auto-Fill: Triggers at <50 units; Stops at >3000 units.
Thermal Safety: Hard-coded cut-offs at 35°C (High) and 10°C (Low).
Mutual Exclusion: Prevents Heater and Cooler from running simultaneously.

Blynk Datastream Configuration

To use this code, create a New Template in the Blynk Console and add the following Datastreams:
Virtual Pin	Name	Data Type	Min	Max	Description
V1	Temperature	Double	0	100	Real-time temp monitoring
V2	Cooler Button	Integer	0	1	Manual Cooler Control
V3	Heater Button	Integer	0	1	Manual Heater Control
V5	Use Button	Integer	0	1	Water Outlet Valve Control
V6	Fill Button	Integer	0	1	Water Inlet Valve Control
V7	Terminal	String	-	-	System logs & safety alerts
V8	Tank Level	Integer	0	3000	Volume/Level monitoring

Widget Setup Guide

After creating the datastreams, add these widgets to your Web Dashboard or Mobile App:
Gauges: Link to V1 (Temp) and V8 (Level).
Switches: Create 4 buttons linked to V2, V3, V5, and V6. Set them to Push or Switch mode.
Terminal: Link to V7 to receive the "High/Low" and "Critical" notifications.
