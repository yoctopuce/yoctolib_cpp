#define _CRT_SECURE_NO_DEPRECATE
#include <time.h>
#include <iostream>
#include <iomanip>

#include "yocto_api.h"
#include "yocto_datalogger.h"
#include "yocto_voltage.h"

using namespace std;

int main(int argc, const char * argv[])
{
    string      errmsg;
    YDataLogger *logger;
    vector<YDataStream *> dataStreams;
    unsigned    i;

    // No exception please
    yDisableExceptions();
    
    if(yRegisterHub("usb", errmsg) != YAPI_SUCCESS) {
        cerr << "yInitAPI failed: " << errmsg << endl;
        return -1;
    }
    if(yUpdateDeviceList(errmsg) != YAPI_SUCCESS) {
        cerr << "yUpdateDeviceList failed: " << errmsg << endl;
        return -1;
    }

    logger = yFirstDataLogger();
    if(logger == NULL) {
        cout << "No data logger found" << endl;
        return 0;
    }
    cout << "Using DataLogger of " << logger->module()->get_serialNumber() << endl;

    if(logger->get_dataStreams(dataStreams) != YAPI_SUCCESS) {
        cerr << "get_dataStreams failed: " << logger->get_errorMessage() << endl;
        return -1;
    }

    cout << dataStreams.size() << " stream(s) of data." << endl;

    for(i = 0; i < dataStreams.size(); i++) {
        YDataStream *s = dataStreams[i];
        unsigned         nrows, r, c;
        
        cout << "Data stream " << i << ":" << endl;
        cout << "- Run #" << s->get_runIndex();
        cout << ", time=" << s->get_startTime();
        if(s->get_startTimeUTC()) {
            time_t utcTime = s->get_startTimeUTC();
            cout << " / UTC " << ctime(&utcTime);
        } else {
            cout << endl;
        }
        nrows = s->get_rowCount();
        if(nrows > 0) {
            cout << "- " << nrows << " samples, taken every ";
            cout << s->get_dataSamplesIntervalMs() << " [ms]" << endl;
            
            vector<string> names = s->get_columnNames();
            for(c = 0; c < names.size(); c++) {
                cout << names[c] << "\t";
            }            
            cout << endl << fixed << setprecision(1);
            
            vector< vector<double> > table = s->get_dataRows();
            for(r = 0; r < table.size(); r++) {
                vector<double> row = table[r];
                for(c = 0; c < row.size(); c++) {
                    cout << row[c] << "\t";
                }
                cout << endl;
            }
        }        
    }
    
    cout << "done." << endl;
    
    return 0;
}

