# -*- coding: utf-8 -*-

import asyncio
import datetime
from importlib.resources import path


from bleak import BleakClient
from bleak import BleakScanner

import matplotlib.pyplot as plt
import pandas as pd

import streamlit as st

# These values have been randomly generated - they must match between the Central and Peripheral devices
# Any changes you make here must be suitably made in the Arduino program as well

Ultrasound_UUID = '13012F01-F8C3-4F4A-A8F4-15CD926DA146'

distancebuffer = []
distancebuffer2 = []

distancecompare = []
distancecompare2 = []
distancelist = []

async def run():

    print('Arduino Nano BLE LED Peripheral Central Service')
    print('Looking for Arduino Nano 33 BLE Peripheral Device...')

    found = False
    devices = await BleakScanner.discover()
    #print(devices)
    for d in devices:       
        if '19:B7:0B:1A:B1:7C' in d.address:
            print('Found Arduino Nano 33 BLE Peripheral')
            print('waiting for connection...(estimated time: 30 seconds)')
            found = True
            async with BleakClient(d.address) as client:
                print(f'Connected to {d.address}')
                print('')

                val = await client.read_gatt_char(Ultrasound_UUID)
                val = int.from_bytes(val, byteorder='little')
                print(f'Initial value: {val}')
                
            
                while True:
                    await asyncio.sleep(0.5) 
                    data = await client.read_gatt_char(Ultrasound_UUID)
                    rawdata = int.from_bytes(data, byteorder='little')
                    datafilter(rawdata,7,False)
                    
    if not found:
        print('Could not find Arduino Nano 33 BLE Peripheral')

def datafilter(rawdata,filter_1_value = False,filter_2_value = False):
    rowdatatime = datetime.datetime.now().strftime("%H:%M:%S")
    distancecompare.append([rowdatatime,rawdata])
    if filter_1_value == False:
        #print('You are getting data without filtering')
        print(f'{rowdatatime}: {rawdata} cm')
        distancelist.append([rowdatatime,rawdata])
    elif 0 <= filter_1_value <= 2:
        #print ('The first filter value should be bigger than 2. You are getting data with a filter value of 3')
        filter_1_value = 3

        if filter_2_value != False:
            if 0 <= filter_2_value <= 2:
                filter_2_value = 3
                if len(distancebuffer) < filter_1_value:
                    distancebuffer.append(rawdata)
                                
                else:
                    distancebuffer.pop(0)
                    distancebuffer.append(rawdata)
                    distancebuffer.sort()
                    distancebuffer.pop(0)
                    distancebuffer.pop(-1)
                    filterdata = sum(distancebuffer)/len(distancebuffer)
                    timecompare2 = datetime.datetime.now().strftime("%H:%M:%S")
                    #print(f'{time}: {filterdata} cm')
                    distancecompare2.append([timecompare2,filterdata])
                                    
                    
                    if len(distancebuffer2) < filter_2_value:
                        distancebuffer2.append(filterdata)
                    else:
                        distancebuffer2.pop(0)
                        distancebuffer2.append(filterdata)
                        distancebuffer2.sort()
                        distancebuffer2.pop(0)
                        distancebuffer2.pop(-1)
                        finaldata = sum(distancebuffer2)/len(distancebuffer2)
                        time = datetime.datetime.now().strftime("%H:%M:%S")
                        print(f'{timecompare2}: {finaldata} cm')
                        distancelist.append([time,finaldata])  
            else:  
                if len(distancebuffer) < filter_1_value:
                    distancebuffer.append(rawdata)
                                
                else:
                    distancebuffer.pop(0)
                    distancebuffer.append(rawdata)
                    distancebuffer.sort()
                    distancebuffer.pop(0)
                    distancebuffer.pop(-1)
                    filterdata = sum(distancebuffer)/len(distancebuffer)
                    timecompare2 = datetime.datetime.now().strftime("%H:%M:%S")
                    #print(f'{time}: {filterdata} cm')
                    distancecompare2.append([timecompare2,filterdata])
                                    
                    
                    if len(distancebuffer2) < filter_2_value:
                        distancebuffer2.append(filterdata)
                    else:
                        distancebuffer2.pop(0)
                        distancebuffer2.append(filterdata)
                        distancebuffer2.sort()
                        distancebuffer2.pop(0)
                        distancebuffer2.pop(-1)
                        finaldata = sum(distancebuffer2)/len(distancebuffer2)
                        time = datetime.datetime.now().strftime("%H:%M:%S")
                        print(f'{timecompare2}: {finaldata} cm')
                        distancelist.append([time,finaldata])  
        elif filter_2_value == False:
            if len(distancebuffer) < filter_1_value:
                distancebuffer.append(rawdata)                      
            else:
                distancebuffer.pop(0)
                distancebuffer.append(rawdata)
                distancebuffer.sort()
                distancebuffer.pop(0)
                distancebuffer.pop(-1)
                filterdata = sum(distancebuffer)/len(distancebuffer)
                timecompare2 = datetime.datetime.now().strftime("%H:%M:%S")
                #print(f'{time}: {filterdata} cm')
                distancecompare2.append([timecompare2,filterdata])
                time = datetime.datetime.now().strftime("%H:%M:%S")
                print(f'{timecompare2}: {filterdata} cm')
                distancelist.append([time,filterdata])
    elif filter_1_value > 2:
        if filter_2_value != False:
            if 0 <= filter_2_value <= 2:
                filter_2_value = 3
                if len(distancebuffer) < filter_1_value:
                    distancebuffer.append(rawdata)
                                
                else:
                    distancebuffer.pop(0)
                    distancebuffer.append(rawdata)
                    distancebuffer.sort()
                    distancebuffer.pop(0)
                    distancebuffer.pop(-1)
                    filterdata = sum(distancebuffer)/len(distancebuffer)
                    timecompare2 = datetime.datetime.now().strftime("%H:%M:%S")
                    #print(f'{time}: {filterdata} cm')
                    distancecompare2.append([timecompare2,filterdata])
                                    
                    
                    if len(distancebuffer2) < filter_2_value:
                        distancebuffer2.append(filterdata)
                    else:
                        distancebuffer2.pop(0)
                        distancebuffer2.append(filterdata)
                        distancebuffer2.sort()
                        distancebuffer2.pop(0)
                        distancebuffer2.pop(-1)
                        finaldata = sum(distancebuffer2)/len(distancebuffer2)
                        time = datetime.datetime.now().strftime("%H:%M:%S")
                        print(f'{timecompare2}: {finaldata} cm')
                        distancelist.append([time,finaldata])  
            else:  
                if len(distancebuffer) < filter_1_value:
                    distancebuffer.append(rawdata)
                                
                else:
                    distancebuffer.pop(0)
                    distancebuffer.append(rawdata)
                    distancebuffer.sort()
                    distancebuffer.pop(0)
                    distancebuffer.pop(-1)
                    filterdata = sum(distancebuffer)/len(distancebuffer)
                    timecompare2 = datetime.datetime.now().strftime("%H:%M:%S")
                    #print(f'{time}: {filterdata} cm')
                    distancecompare2.append([timecompare2,filterdata])
                                    
                    
                    if len(distancebuffer2) < filter_2_value:
                        distancebuffer2.append(filterdata)
                    else:
                        distancebuffer2.pop(0)
                        distancebuffer2.append(filterdata)
                        distancebuffer2.sort()
                        distancebuffer2.pop(0)
                        distancebuffer2.pop(-1)
                        finaldata = sum(distancebuffer2)/len(distancebuffer2)
                        time = datetime.datetime.now().strftime("%H:%M:%S")
                        print(f'{timecompare2}: {finaldata} cm')
                        distancelist.append([time,finaldata])  
        else:
            if len(distancebuffer) < filter_1_value:
                distancebuffer.append(rawdata)
                            
            else:
                distancebuffer.pop(0)
                distancebuffer.append(rawdata)
                distancebuffer.sort()
                distancebuffer.pop(0)
                distancebuffer.pop(-1)
                filterdata = sum(distancebuffer)/len(distancebuffer)
                timecompare2 = datetime.datetime.now().strftime("%H:%M:%S")
                #print(f'{time}: {filterdata} cm')
                distancecompare2.append([timecompare2,filterdata])
                time = datetime.datetime.now().strftime("%H:%M:%S")
                print(f'{timecompare2}: {filterdata} cm')
                distancelist.append([time,filterdata])

loop = asyncio.get_event_loop()
try:
    loop.run_until_complete(run())
except KeyboardInterrupt:
    print('\nReceived Keyboard Interrupt')
finally:
    print('Program finished')
    #save data into a dataframe
    dfcompare = pd.DataFrame(distancecompare, columns = ['time', 'distance'])
    
    dfcompare2 = pd.DataFrame(distancecompare2, columns = ['time', 'distance'])
    
    df = pd.DataFrame(distancelist, columns = ['time', 'distance'])
    print(df)
    st.table(df)

    print(f"Standard deviation of rawdata: {df['distance'].std()}")
    print(f"Standard deviation of filtered data: {dfcompare2['distance'].std()}")
    print(f"Standard deviation of final data: {dfcompare['distance'].std()}")

    #save dataframe into a csv file in the same folder
    path = r'./'
    df.to_csv(path + '/distance.csv', index = False)
    
    #plot the dfcompare and dfcompare2 and final data
    fig, (ax1, ax2, ax3) = plt.subplots(3, 1)
    #adjust the space between subplots
    fig.subplots_adjust(hspace=1)
    
    ax1.plot(dfcompare['time'], dfcompare['distance'])
    ax1.xaxis.set_major_locator(plt.MaxNLocator(5))
    ax1.set_title('raw data')
    ax1.set_xlabel('time')
    ax1.set_ylabel('distance')

    ax2.plot(dfcompare2['time'], dfcompare2['distance'])
    ax2.xaxis.set_major_locator(plt.MaxNLocator(5))
    ax2.set_title('filtered data')
    ax2.set_xlabel('time')
    ax2.set_ylabel('distance')

    ax3.plot(df['time'], df['distance'])
    ax3.xaxis.set_major_locator(plt.MaxNLocator(5))
    ax3.set_title('final data')
    ax3.set_xlabel('time')
    ax3.set_ylabel('distance')

    plt.show()
    loop.close()

        
