#include <iostream>
#include <filesystem>
#include <string>
#ifdef PERSONAL_API_USE_PYBIND11
#include <pybind11/embed.h>
#define PERSONAL_API_HAS_PYBIND11 1
#else
#define PERSONAL_API_HAS_PYBIND11 0
#endif

#include "../include/database.hpp"
#include "../include/api_server.hpp"
#include <appWindow.h>

#if PERSONAL_API_HAS_PYBIND11
namespace py = pybind11;
#endif

int getGarmin(AppWindow& ui);
int run_window();

int main() {

    std::cout << "Starting Smart Hub..." << std::endl;

    const std::filesystem::path databasePath{"storage/Personal.db"};
    std::error_code filesystemError;
    std::filesystem::create_directories(databasePath.parent_path(), filesystemError);
    if (filesystemError) {
      std::cerr << "Failed to create database directory: "
                << filesystemError.message() << '\n';
      return 1;
    }

    personalAPI::Database db(databasePath.string());

    if (!db.open()) {
      std::cerr << "Failed to initialize database connection: "
                << db.getLastError() << '\n';
      return 1;
    }

    if (!db.execute(
        "CREATE TABLE IF NOT EXISTS metrics ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "source TEXT NOT NULL, " // garmin rowing kobo
        "payload TEXT, "
        "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP);"
        )) {
      std::cerr << "Failed to create metrics table: "
                << db.getLastError() << '\n';
      return 1;
    }

    // list all tables managed by your api backend
    auto tables = db.listTables();
    std::cout << "active tables in local database: \n";
    for (const auto& table : tables) {
      std::cout << " - " << table << "\n";
    }

  #if PERSONAL_API_HAS_PYBIND11
    py::scoped_interpreter interpreter{};
  #endif

  return run_window();
}

int run_window() {
  auto ui = AppWindow::create();

  ui->set_db_status(slint::SharedString("Connected"));

  std::cout << "Starting initial Garmin data fetch.\n";
  getGarmin(*ui);

  const slint::ComponentWeakHandle<AppWindow> ui_weak = ui;
  ui->on_fetch_Garmin([ui_weak] {
    if (auto ui = ui_weak.lock()) {
      std::cout << "fetching Garmin data" << std::endl;
      (*ui)->set_garmin_status(slint::SharedString("Fetching Garmin data..."));
      getGarmin(**ui);
    }
  });

  ui->run();
  return 0;
}


int getGarmin(AppWindow& ui) {
#if !PERSONAL_API_HAS_PYBIND11
  std::cerr << "Garmin integration is unavailable: pybind11 headers were not found.\n";
  ui.set_garmin_status(slint::SharedString("Garmin integration unavailable"));
  return 1;
#else
  std::cout << "Fetching data" << std::endl;

  try {
    // Load Garmin.py from the source directory, regardless of the working directory.
    py::module_ sys = py::module_::import("sys");
    const auto scriptDirectory = std::filesystem::path(__FILE__).parent_path();
    sys.attr("path").attr("insert")(0, scriptDirectory.string());

    py::module_ garmin_mod = py::module_::import("Garmin");

    std::string my_email = "";
    std::string my_password = "";
    std::cout << "[C++] Calling Python Garmin script" << std::endl;

    py::dict data_package;
    std::string status;
    while (true) {
      data_package = garmin_mod.attr("fetch_all_data")(my_email, my_password)
                         .cast<py::dict>();
      status = data_package["status"].cast<std::string>();
      if (status == "success" || status == "partial") {
        if (status == "partial") {
          std::cerr << "[C++] Garmin returned partial data.\n";
        }
        break;
      }

      std::cerr << "[C++] Garmin retrieval status: " << status << '\n';
      ui.set_garmin_status(slint::SharedString(
          "Garmin login failed; enter credentials in the terminal"));
      std::cout << "Enter your email: ";
      if (!std::getline(std::cin, my_email)) {
        ui.set_garmin_status(slint::SharedString("Garmin fetch cancelled"));
        return 1;
      }
      std::cout << "Enter your password: ";
      if (!std::getline(std::cin, my_password)) {
        ui.set_garmin_status(slint::SharedString("Garmin fetch cancelled"));
        return 1;
      }
    }

    std::cout << "\n[C++] Successfully received Garmin package\n";

    if (data_package.contains("stress_level") &&
        !data_package["stress_level"].is_none()) {
      const int avg_stress = data_package["stress_level"].cast<int>();
      std::cout << "Average Stress Level: " << avg_stress << '\n';
      ui.set_avgStress(avg_stress);
    }

    if (data_package.contains("heart_rate") &&
        py::isinstance<py::dict>(data_package["heart_rate"])) {
      py::dict hr_data = data_package["heart_rate"].cast<py::dict>();
      if (hr_data.contains("resting")) {
        const int resting = hr_data["resting"].cast<int>();
        std::cout << "Resting Heart Rate: "
                  << resting << " bpm\n";
        ui.set_rHR(resting);
      }
      if (hr_data.contains("max")) {
        const int maximum = hr_data["max"].cast<int>();
        std::cout << "Maximum Heart Rate: "
                  << maximum << " bpm\n";
        ui.set_mHR(maximum);
      }
    }

    if (data_package.contains("body_battery")) {
      py::object body_battery = data_package["body_battery"];
      py::object battery_value = py::none();
      if (py::isinstance<py::dict>(body_battery)) {
        py::dict battery_data = body_battery.cast<py::dict>();
        if (battery_data.contains("bodyBatteryValue")) {
          battery_value = battery_data["bodyBatteryValue"];
        }
      } else if (py::isinstance<py::list>(body_battery)) {
        for (const py::handle item : body_battery.cast<py::list>()) {
          if (py::isinstance<py::dict>(item)) {
            py::dict battery_data = py::reinterpret_borrow<py::dict>(item);
            if (battery_data.contains("bodyBatteryValue")) {
              battery_value = battery_data["bodyBatteryValue"];
              break;
            }
          }
        }
      }
      if (!battery_value.is_none()) {
        const int body_battery_value = battery_value.cast<int>();
        std::cout << "Current Body Battery: " << body_battery_value << '\n';
        ui.set_bodyBattery(body_battery_value);
      }
    }

    ui.set_garmin_status(slint::SharedString(
        status == "partial" ? "Garmin data loaded with some errors"
                            : "Garmin data loaded"));
  } catch (const py::error_already_set& e) {
    std::cerr << "[C++] Python Exception caught: " << e.what() << std::endl;
    ui.set_garmin_status(slint::SharedString("Garmin fetch failed; see terminal"));
    return 1;
  }
  return 0;
#endif
}
