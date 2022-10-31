import bluetooth
import json
from influxdb import InfluxDBClient
import datetime

sensor_address = '19:B7:0B:1A:B1:7C'
socket = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
socket.connect((sensor_address, 1))

influx = InfluxDBClient('localhost', 8086, 'root', 'root', 'example')
influx.create_database('example')

def log_stats(client, sensor_address, stats):
    json_body = [
        {
            "measurement": "sensor_data",
            "tags": {
                "client": sensor_address,
            },
            "time": datetime.datetime.now(datetime.timezone.utc).isoformat(),
            "fields": {
                "distance": stats['distance'],
            }
        }
    ]

    client.write_points(json_body)

buffer = ""

while True:
    data = socket.recv(1024).decode('utf-8')
    buffer += str(data, encoding='ascii')
    try:
        data = json.loads(buffer)
        print("Received chunk", data)
        log_stats(influx, sensor_address, data)
        buffer = ""
    except json.JSONDecodeError as e:
        continue

socket.close()