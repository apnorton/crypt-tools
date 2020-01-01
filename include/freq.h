#ifndef FREQ_H
#define FREQ_H

#include<string>

struct Args {
  std::string filename = "";
  std::string sampleFilename = "";
  bool coerceCaps = false;
  bool ignoreNonAlpha = false;
  bool printTable = false;
  int k = 1;
};

void printUsage(std::string exeName);

Args parseArgs(int argc, char** argv);

std::map<std::string, int> freqFromFile(std::string filename, Args args);

std::map<std::string, double> freqsToPercentages(std::map<std::string, int> freqMap);

void printTable(std::map<std::string, double> percentages, Args args);

int main(int argc, char **argv);

#endif
