import json
import requests
from pymongo import MongoClient
import time


#GETTING DATA FROM YR.no
def get_yr_data():
    headers = {'User-Agent': 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_11_5) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/50.0.2661.102 Safari/537.36'}
    url = "https://api.met.no/weatherapi/locationforecast/2.0/compact.json?lat=59.3293&lon=18.0686"
    result = requests.get(url, headers=headers)
    responseJson = result.json()
    yr_data = responseJson['properties']['timeseries'][0]


    # temperature = responseJson['properties']['timeseries'][0]['data']['instant']['details']['air_temperature']

    return yr_data
def get_thingspeak():
    headers = {
        'User-Agent': 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_11_5) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/50.0.2661.102 Safari/537.36'}
    url = "THINGSPEAK URL"
    result = requests.get(url, headers=headers)
    responseJson = result.json()
    co2values = responseJson['feeds'][0]
    return co2values


#Connectiong to mongodDB
def get_database():
    # Provide the mongodb atlas url to connect python to mongodb using pymongo
    CONNECTION_STRING = "mongodb+srv://USERNAME:PASSWORD@clustero2.hqevo.mongodb.net/Co2?retryWrites=true&w=majority"

    # Create a connection using MongoClient. You can import MongoClient or use pymongo.MongoClient
    client = MongoClient(CONNECTION_STRING)
    return client['Co2']
def send_database():

    smhi_data = get_yr_data()

    dbname = get_database()
    collection_name = dbname["smhi_stockholm"]

    collection_name.insert_one(smhi_data)

    print(smhi_data)
    return smhi_data

hour = 60*60
def send_database_co2():
    thingspeak = get_thingspeak()
    dbname = get_database()
    collection_name = dbname["co2_values"]
    collection_name.insert_one(thingspeak)
    print(thingspeak)
    return thingspeak

while True:

    send_database()
    send_database_co2()
    time.sleep(hour)


