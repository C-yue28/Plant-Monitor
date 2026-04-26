import serial
import keyboard
import sys
import csv

from plant_monitor_data_augmentation import Data

def log_data_and_end():
    global entries
    has_been_created = True
    try:
        test_file = open("plant_monitor_data.csv", "r+")
        test_file.close()
    except:
        has_been_created = False

    with open("plant_monitor_data.csv", "a+") as data_table:
        writer = csv.writer(data_table)
        writer.writerows(entries)

    sys.exit()

SERIAL = serial.Serial(port='COM3', baudrate=115200)
keyboard.add_hotkey('q', log_data_and_end)

entries = []

while True:
    new_entry = Data()
    for i in range(5):
        serial_data = SERIAL.readline().decode("utf-8").strip()
        new_entry[i] = float(serial_data[(serial_data.find(":")+2):])
    new_entry.time /= 3600000
    entries.append(new_entry.to_raw_data())

SERIAL.close()