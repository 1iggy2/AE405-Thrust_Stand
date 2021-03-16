# AE405 Thrust Stand
![Image](http://clasp-research.engin.umich.edu/groups/s3fl/images/aero.jpg)
Aero 405 Thrust Stand Project

*By Cameron Gable (1iggy2), Anthony Russo, Hogan Hsu, Sergio Ramirez Sabogal, Brian Sandor*

This project is aimed towards providing a thrust testing stand for brushless electric motors
and propellers used by student project teams as part of Professor George Halow’s AERO 495
course. Prof. Halow has indicated that there is currently a need for some of the project teams that
are part of his course to have an easy way to test the thrust capabilities of their electric motors and
propellers for applications ranging from quadcopters to fixed wing aircraft. Our project concept is
therefore a proposal to design, build, and test a modular thrust test stand that allows for multiple
motor and propeller sizes to be tested safely and effectively by any of the teams participating in
AERO 495. This test stand will use sensors to measure thrust, torque, voltage, current, and RPM
and involve a processing system for automated data collection for simple export to software like
Excel or MatLab for analysis.

# Base Station Logic

The Base Station is the measurement and data acquistion hub of this project. Tests will be run based on the Menu ID sent from the remote. Raw sensor output, calibration, and settings will be accessed via a physical button press and the serial monitor. This is due to RAM limitations in the Arduino Nano powering the Remote. A full flowchart of the logic is provided below.

<p align="center">
  <img width="686" height="1211" src="https://github.com/1iggy2/AE405-Thrust_Stand/blob/main/Flowcharts/BaseStationLogic.png?raw=true">
</p>

# Remote GUI Explaination

After initial testing of the remote GUI intiailly proposed it was apparent that the Arduino Nano did not have enough RAM to display the fully fleshed out GUI. In order to combat this we have designed a low menu count GUI to optimize the remote. This menu uses a minimal amount of unique screens to lower the RAM usage. The image below is the design flowchart for the optimized remote.

<p align="center">
  <img width="686" height="1211" src="https://github.com/1iggy2/AE405-Thrust_Stand/blob/main/Flowcharts/Low_RAM_GUI_Flowchart.png?raw=true">
</p>
