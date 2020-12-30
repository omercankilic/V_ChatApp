#include "camera_capture.h"
#include <iostream>
#include <Helper_Functions.h>
using namespace std;

Camera_Capture::Camera_Capture()
{
        cout<<"Videocapture class started" <<endl;
        
}

void Camera_Capture::set_format_ctx()
{
        
        fmt_ctx = avformat_alloc_context();
        if(fmt_ctx == nullptr){
                cout<< "format context olusturulamadi";
                return;
        }
        avdevice_register_all();
        this->input_format = av_find_input_format("video4linux2");
        if(input_format == nullptr){
                cout<<"Input format bulunamadi"<<endl;
                return;
        }
        //Dictionary options can be set here
        {
                if( av_dict_set(&(this->dict_ctx),"framerate","25",0)<0){
                        cout<<"dictionary set frame rate error "<<endl;
                        return;
                }
        }
        
        //
        
        if( avformat_open_input(&(this->fmt_ctx),this->file_name,this->input_format,nullptr) < 0 ){
                cout<<"can not open format context "<<endl;
         }
        
        //prints device information
        av_dump_format(this->fmt_ctx,0,this->file_name,0);
        
        AVPacket *temp_packet =av_packet_alloc();
        
        while(true){
                
                cout<< "1 : "<<setprecision(std::numeric_limits<long double>::digits10 + 1)<<get_current_time_millisecond()<<endl;
                av_read_frame(this->fmt_ctx,temp_packet);
                cout<<"2 : "<< setprecision(std::numeric_limits<long double>::digits10 + 1)<<get_current_time_millisecond()<<endl;
                
                
        }
        
        
        
        int a ;
        cin>>a;
        
        
        
        
}

void Camera_Capture::set_dict_opt()
{
        
}

int Camera_Capture::start_video_from_cam()
{
        Mat frame;
        //--- INITIALIZE VIDEOCAPTURE
        VideoCapture cap;
        // open the default camera using default API
        // cap.open(0);
        // OR advance usage: select any API backend
        int deviceID = 0;             // 0 = open default camera
        int apiID = cv::CAP_ANY;      // 0 = autodetect default API
        // open selected camera using selected API
        cap.open(deviceID + apiID);
        // check if we succeeded
        if (!cap.isOpened()) {
                cerr << "ERROR! Unable to open camera\n";
                return -1;
        }
        //--- GRAB AND WRITE LOOP
        cout << "Start grabbing" << endl
             << "Press any key to terminate" << endl;
        for (;;)
        {
                // wait for a new frame from camera and store it into 'frame'
                
                
                cout<< "1 : "<<setprecision(std::numeric_limits<long double>::digits10 + 1)<<get_current_time_millisecond()<<endl;
                cap.read(frame);
                
                // encoder acma
                // encode 
                // avformat_w
                
                
                cout<<"2 : "<< setprecision(std::numeric_limits<long double>::digits10 + 1)<<get_current_time_millisecond()<<endl;
                
                
                // check if we succeeded
                if (frame.empty()) {
                        cout<<"empty frame"<<endl;
                        // cerr << "ERROR! blank frame grabbed\n";
                        // break;
                }
                // show live and wait for a key with timeout long enough to show images
                imshow("Live", frame);
                waitKey(5);
        }
        // the camera will be deinitialized automatically in VideoCapture destructor
        return 0;
        
        
}
