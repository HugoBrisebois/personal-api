from fastapi import FastAPI
import sqlite3
import requests

app = FastAPI()


# connect to the database or create it if it doesn't exist
def connect():
    connection = sqlite3.connect("personal.db")
    connection.close()

@app.get("/")
def read_root():
    return {"hello" : "world"}


@app.get("/items/{item_id}")
def read_item(item_id: int, q: str | None = None):
    return{"item_id": item_id, "q":q }


print("starting api")
connect()
