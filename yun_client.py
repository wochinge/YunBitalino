import sys
import time
import requests

sys.path.insert(0, '/usr/lib/python2.7/bridge')
from bridgeclient import BridgeClient as bridgeclient

UUID = ''
USER_ACCESS_TOKEN = ''
AUTH = {'access_token': USER_ACCESS_TOKEN}

HOST = ''
ENDPOINT = HOST + '/v1/users/' + UUID + 'samples'


bridge = bridgeclient()
data = {'auth': AUTH}

start = time.time()

def main():
    # Provide a default  value
    bridge.put('bpm', '')
    bridge.put('respiration', '')

    while True:
        # Check for new value from the arduino
        bpm = check_bridge_for('bpm')
        respiration = check_bridge_for('respiration')
        post(bpm, respiration)


def check_bridge_for(key):
    val = bridge.get(key)
    if val:
        bridge.put(key, '')
        return val
    return None


def post(bpm_value, respiration_value):
    sample_data = {}
    if bpm_value:
        sample_data['heartbeat'] = [create_post_data(bpm_value)]
    if respiration_value:
        sample_data['respiration'] = [create_post_data(respiration_value)]

    if sample_data:
        data['samples'] = sample_data
        requests.post(ENDPOINT, json=data)
        del data['samples']


def create_post_data(value):
    return {'recorded_at': time.time(), 'value': value}


if __name__ == "__main__":
    main()
