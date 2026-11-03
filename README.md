# personal-api
a project that will group data from various streams and group them in a unified dashboard for the raspberry pi 3b.

# Garmin integration
The CMake build requires Python development files for the embedded interpreter. On Fedora, install them with `sudo dnf install python3-devel`. Install `garminconnect` in that same Python environment. Garmin token login is attempted first; if unavailable, set `GARMIN_EMAIL` and `GARMIN_PASSWORD` in the environment.

# it will include 
this project will gather data from garmin connect with a python script, from the kobo ereader where there is a database hidden inside the hidden folder .kobo/koboreader.sqlite file where the device already precompiles the data so all this program has to do is fetch the file, read it and then sync the NEW data in the corresponbing table, it will also expose an api that will collect data from a worker node being a raspberry pi zero 2w that will recieve and send data from a rowing machine. 

## this project will be on hold for a while
i am wanting to move onto another project and so i am going to put this project on hold but i am going to continue developement later on.

## to build
in order to build this project you are going to need the rust tech stack installed so that it can build the slint UI module making the GUI interface of the app, you are also going to need the python 3.14 interpreter so that the python script can run. if you just want to try the built binary(which should work with only the file, you are going to need to run it in a terminal so that you can input your credentials to fetch the garmin data). this project will for now, just fetch the garmin data and display it. make sure you have the `pip install garminconnect` package `garminconnect` so that the executable can fetch the data from garmin.

## further developement
this project is something i want to build but am not ready to continue on this journey to build yet, his project as mentioned above is supposed to include syncing of a kobo ereader, data sotring and handling (the reason there is a function to build and setup a database in the main.cpp file), as well as include some "smart features" that i am willing to include but not yet. for this reason, these featues are going to be included later on with full support of data logging of the garmin and hopefully the next feature being the kobo ereader sync features being included.
