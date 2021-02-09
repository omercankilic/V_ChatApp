# V-Chat

## System Requirements  
### C++ 11 and above
### OpenCv
   Opencv can be installed via command below:
   $ sudo apt install libopencv-dev python3-opencv
### QMake version 3.1, Using Qt version 5.12.8
   Way 1)
   To download Qt you can check this https://www.qt.io/download-open-source website and click Download the Qt-Online Installer
   **Note:** In QtCreator you should give the correct directories in .pro file for detecting ffmpeg and other libraries correctly.
   Way 2)
      1 - sudo apt install qtcreator
      2 - sudo apt install build-essential
      3 - sudo apt install qt5-default
      4 - sudo apt install qt5-doc qt5-doc-html qtbase5-doc-html qtbase5-examples

   
### FFmpeg 4.2.4 :
   **How to download ?**
      Way 1) You can follow the instructions in this link **https://linuxize.com/post/how-to-install-ffmpeg-on-ubuntu-20-04/**
      or you may try the commands below
      
      $ sudo apt update
      $ sudo apt install ffmpeg
      $ ffmpeg -version  //version check
      $ ffmpeg -encoders //valid encoders check
      $ ffmpeg -decoders //valid decoders check
      
      Way 2)
      1 - Install FFmpeg repository (https://github.com/FFmpeg/FFmpeg.git)
	2 - Install "nasm" with command "sudo apt-get install nasm"
	3 - Go to the FFmpeg directory and write into the terminal
		./configure        "For creating configuration"
		make 	    	       "For building"
		sudo make install  "For installing build data"

      
### Operating Sytem : 
   Ubuntu 20.04 LTS
    
## Network Challange
  **1-** Buffering and latency
  	 No buffering in our application, 400 ms latency detected in our tests.
  **2-** Packet loss
  	 We decreased gop size to 12,hence even we lose packet we will see at least I frame in each 500 ms. In addition we made some arrangements in encoder and decoder setups to decrease packet loss.
  **3-** Data/frame size
  	 We just send raw and encoded data over network.
  **4-** Frame synchronization
  	 From ffmpeg decoder we are getting a playable frame, ffmpeg helps us with synchronization.
  
## Last note : 
  We give the name of the video source name by hand for test. Your computer video source director may be different so your can check VideoCall 
  Constructor and change **active_camera.second** with the your computer's video source ip like /dev/video2..
  You can check connected webcams to your computer with commands above. **Note:** You can not reach the same device from different application 
  windows.
      $ sudo apt-get install v4l-utils
      $ v4l2-ctl --list-devices

        In the case of any problem you can contact me.
        omercankkilic@gmail.com
