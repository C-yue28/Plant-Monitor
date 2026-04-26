import csv
import pandas as pd

# sensor data
# moisture difference in past 1 hr / 3 hr / 6 hr
# moisture rate change in past 6 hr
# time to the next watering

# each entry is 5 minutes

raw_data_headers = ["soil", "temp", "humidity", "has_watered", "time"]

class Data:
    def __init__(self):
        self.soil_moisture = 0.0
        self.temp = 0.0
        self.humidity = 0.0
        self.watered = False
        self.time = 0.0

    def __setitem__(self, index, value):
        if index == 0:
            self.soil_moisture = value
        elif index == 1:
            self.temp = value
        elif index == 2:
            self.humidity = value
        elif index == 3:
            self.watered = value > 0
        elif index == 4:
            self.time = value
        else:
            raise IndexError("Index of data out of range.")

    def to_raw_data(self):
        return [self.soil_moisture, self.temp, self.humidity, self.watered, self.time]

    def convert_to_data_entry(self):
        return {"soil": self.soil_moisture, "temp": self.temp, "humidity": self.humidity, }

def load_raw_data():
    rows = []

    with open("plant_monitor_data.csv", "r") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue

            parts = line.split(",")

            data = {}
            for i, p in enumerate(parts):
                data[raw_data_headers[i]] = float(p)

            rows.append(data)

    return pd.DataFrame(rows)

df = load_raw_data()
df.columns = raw_data_headers

# feature engineering
df["moisture_diff_1hr"] = df["soil"].diff(1)
df["moisture_diff_3hr"] = df["soil"].diff(3)
df["moisture_diff_6hr"] = df["soil"].diff(6)

df["moisture_mean_6"] = df["soil"].rolling(6).mean()
df["moisture_std_6"] = df["soil"].rolling(6).std()
df["moisture_rate"] = df["soil"].diff(6) / 6

current_watering_time = -1
for i in range(len(df)-1, -1, -1):
    if current_watering_time == -1 and not df["has_watered"][i]:
        df.drop(i)
    elif df["has_watered"][i]:
        current_watering_time = -1
        df["time_until_dry"][i] = 0
    else:
        df["time_until_dry"][i] = current_watering_time-df["time"][i]

df.drop(["has_watered", "time"])
final_df = df.dropna()
final_df.to_csv("plant_monitor_data.csv")