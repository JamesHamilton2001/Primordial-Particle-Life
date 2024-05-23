import pandas as pd
import matplotlib.pyplot as plt
import json

dirName = "./data/"
fileName = "debug_seeded(10000).json"

data = pd.read_json(dirName + fileName)
first_attribute = data.columns[0]
print(first_attribute)
