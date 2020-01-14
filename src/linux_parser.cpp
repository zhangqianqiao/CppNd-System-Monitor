#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::stol;
using std::stoi;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel >> version;
  }
  return version;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() 
{ 
  string line, key;
  long value, MemTotal, MemFree;

  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()){
    while(std::getline(stream, line)){
      std::istringstream linestream(line);
      while(linestream >> key >> value){
        if(key == "MemTotal:"){
          MemTotal = value;
        }else if(key == "MemFree:"){
          MemFree = value;
        }
      }
    }
  }
  
  float MemUsedRate = float(MemTotal - MemFree) / float(MemTotal);
  
  return MemUsedRate; 
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {

  string line;
  string upTime, procTime;

  std::ifstream stream("/proc/uptime");
  if (stream.is_open()){

    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> upTime >> procTime;
  }

  return stol(upTime); 
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 

  vector<string> CpuList = CpuUtilization();

  long TotalJiffies = 0;

  for (auto value : CpuList){
    TotalJiffies += stol(value);
  }
  
  return TotalJiffies; 
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 

  long total_time = 0;
  string value, line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    int countId = 0;
    while (linestream >> value) {
      if (countId >= 13 && countId <= 16) {
        total_time += stol(value);
      }
      countId++;
    }
  }
  return total_time;

}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  return LinuxParser::Jiffies()-LinuxParser::IdleJiffies(); 
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 

  vector<string> CpuList = CpuUtilization();

  long TotalIdel = stol(CpuList[3]) + stol(CpuList[4]);

  return TotalIdel; 
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 

  vector<string> cpuList;
  string value, line;

  std::ifstream stream("/proc/stat");

  if (stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);

    while(linestream >> value){
      if (value != "cpu"){
        cpuList.push_back(value);
      }
    }
  }

  return cpuList; 
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 

  string key, value, line;
  std::ifstream stream(kProcDirectory + kStatFilename);

  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "processes") {
        return stoi(value);
      }
    }
  }
  return 0;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 

  string key, value, line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "procs_running") {
        return stoi(value);
      }
    }
  }
  return 0;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 

  string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);

  if(stream.is_open()){
    //std::istringstream linestream(line);
    std::getline(stream, line);
  }

  return line; 
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 

  string line, key, value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize:") {
          return to_string(stoi(value) / 1024);
        }
      }
    }
  }
  return string(); 
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 

  string line, key, value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:") {
          return to_string(stoi(value) / 1024);
        }
      }
    }
  }

  return string(); 
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  
  string line;
  string user, x, suid;
  string uid = LinuxParser::Uid(pid);
  std::ifstream filestream(kPasswordPath);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream stringstream(line);
      stringstream >> user >> x >> suid;
      if (suid.compare(uid) == 0) {
        return user;
      }
    }
  }

  return string(); 
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
  long int uptime{0};
  string value;
  std::ifstream filestream(LinuxParser::kProcDirectory + to_string(pid) + LinuxParser::kStatFilename);

  if (filestream.is_open()) {
    for (int i = 0; filestream >> value; ++i)
      if (i == 13) {
        uptime = stol(value);
        return uptime;
      }
  }
  return 0; 
}