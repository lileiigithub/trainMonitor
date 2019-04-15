import numpy as np
import matplotlib.pyplot as plt

def get_data(_file_path):
    data_list = []
    with open(_file_path) as file:
        for line in file.readlines():
            line = line.strip()
            temperature = line.split(",")[0]
            humidity = line.split(",")[1]
            PM10 = line.split(",")[2]
            CO2 = line.split(",")[3]
            data_list.append([temperature,humidity,PM10,CO2])
    return np.array(data_list)


if __name__=='__main__':
    FilePath = "data.txt"
    data_array = get_data(FilePath)
    print("data shape:",data_array.shape)
    
    