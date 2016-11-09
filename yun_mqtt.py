import sys
import paho.mqtt.client as mqtt
sys.path.insert(0, '/usr/lib/python2.7/bridge')
from bridgeclient import BridgeClient as bridgeclient

bridge = bridgeclient()
client = mqtt.Client()


def on_connect(client, userdata, flags, rc):
    client.publish("maph/info", "connected")


def main():
    # Provide a default  value
    bridge.put("bpm", "")
    # Connect mqtt client
    client.on_connect = on_connect
    client.connect("test.mosca.io", 1883, 60)
    client.loop_start()
    while True:
        # Check for new value from the arduino
        bpm = bridge.get("bpm")
        if bpm:
            client.publish("maph/bpm", bpm)
            # Set the the store as empty so we can recognize new values from the arduino
            bridge.put("bpm", "")

if __name__ == "__main__":
    main()
