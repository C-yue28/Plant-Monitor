import lightgbm as lgb
import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.metrics import mean_absolute_error

data_features = ["soil", "temp", "humidity", "moisture_diff_1hr", "moisture_diff_3hr", "moisture_diff_6hr", "moisture_mean_6", "moisture_std_6", "moisture_rate"]
df = pd.read_csv("plant_monitor_data.csv")

X = df[data_features]
Y = df["time_until_dry"]

X_train, X_valid, Y_train, Y_valid = train_test_split(X, Y, shuffle=False, test_size=0.2)

model = lgb.LGBMRegressor(n_estimators=350, learning_rate=0.05, num_leaves=15)
model.fit(X_train, Y_train)

predictions = model.predict(X_valid)
print("ERROR: " + mean_absolute_error(Y_valid, predictions))

model.booster_.save_model("plant_monitor_ml_model.txt")

