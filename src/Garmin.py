import datetime
import os
from pathlib import Path
from typing import Any, Dict, Optional

from garminconnect import Garmin

token_store = "~/.garminconnect"


def initialize_garmin(
    email: Optional[str] = None, password: Optional[str] = None
) -> Garmin:
    """Log in with cached Garmin tokens, falling back to supplied or environment credentials."""
    tokenstore_path = str(Path(token_store).expanduser())

    try:
        garmin = Garmin()
        garmin.login(tokenstore_path)
        return garmin
    except Exception as token_error:
        email = email or os.getenv("GARMIN_EMAIL")
        password = password or os.getenv("GARMIN_PASSWORD")
        if not email or not password:
            raise RuntimeError(
                "Garmin token login failed and credentials are not configured"
            ) from token_error

        try:
            garmin = Garmin(email=email, password=password)
            garmin.login(tokenstore_path)
            return garmin
        except Exception as error:
            raise RuntimeError(f"Garmin authentication failed: {error}") from error


def fetch_all_data(
    email: Optional[str] = None, password: Optional[str] = None
) -> Dict[str, Any]:
    results: Dict[str, Any] = {
        "status": "failed",
        "daily_summary": {},
        "sleep_data": {},
        "heart_rate": {},
        "body_battery": {},
        "stress_level": -1,
        "errors": {},
    }

    try:
        api = initialize_garmin(email, password)
    except Exception as error:
        results["errors"]["authentication"] = str(error)
        return results

    today = datetime.date.today().isoformat()
    fetchers = {
        "daily_summary": lambda: api.get_stats(today),
        "sleep_data": lambda: api.get_sleep_data(today),
        "body_battery": lambda: api.get_body_battery(today),
        "stress_level": lambda: api.get_stress_data(today),
    }

    for key, fetch in fetchers.items():
        try:
            data = fetch()
            if key == "daily_summary":
                summary = data or {}
                results[key] = summary
                results["heart_rate"] = {
                    "resting": summary.get("restingHeartRate", 0),
                    "min": summary.get("minHeartRate", 0),
                    "max": summary.get("maxHeartRate", 0),
                }
            elif key == "stress_level":
                if data:
                    results[key] = data.get("avgStressLevel", -1)
            elif data:
                results[key] = data
        except Exception as error:
            results["errors"][key] = str(error)

    if not results["errors"]:
        results["status"] = "success"
    elif len(results["errors"]) < len(fetchers):
        results["status"] = "partial"

    return results
