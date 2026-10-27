from fastapi import FastAPI
from datetime import date, datetime
from pydantic import BaseModel
import sqlite3


# build the api for garmin data returns
app = FastAPI()

class GarminMetrics(BaseModel):
    day: date
    id: int
    sleep_durations: int
    stress_levels: int
    body_battery: int
    steps: int


@app.get("/")
def read_root():
    return {"hello": "world"}

@app.post("/items/{item_id}")
def create_or_read_item(item_id: int, item: GarminMetrics):
    return{
        "item_str": item.name,
        "item_id": item_id, 
        "sleep": item.sleep_durations, 
        "stress": item.stress_levels, 
        "bodyBattery": item.body_battery,
        "steps": item.steps
    }

@app.put("/items/{item_id}")
def update_item(item_id: int, item: GarminMetrics):
    return {
        "item_str": item.name, 
        "item_id": item_id, 
        "sleep": item.sleep_durations,
        "stess": item.stress_levels,
        "bodyBattery": item.body_battery,
        "steps": item.steps
    }


# connect to the database or create it if it doesn't exist
def connect():
    connection = sqlite3.connect("personal.db")
    connection.close()

print("starting api")
connect()
