from flask import Flask, request, jsonify
import paho.mqtt.client as mqtt
import threading

app = Flask(__name__)

mqtt_broker = "broker.mqtt.cool"
mqtt_port = 1883
mqtt_topic = "temperatureTopic"

mqtt_client = mqtt.Client()

@app.route('/api/temperature', methods=['POST'])
def temperature():
    if request.is_json:
        data = request.get_json()
        temperature = data.get('temperature')

        if temperature is not None:
            response = {
                "status": "success",
                "message": f"Temperature received: {temperature}°C"
            }
            return jsonify(response), 200
        else:
            response = {
                "status": "fail",
                "message": "Temperature data missing"
            }
            return jsonify(response), 400
    else:
        response = {
            "status": "fail",
            "message": "Request body must be JSON"
        }
        return jsonify(response), 400

def on_connect(client, userdata, flags, rc):
    print(f"Connected to MQTT broker with result code {rc}")
    client.subscribe(mqtt_topic)

def on_message(client, userdata, msg):
    temperature = msg.payload.decode()
    print(f"Temperature received: {temperature}°C")

def run_mqtt_client():
    mqtt_client.on_connect = on_connect
    mqtt_client.on_message = on_message
    mqtt_client.connect(mqtt_broker, mqtt_port, 60)
    mqtt_client.loop_forever()

if __name__ == '__main__':
    mqtt_thread = threading.Thread(target=run_mqtt_client)
    mqtt_thread.start()
    app.run(debug=True, host='0.0.0.0', port=5000)