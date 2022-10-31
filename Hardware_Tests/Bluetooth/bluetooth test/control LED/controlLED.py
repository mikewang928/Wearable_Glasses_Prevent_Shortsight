# -*- coding: utf-8 -*-

import asyncio

from bleak import BleakClient
from bleak import BleakScanner

# These values have been randomly generated - they must match between the Central and Peripheral devices
# Any changes you make here must be suitably made in the Arduino program as well

LED_UUID = '13012F01-F8C3-4F4A-A8F4-15CD926DA146'


RED = False
GREEN = False
BLUE = False


async def setColor(client):
    global RED, GREEN, BLUE
    val = input('\nEnter rgb to toggle red, green and blue LEDs :')
    #print(val)
    
    if val == 'r':
        #RED = not RED 
        await client.write_gatt_char(LED_UUID, bytearray([1]), response=True)
        print('RED LED is now ON')
        status = await client.read_gatt_char(LED_UUID)
        #print(status)
        print('')
    if val == 'g':
        #GREEN = not GREEN
        await client.write_gatt_char(LED_UUID, bytearray([2]), response=True)
        print('GREEN LED is now ON')
        status = await client.read_gatt_char(LED_UUID)
        #print(status)
        print('')
    if val == 'b':
        #BLUE = not BLUE
        await client.write_gatt_char(LED_UUID, bytearray([3]), response=True)
        print('BLUE LED is now ON')
        status = await client.read_gatt_char(LED_UUID)
        #print(status)
        print('')
    if val == 'rg':
        #RED = not RED
        #GREEN = not GREEN
        await client.write_gatt_char(LED_UUID, bytearray([12]), response=True)
        print('RED and GREEN LED are now ON')
        status = await client.read_gatt_char(LED_UUID)
        #print(status)
        print('')
    if val == 'rb' or val == 'br':
        #RED = not RED
        #BLUE = not BLUE
        await client.write_gatt_char(LED_UUID, bytearray([13]), response=True)
        print('RED and BLUE LED are now ON')
        status = await client.read_gatt_char(LED_UUID)
        #print(status)
        print('')
    if val == 'gb' or val == 'bg':
        #GREEN = not GREEN
        #BLUE = not BLUE
        await client.write_gatt_char(LED_UUID, bytearray([23]), response=True)
        print('GREEN and BLUE LED are now ON')
        status = await client.read_gatt_char(LED_UUID)
        #print(status)
        print('')
    if val == 'rgb' or val == 'rbg' or val == 'grb' or val == 'gbr' or val == 'brg' or val == 'bgr':
        #RED = not RED
        #GREEN = not GREEN
        #BLUE = not BLUE
        await client.write_gatt_char(LED_UUID, bytearray([123]), response=True)
        print('All LEDs are now ON')
        status = await client.read_gatt_char(LED_UUID)
        #print(status)
        print('')
    if val == '0':
        await client.write_gatt_char(LED_UUID, bytearray([0]), response=True)
        print('All LEDs are now OFF')
        status = await client.read_gatt_char(LED_UUID)
        #print(status)
        print('')
    if val == 'q':
        print('Exiting...')
        exit()

async def run():
    global RED, GREEN, BLUE

    print('Arduino Nano BLE LED Peripheral Central Service')
    print('Looking for Arduino Nano 33 BLE Peripheral Device...')

    found = False
    devices = await BleakScanner.discover()
    #print(devices)
    for d in devices:       
        if '19:B7:0B:1A:B1:7C' in d.address:
            print('Found Arduino Nano 33 BLE Peripheral')
            found = True
            async with BleakClient(d.address) as client:
                print(f'Connected to {d.address}')
                print('')

                val = await client.read_gatt_char(LED_UUID)
                val = int.from_bytes(val, byteorder='little')
                print(f'Initial value: {val}')
                
                if val == 1:
                    print ('RED ON\nGREEN OFF\nBLUE OFF')
                    RED = True
                    GREEN = False
                    BLUE = False
                if val == 2:
                    print ('RED OFF\nGREEN ON\nBLUE OFF')
                    RED = False
                    GREEN = True
                    BLUE = False
                if val == 3:
                    print ('RED OFF\nGREEN OFF\nBLUE ON')
                    RED = False
                    GREEN = False
                    BLUE = True
                if val == 12:
                    print ('RED ON\nGREEN ON\nBLUE OFF')
                    RED = True
                    GREEN = True
                    BLUE = False
                if val == 13:
                    print ('RED ON\nGREEN OFF\nBLUE ON')
                    RED = True
                    GREEN = False
                    BLUE = True
                if val == 23:
                    print ('RED OFF\nGREEN ON\nBLUE ON')
                    RED = False
                    GREEN = True
                    BLUE = True
                if val == 123:
                    print ('RED ON\nGREEN ON\nBLUE ON')
                    RED = True
                    GREEN = True
                    BLUE = True
                if val == 0:
                    print ('RED OFF\nGREEN OFF\nBLUE OFF')
                    RED = False
                    GREEN = False
                    BLUE = False
            
                while True:
                    await setColor(client)

    if not found:
        print('Could not find Arduino Nano 33 BLE Peripheral')

                    
loop = asyncio.get_event_loop()
try:
    loop.run_until_complete(run())
except KeyboardInterrupt:
    print('\nReceived Keyboard Interrupt')
finally:
    print('Program finished')
        
