'''
	Raspberry Pi GPIO Status and Control
'''
#import RPi.GPIO as GPIO
import flask
from flask import Flask, render_template, request
from greenhouse import Greenhouse
import asyncio
import threading


app = Flask(__name__)
g = Greenhouse()


# define actuators GPIOs
state=0 #0 automatic / 1 manual
led = 22
fan = 24
pump = 23
#sensors
temperature = 20
humidity = 21
waterLevel = 25

# initialize GPIO status variables
ledSts = g.getLedSts()
fanSts = g.getFanSts()
pumpSts = g.getPumpSts()
temperatureSts = g.getTempSts()
humiditySts = g.getHumidSts()
waterLevelSts = g.getWaterSts()


@app.route("/")
def index():
    # Read Sensors Status
    # Read components state

    templateData = {
        'title': 'Greenhouse status',
        'led': g.getLedSts(),
        'fan': g.getFanSts(),
        'pump': g.getPumpSts(),
        'temperature': g.getTempSts(),
        'humidity': g.getHumidSts(),
        'waterLevel': g.getWaterSts(),
    }
    return render_template('index.html', **templateData)


@app.route('/get_led_status')
def led_status():
  current_status = flask.request.args.get('LEDstatus')
  #print(current_status)
  return 'ON' if current_status == 'OFF' else 'OFF'

@app.route('/get_fan_status')
def fan_status():
  current_status = flask.request.args.get('FANstatus')
  #print(current_status)
  return 'ON' if current_status == 'OFF' else 'OFF'

@app.route('/get_pump_status')
def pump_status():
  current_status = flask.request.args.get('PUMPstatus')
  #print(current_status)
  return 'ON' if current_status == 'OFF' else 'OFF'

@app.route("/<deviceName>/<action>")
def action(deviceName, action):
    if deviceName == 'led':
        actuator = led
    if deviceName == 'pump':
        actuator = pump
    if deviceName == 'fan':
        actuator = fan

    if action == "on":
        if actuator == led:
            g.setLedSts(1)
        if actuator == fan:
            g.setFanSts(1)
        if actuator == pump:
            g.setPumpSts(1)
        print(actuator, " ", action)

    if action == "off":
        if actuator == led:
            g.setLedSts(0)
        if actuator == fan:
            g.setFanSts(0)
        if actuator == pump:
            g.setPumpSts(0)

        print(actuator, " ", action)

    template_data = {
        'title': 'Greenhouse status',
        'led': ledSts,
        'fan': fanSts,
        'pump': pumpSts,
        'temperature': temperatureSts,
        'humidity': humiditySts,
        'waterLevel': waterLevelSts,
    }

    return render_template('index.html', **template_data)

def controlLED():
    ledSts = g.getLedSts()
    if ledSts == 0:
        g.setLedSts(1)
    else:
        g.setLedSts(0)


def main():
    return 0
def flaskThread():
    app.run(host='0.0.0.0', port=80, debug=True, use_reloader=False)

if __name__ == "__main__":
    threading.Thread(target=flaskThread).start()
    #g.run()





