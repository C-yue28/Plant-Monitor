import pandas as pd
from scipy.optimize import curve_fit

data = pd.read_csv("plant_monitor_data.csv")

x = data[["soil", "temp", "humidity"]]
y = data["time"]

# the relationships between the variables was determined after looking at a bunch of online sources and referencing ChatGPT for help
def model(X, K, a, b, c):
    S, T, H, P = X
    return K * (S**a) * (1+b*T) * (1-c*H)

parameters, pcov = curve_fit(model, x, y, p0=[0.05, 0.8, 0.1, 0.5]) # these values are kind of arbitrary, I'm just guessing what would impact it more
K, a, b, c = parameters

with open("plant_monitor_ml_model_params.txt") as file:
    file.write(str(K)+str(a)+str(b)+str(c)) 
