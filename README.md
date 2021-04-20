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

Table of Contents
=================

<!--ts-->
   * [Table of Contents](#table-of-contents)
   * [Instruction Manual](#instruction-manual)
   * [Construction Guide](#construction-guide)
      * [Wiring](#wiring)
        * [Remote](#remote)
        * [Base Station](#base-station)
      * [Thrust Stand](#thrust-stand)
   * [Design Justification](#design-justification)
      * [Code](#code)
        * [Base Station Logic](base-station-logic)
        * [Remote GUI Explaination](remote-gui-explaination)
      * [Physical Structure Analysis and Testing](physical-structure-analysis-and-testing)
        * [Thrust Stand Simulation](thrust-stand-simulation)
<!--te-->



## Instruction Manual

### Safety Considerations

Be careful when near spinning and/or powered propellers they can and will maim you. Please remove all propellers while working on the thrust stand. Propellers of all sizes are dangerous and deserve your respect. Please use the remote to remain clear of the spinning blades of death, and ensure you have a way to break the physical circuit from a distance readily availible. 
[![](http://img.youtube.com/vi/q4shbLB9U7Q/0.jpg)](http://www.youtube.com/watch?v=q4shbLB9U7Q "")

### How to Use the Remote



## Construction Guide

### Wiring

The wiring and wiring diagram were constructed using the website <https://Circuito.io>, an excellent tool for circuit design. A link to the Remote Circuito project can be found [here](https://www.circuito.io/static/reply/index.html?solutionId=6052555829fa400030165b44&solutionPath=storage.circuito.io) and the Base Station Circuito project [here](https://www.circuito.io/static/reply/index.html?solutionId=605298ebe53fd30030c5ce1c&solutionPath=storage.circuito.io). 
#### Remote


<p align="center">
  <img src="https://github.com/1iggy2/AE405-Thrust_Stand/blob/main/Images/RemoteCircuitoDiagram.PNG?raw=true">
</p>

| nRF24L01 | OLED  |
|:--------:|:-----:|
| ![](https://github.com/1iggy2/AE405-Thrust_Stand/blob/main/Images/nRF24L01%20Connections.PNG)    | ![](https://github.com/1iggy2/AE405-Thrust_Stand/blob/main/Images/AdafruitOLEDCircuitoDiagram.PNG) <br><br><br> *N.B. Not the same as the one code written for* |

#### Base Station


<p align="center">
  <img src="https://github.com/1iggy2/AE405-Thrust_Stand/blob/main/Images/BaseStationCircuitoDiagram.PNG?raw=true">
</p>

| BME280 | Hall Effect Sensor | Infrared Sensor | nRF24L01 | SD Card Shield |
|--------|--------------------|-----------------|----------|----------------|
| ![](https://github.com/1iggy2/AE405-Thrust_Stand/blob/main/Images/BME280Connections.PNG)  | ![](https://github.com/1iggy2/AE405-Thrust_Stand/blob/main/Images/HallConnections.PNG)              | ![](https://github.com/1iggy2/AE405-Thrust_Stand/blob/main/Images/IRConnections.PNG)           | ![](https://github.com/1iggy2/AE405-Thrust_Stand/blob/main/Images/nRF24L01%20Connections_MEGA.PNG)    | ![](https://github.com/1iggy2/AE405-Thrust_Stand/blob/main/Images/SDConnections.PNG)          |

*N.B. Not all components were availible on Circuito.io, additional components connections labeled in seperate diagram (Not yet created)

### Thrust Stand

Our thrust stand has been designed with modularity as the main objective. The interchangeable
vertical arm lengths allow for testing to occur for a variety of propeller and motor sizes. This also
allows for the thrust stand to operate in both large and small wind tunnels, making test scheduling more convenient for the users. Our motor mounting plates will allow for the mounting of
various motors with different bolt patterns. The various sizes of these plates will allow for testing
on small motors and large motors on the same stand. 


Long Arm Render             |  Short Arm Render
  :-------------------------:|:-------------------------:
![](https://github.com/1iggy2/AE405-Thrust_Stand/blob/main/Images/PH1R_L-Stand_Assembly_long.PNG)  |  ![](https://github.com/1iggy2/AE405-Thrust_Stand/blob/main/Images/PH1R_L-Stand_Assembly_short.PNG)

Our unique flexure torque measurement
device, seen below, allows for torque data to be collected from all ranges of motor sizes that fit on the thrust
testing stand. This innovation increases the effectiveness and accuracy of the data from other more
traditional torque measurement solutions. These system design choices increase the effectiveness
of our thrust stand and allow it to meet a wide variety of team’s thrust testing needs.
<p align="center">
  <img src="https://github.com/1iggy2/AE405-Thrust_Stand/blob/main/Images/FlexureTorqueMeasurement.PNG?raw=true">
</p>
Our design also features a pivot comprised of two pillow blocks to minimize any stiction, or static friction. A beautiful render of this pivot is shown below. A majority of the parts purchased for the structural construction came from McMaster Carr.

<p align="center">
  <img src="https://github.com/1iggy2/AE405-Thrust_Stand/blob/main/Images/PIVOT.png?raw=true">
</p>


## Design Justification

### Code

#### Base Station Logic

The Base Station is the measurement and data acquistion hub of this project. Tests will be run based on the Menu ID sent from the remote. Raw sensor output, calibration, and settings will be accessed via a physical button press and the serial monitor. This is due to RAM limitations in the Arduino Nano powering the Remote. A full flowchart of the logic is provided below.

<p align="center">
  <img src="https://github.com/1iggy2/AE405-Thrust_Stand/blob/main/Flowcharts/BaseStationLogic.png?raw=true">
</p>

#### Remote GUI Explaination

After initial testing of the remote GUI intiailly proposed it was apparent that the Arduino Nano did not have enough RAM to display the fully fleshed out GUI. In order to combat this we have designed a low menu count GUI to optimize the remote. This menu uses a minimal amount of unique screens to lower the RAM usage. The image below is the design flowchart for the optimized remote.

<p align="center">
  <img src="https://github.com/1iggy2/AE405-Thrust_Stand/blob/main/Flowcharts/Low_RAM_GUI_Flowchart.png?raw=true">
</p>

### Physical Structure Analysis

#### Thrust Stand Simulation

In order to model the thrust stand system in FEA many of the complex structures
were simplified. This included the flexure assembly being modelled as a rectangular block and
the bearing pivot being modelled as a bearing fixture. The CAD model was done in Solidworks
and the FEA was done in the Solidworks built-in simulation tool. The load cell and the sides of the
pivot were modelled as roller/slider fixtures so that they would be constrained in the plane but
can move laterally. The built-in 6061 aluminum alloy material model was applied to the model
due to convenience, which has the same properties as the 6105-T5 alloy, just a lower yield strength.
The mesh was generated using Solidworks’ automated mesh tool, but mesh control was applied
in order to ensure that there were at least 3 elements across each dimension to accurately model
the problem. In the image below, a zoomed in display of the maximum stress point is shown along with
the mesh for the max loading condition of 20 lbf of thrust and 15 lbf-in of torque on the long
arm model. The maximum stress ended up being in the same region for all the simulations, right
where the 90◦ bracket met the horizontal arm.

<p align="center">
  <img src="https://github.com/1iggy2/AE405-Thrust_Stand/blob/main/Images/PH1R_maxstress.png?raw=true">
</p>

Another accuracy concern is the drag produced by inlet air during wind tunnel testing, particularly
on the vertical arm of the test stand. The load cell measuring thrust will not be able to distinguish between this drag force and a thrust difference, thus reducing accuracy. To make sure that
our measurement is as accurate as possible during these testing circumstances, we surveyed various cross-sectional shapes of the vertical beam using CFD simulations. STAR-CCM+ CFD simulations were used to model the drag of the
stand. Because the only variable between these design is the vertical beam cross-section, simulations were performed using the 15 inch beam by itself, with semicircular fairings, and with one
semicircular fairing on the front face and an elliptical section on the rear face to give the overall cross-section an airfoil shape. Seen in the table below.

| No Fairing | Semi-Circular Fairing | Airfoil Fairing |
|------------|-----------------------|-----------------|
| ![](https://github.com/1iggy2/AE405-Thrust_Stand/blob/main/Images/RawDragSim.png)      | ![](https://github.com/1iggy2/AE405-Thrust_Stand/blob/main/Images/CircDragSim.png)                 | ![](https://github.com/1iggy2/AE405-Thrust_Stand/blob/main/Images/AirfoilDragSim.png)           |

The simulations were then run with the respective beam suspended,
centered in a simulated 2 foot X 2 foot wind tunnel using iterations with intake airspeed of 11, 22,
34, 45, 56, and 67 miles per hour. The results are shown in the table below.

<p align="center">
  <img src="https://github.com/1iggy2/AE405-Thrust_Stand/blob/main/Images/DragTable.PNG?raw=true">
</p>

Plotted into a graph:

<p align="center">
  <img src="https://github.com/1iggy2/AE405-Thrust_Stand/blob/main/Images/DragChart.png?raw=true">
</p>

With this information we are designing a 3D printed airfoil to slot into the 8020 aluminium extrusion. We will also be putting our structure, shrouded with an aerodynamic
fairing, into the wind tunnel and testing the amount of drag that the system generates with the
goal of minimizing this effect. This will be to validate our simulations. Our current design for the airfoil fairing is show here.

<p align="center">
  <img src="https://github.com/1iggy2/AE405-Thrust_Stand/blob/main/Images/AerofairingCrossSection.png?raw=true">
</p>
