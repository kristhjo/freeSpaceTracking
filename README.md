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
The configuration files named config_BISAM.ini and config_IQOQI.ini contains the configuration settings for the setup at Bisamberg and IQOQI respectively. These can also be used as templates for new configuration files. The configuration file is read by the trackingGui, which passes along the settings to the seeingGui and hexapodGui. Following is a list of critical settings for smooth operation: 

1. For camera operation:
* BaumerID, i.e. mac-adress of the Baumer in the local ethernet.
2. For seeing measurements: 
* correct parameters of the seeing optics. 
* imaging wavelength
3. Hexapod operation: 
* Hexapod ip adress in the local ethernet.
4. Hexapod stabilization: 
* correct parameters of the tracking optics. 
* Axis and sign of the horizontal and vertical axis. 

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
* Manual control is activated by pressing the button "Manually control hexapod". When the mouse is activated in the specified window, this activates keyboard control of the 6 axes. The step size of the motion can be set by dragging the corresponding slider
* Position coordinates is set in under the header "move to pos", followed by a "move to position" button. These coordinates are defined relative to the active coordinate system, and are therefore not necessarily absolute coordinates. 

### Hexapod coordinate systems 
The hexapod can be operated with two different coordinate systems. The ZERO system is always defined as the origin of the hexapod, i.e. absolute coordinates, while the USERDEFINED system can be set at any current coordinate. Activating a USERDEFINED coordinate system defines new axis directions along the current rotations. 

### Setting up Hexapod stabilization <br> 
1. Hexapod tracking requires the camera and hexapodgui to work simultaneously. First, find a suitable beam spot to track on. For reliable tracking, there must be a considerable contrast in brightness between other objects in the image. 
2. The beam spot centroid is then calculated and showed real-time on the image by pressing the "Start tracking" button. This centroid can be calculated either by a windowing algorithm, which defines a circular region of interest around the brightest region on the image (a gaussian blur is applied to limit single pixels to dominate the centroid), or by a thresholding algorithm, that filters out pixels below a certain brightness value (on a 0-255 scale). The corresponding radius and brightness threshold can be set and changed while tracking is activated to achieve the best setup. 
* For night-time measurements where the contrast between the beam spot and the background is considerable, the thresholding method has proved to be slightly more reliable. 
3. When the hexapod is moved to the position that is to be stabilized, set the active coordinate system to USERDEFINED at the current position. The hexapod then moves along the axes defined by the current rotation. To start stabilization, press "start stabilization" in the main gui. 
* When stabilization is activated, the program first uses 3 seconds to measure a reference beam spot position used as "home". Make sure not to disturb the setup during this procedure.
4. During stabilization, the measured beam spot position is averaged over the integration time, and a corrective tilt is applied to keep the beam spot at the "home" position. The integration time can be changed during stabilization, as well as a dead-time between each beam spot measurement, a gain factor to each corrective tilt and a maximal motion barrier (set in degrees). 

###### NB! 
* Hexapod stabilization is setup to move around two rotation axes, corresponding to horizontal and vertical movement of the beam spot in the image plane. These axes may vary with the setup of the hexapod, and must therefore be defined in the configuration file, examplary as horizontalAxis="W". Also, a sign must be assigned to the axis in the configuration file to ensure a positive rotation corresponds to the correct centroid movement. 

## Performing a seeing measurement <br> 
1. To set up a seeing measurement, first begin image capture.
2. The measurement configuration is set by pressing "configure seeing measurement", and must be defined before the measurement can start.
* To accurately perform a measurement, the parameters of the optical setup must be defined in the ini files. The optical setup is assumed to consist of a final focusing lens onto the baumer camera, with an optional magnification stage in front. The final focus length and the magnification must be defined, as well as the limiting aperture size of the setup. 

### Gaussian fit <br>


### Differential Image Motion Monitoring <br>
The DIMM measurement is calculated from the variance in the relative distance of two beam spots corresponding to the wavefront imaged over two spatially separated apertured.  

