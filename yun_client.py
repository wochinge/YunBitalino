import sys
import time
import requests

sys.path.insert(0, '/usr/lib/python2.7/bridge')
from bridgeclient import BridgeClient as bridgeclient

UUID = 'e9ef17b3-e045-4966-99dd-854c35933aa5'
USER_ACCESS_TOKEN = 'e0225677f6d675e18f9794f8932b67de'
AUTH = {'access_token': USER_ACCESS_TOKEN}

HOST = 'http://api.nightcare.fbrix.corvus.uberspace.de'
ENDPOINT = HOST + '/v1/users/' + UUID + '/samples'

TEN_MILLIS = 0.01

bridge = {}
bridge = bridgeclient()
data = {'auth': AUTH}

start = time.time()

def main():
    # Provide a default  value
    bridge.put('bpm', '')
    bridge.put('respiration', '')
    bridge.put('temp', '')


    while True:
        # Check for new value from the arduino
        bpm = check_bridge_for('bpm')
        time.sleep(TEN_MILLIS)
        respiration = check_bridge_for('respiration')
        time.sleep(TEN_MILLIS)
        temp = check_bridge_for('temp')
        post(bpm, respiration, temp)


def check_bridge_for(key):
    val = bridge.get(key)
    if val:
        bridge.put(key, '')
        return float(val)
    return None


def post(bpm_value, respiration_value, temperature):
    sample_data = {}
    if bpm_value:
        sample_data['heartbeat'] = [create_post_data(bpm_value)]
    if respiration_value:
        sample_data['respiration'] = [create_post_data(respiration_value)]
    if temperature:
        sample_data['temperature'] = [create_post_data(temperature)]
    if sample_data:
        data['samples'] = sample_data
        try:
            response = requests.post(ENDPOINT, json=data)
            bridge.put('response_status', '{}'.format(response.status_code))
        except:
            bridge.put('response_status', '0')
        del data['samples']


def create_post_data(value):
    return {'recorded_at': '{}'.format(time.time()), 'value': value}


if __name__ == "__main__":
    main()
