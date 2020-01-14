#include "processor.h"
#include "linux_parser.h"
#include <unistd.h>

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 

    long Total = LinuxParser::Jiffies();
    long idle = LinuxParser::IdleJiffies();
    //long nonIdle = LinuxParser::ActiveJiffies();

    long Totald = Total - this->preTotal;
    long idled = idle - this->preIdle;

    this->preTotal = Total;
    this->preIdle = idle;

    float CpuPercentage = (float)(Totald - idled) / idled;

    return CpuPercentage; 
}