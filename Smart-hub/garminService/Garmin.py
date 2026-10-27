import datetime
import json
import os
from pathlib import Path
from garminconnect import Garmin

email = input("Input your email address for Garmin: ")
password = input("Input your Garmin password: ")
token_store = "~/.garminconnect"  # Directory to cache tokens

def initialize_garmin():
    """Initializes the Garmin client, utilizing stored tokens if available."""
    tokenstore_path = str(Path(token_store).expanduser())
    
    # Try loading previously stored session tokens first
    try:
        print(f"Attempting to login using stored tokens from: {token_store}")
        garmin = Garmin()
        garmin.login(tokenstore_path)
        print("Successfully logged in using stored tokens!")
        return garmin
    except Exception as e:
        print(f"Token login failed ({e}). Logging in with credentials...")
        try:
            # Login with credentials and pass tokenstore_path so it saves automatically
            garmin = Garmin(email=email, password=password)
            garmin.login(tokenstore_path)
            print(f"Successfully logged in and saved tokens to {token_store}")
            return garmin
        except Exception as ex:
            print(f"Authentication failed: {ex}")
            exit(1)

def main():
    # Init API client
    api = initialize_garmin()

    # Get today's date in YYYY-MM-DD
    today = datetime.date.today().isoformat()
    print(f"\nFetching health stats for today: {today}\n" + "="*40)

    try:
        # General daily stats (steps, calories, distance, floors, etc.)
        print("--- Daily Summary ---")
        stats = api.get_stats(today)
        print(json.dumps(stats, indent=2))

        # Heart rate data
        print("\n--- Heart Rate Data ---")
        hr_data = api.get_heart_rates(today)
        print(f"Resting Heart rate: {stats.get('restingHeartRate')} bpm")
        print(f"Min Heart rate: {stats.get('minHeartRate')} bpm")
        print(f"Max Heart Rate: {stats.get('maxHeartRate')} bpm")

        # Body Battery data
        print("\n--- Body Battery Data ---")
        try: 
            body_battery = api.get_body_battery(today)
            if body_battery:
                print("Body Battery latest/charged values retrieved successfully:")
                print(json.dumps(body_battery[-1], indent=2))
            else: 
                print("No Body Battery data found for today.")
        except Exception as e:
            print(f"Could not fetch body battery data: {e}")

        # Stress data
        print("\n--- Stress Data ---")
        try:
            stress_data = api.get_stress_data(today)
            print(f"Average stress level: {stress_data.get('avgStressLevel', 'N/A')}")
        except Exception as e:
            print(f"Could not fetch stress data: {e}")

    except Exception as err:
        print(f"Error retrieving health data: {err}")

if __name__ == "__main__":
    main()