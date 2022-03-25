import paho.mqtt.client as mqtt
import json
import requests
import threading

nest_flask_url = "http://nest-flask:5000"
delay_door_open_in_seconds = 60
turn_on_thread = None

def turn_eco_on():
    requests.post(nest_flask_url + "/eco_on")
    print("Eco is now on")

def turn_eco_off():
    requests.post(nest_flask_url + "/eco_off")
    print("Eco is now off")

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    global turn_on_thread
    payload = json.loads(msg.payload)

    if (payload.get("contact", True)):
        if (turn_on_thread is not None):
            turn_on_thread.cancel()
        
        turn_on_thread = None
        turn_eco_off()
    else:
        turn_on_thread = threading.Timer(delay_door_open_in_seconds, turn_eco_on)
        turn_on_thread.start()

client = mqtt.Client()
client.on_message = on_message

client.connect("mqtt", 1883, 60)
client.subscribe("zigbee2mqtt/porta-varanda")

# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.
client.loop_forever()
