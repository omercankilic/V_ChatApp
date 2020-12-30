#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H
#include <chrono>
#include <limits>
#include <iomanip>
#include <cmath>

long double get_current_time_millisecond(){
        long double millisec_since_epoch = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
        return millisec_since_epoch;
}


#endif // HELPER_FUNCTIONS_H
