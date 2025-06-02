# SmartEnergyTable Improvement S4 - 2025
Embedded Systems Team on the Smart Energy Table Project S4

# Requirements
- Mosquitto Broker
- Python 

# Setup

When the board is powered, an access point will come up as Pepper_C1-MUX-[ID].
Connect to it and configure a static IP address of 192.168.150.24.

To configure the Pepper C1 MUX, go into your web browser and type 192.168.150.1 into the search bar. The username and password is given in the documentation.



# How to run
1. Turn on the board and connect to the Pepper
2. Make sure the IP address is set in the Pepper web server
3. Run the MQTT Broker
4. Run the Python program
5. Make sure every device is connected in the MQTT broker

---
If you want to have internet connection while connected to the Pepper, you'd have to have an extra Wi-Fi dongle connected to eduroam.
