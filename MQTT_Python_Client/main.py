import paho.mqtt.client as mqtt
import json

def on_connect(client, userdata, flags, reason_code, properties):
    print(f"Connected with result code {reason_code}")
    client.subscribe("pepper/data")

def on_message(client, userdata, msg):
    try:
        # Decode payload to string
        payload_str = msg.payload.decode('utf-8')
        # print(f"Raw message: {payload_str}")

        # Parse the JSON string to a python directory
        data = json.loads(payload_str)

        # print(f"Parsed JSON: {data}")
        # the fields can be selected
        antenna_value = data.get("antenna")
        print(f"Antenna: {antenna_value}")

        # Forward it to the ESP32
        client.publish("pepper/forwarded", str(antenna_value))


    except json.JSONDecodeError as e:
        print(f"Failed to decode JSON: {e}")

mqttc = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
mqttc.on_connect = on_connect
mqttc.on_message = on_message
mqttc.username_pw_set(username="mqtt_user", password="smartenergytable")
mqttc.connect("127.0.0.1", 1883, 60)
mqttc.loop_forever()
