#include<iostream>
#include<iomanip>
#include<getopt.h>
#include<fstream>
#include<cctype>
#include<map>
#include<vector>
#include<algorithm>
#include "freq.h"

/**
 * freq
 * Basic frequency analysis tool
 * Intended for "normal" ascii-ranged text only
 *
 * Arguments:
 *  FILE : required source file to analyze, or - to read from stdin
 *
 * Flags:
 *  -h, --help : print usage notes and quit
 *  -c, --coerce-caps : converts all letters to capitals
 *  -a, --alpha : only consider alphabetic characters
 *  -t, --table : suppress table and print output for translate
 *                (requires ground-truth file)
 *  -s, --sample [language sample] : path to language sample file
 *  -k [num]    : kmer/ngram length
 */

void printUsage(std::string exeName) {
  std::cout << "Usage: " << exeName << " [FILE]" << std::endl
    << std::endl 
    << "Computes frequency information from FILE." << std::endl;
}

Args parseArgs(int argc, char** argv) {
  Args retval;
  // todo parse command-line arguments
  static struct option long_options[] = {
    {"coerce-caps", no_argument,       0, 'c'},
    {"alpha",       no_argument,       0, 'a'},
    {"help",        no_argument,       0, 'h'},
    {"table",       no_argument,       0, 't'},
    {0, 0, 0, 0}
  };
  int c;

  while(1) {
    int option_index = 0;
    c = getopt_long(argc, argv, "cahs:k:t", long_options, &option_index);
    if (c == -1)
      break;

    switch (c) {
      case 'c':
        retval.coerceCaps = true;
        break;
      case 'a':
        retval.ignoreNonAlpha = true;
        break;
      case 'k':
        // TODO confirm this is the best way of converting to int
        // https://stackoverflow.com/a/6154614
        retval.k = std::stoi(optarg);
        break;
      case 'h':
        printUsage(argv[0]);
        // TODO exit gracefully 
        // https://stackoverflow.com/a/30251056
        abort();
      case 't':
          retval.printTable = true;
          break;
      default:
        std::cerr << "Try '" << argv[0] << " --help' for more information" << std::endl;
        abort();
    }

  }

  // only two non-flag arguments:
  if (optind == argc - 2) {
    retval.filename = argv[optind++];
    retval.sampleFilename = argv[optind];
  }
  else {
    printUsage(argv[0]);
    std::cerr << "Source and/or destination file not detected; aborting..." << std::endl;
    abort();
  }

  return retval;
}

/* Fills a given frequency map with character counts
 *  - filename : ...the filename (- = stdin)
 *  - args     : commandline args
 *  
 *  returns map of strings (kmers) to counts of occurrences
 */
std::map<std::string, int> freqFromFile(std::string filename, Args args) {
  // input points to the stream we're getting characters from
  std::istream* input;
  std::fstream fs;
  bool fromStdin = filename == "-";

  if (fromStdin)
    input = &std::cin;
  else {
    fs.open(filename);
    input = &fs;
  }

  // now iterate over the stream
  std::map<std::string, int> retval;
  int i = 0; // index into the current kmer
  char* kmer = new char[args.k + 1];
  kmer[args.k] = 0; // need null terminator
  char c;
  while (input->get(c)) {
    // skip any skippable characters
    if (args.ignoreNonAlpha && !std::isalpha(c))
      continue;
    if (args.coerceCaps)
      c = std::toupper(c);

    kmer[i++] = c;

    if (i == args.k) { // reached end of kmer; time to increment map entry
      std::string kmerStr(kmer);
      retval[kmerStr] = retval.count(kmerStr) ? retval[kmerStr] + 1 : 1;
      i = 0;
    }
  }

  delete[] kmer;

  fs.close();
  
  return retval;
}

std::map<std::string, double> freqsToPercentages(std::map<std::string, int> freqMap) {
  int totalCount = 0;
  for (auto& elem: freqMap)
    totalCount += elem.second;

  std::map<std::string, double> retval;
  for (auto& elem: freqMap)
    retval[elem.first] = double(elem.second) / totalCount;

  return retval;
}

void printTable(std::map<std::string, double> percentages, Args args) {
  int kmerWidth = std::max(args.k, 6);
  std::cout << std::left << std::setw(kmerWidth) << "kmer" 
    << "frequency" << std::endl;
  for (auto& elem: percentages) {
    std::cout << std::left 
      << std::setw(kmerWidth)
      << elem.first
      << "    " 
      << std::fixed << std::setprecision(5) << elem.second 
      << std::endl;
  }
}

void attemptSolve(std::map<std::string, double> plainMap, std::map<std::string, double> langMap) {
  typedef std::pair<std::string, double> freqPair;
  std::vector<freqPair> plain(plainMap.begin(), plainMap.end());
  std::vector<freqPair> lang(langMap.begin(), langMap.end());
  std::sort(plain.begin(), plain.end(), [](freqPair& a, freqPair& b){ return a.second > b.second; });
  std::sort(lang.begin(), lang.end(), [](freqPair& a, freqPair& b){ return a.second > b.second; });

  for (auto plainIt = plain.begin(), langIt = lang.begin();
      plainIt != plain.end() && langIt != lang.end(); 
      plainIt++, langIt++) {
    std::cout << plainIt->first << " -> " << langIt->first << std::endl;
  }
}

int main(int argc, char **argv) {
  Args args = parseArgs(argc, argv);

  auto freqMap = freqFromFile(args.filename, args);
  auto percentMap = freqsToPercentages(freqMap);

  auto freqMapLang = freqFromFile(args.sampleFilename, args);
  auto percentMapLang = freqsToPercentages(freqMapLang);

  if (args.printTable)
    printTable(percentMap, args);

  attemptSolve(percentMap, percentMapLang);

  // Attempt to solve

  return 0;
}

