# freeSpaceTracking <br>
A Qt application used for tracking in a free space quantum key-distribution experiment, automating the interactions between a Baumer VGL-22M camera and a PI systems hexapod platform. The application additionally features the options of doing on the fly atmospheric seeing measurements.

## Getting started: (linux) <br>

qt5:<br>
sudo apt-get install build-essential <br>
get latest qt5 distro from qhttps://www.qt.io/download <br>

opencv2: <br>
sudo apt-get install libopencv-dev <br>
 
Additionally, software libraries from Baumer and PI Systems must be included in the src directory. 
## Setting a  configuration


## Connecting to a camera
1. Initialize a configuration. This unfreezes the other gui buttons.  
* Within the configuration file, the network ID of the relevant Baumer is listed as BaumerID. To add a different camera, change this ID. 
2. Press "Connect Baumer". If succesful, this is displayed in the text window. 
* This freezes the configuration choice. To unfreeze, press "Disconnect Baumer".
3. Press start and stop to begin and end live camera capture respectively. 
* The camera must be in image capture mode to be able to start tracking or to configure seeing measurements. 
* The main gui window offers the possibility of setting the exposure time (in micro sec), gain, frame rate and image region of interest by using the width, height and X and Y offset options. 

![Main window](/docs/trackingGui.png)

## Controlling the Hexapod
1. Initialize a configuration. 
* Within the configuration file, the ip adress of the hexapod is stored. 
2. Press "Connect hexapod". If succesfull, this initates an internal reference procedure in the hexapod to find the correct origin position. 
3. The hexapod can be controlled manually with the keyboard, or by setting the position of the X, Y, Z, U, V and W axes in the hexapodgui window. 
* Manual control is activated by pressing the button "Manually control hexapod". When the mouse is activated in the specified window, this activates keyboard control of the 6 axes. The step size of the motion can be set by dragging the corresponding slider. 
4. 
### Hexapod coordinate systems 

### Setting up Hexapod tracking <br> 

## Performing a seeing measurement <br> 

### Gaussian fit <br>

### Differential Image Motion Monitoring <br>


