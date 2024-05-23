import pandas as pd
import matplotlib.pyplot as plt

dirName = "./data/"
fileName = "big_boy_seeded(50000).json"

# Read the data
df = pd.read_json(dirName + fileName)
print(df.head())