#ifndef RECEIVER_H
#define RECEIVER_H
#include "FFmpeg_Headers.h"
#include <QObject>
namespace vc{
    
    
    class Receiver:public QObject
    {
            Q_OBJECT
        public:
            
            friend class VideoCall;
        protected:
            Receiver();
            void create_decoder();
            void start_reading();
            void pause_reading();
            void stop_reading();
            void start_recording();
            
            
    };

}
#endif // RECEIVER_H
