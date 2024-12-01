#ifndef OPTIONS_INL_
#define OPTIONS_INL_

/*
@file
    Implementation of command line option handling.

    @author Roger Orr <rogero@howzatt.co.uk>

    Copyright &copy; 2003.
    This software is distributed in the hope that it will be useful, but
    without WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    Permission is granted to anyone to make or distribute verbatim
    copies of this software provided that the copyright notice and
    this permission notice are preserved, and that the distributor
    grants the recipient permission for further distribution as permitted
    by this notice.

    Comments and suggestions are always welcome.
    Please report bugs to rogero@howzatt.co.uk.

    $Revision: 2480 $
*/

// $Id: Options.inl 2480 2024-09-28 19:32:14Z roger $

#include <iomanip>
#include <iostream>
#include <vector>

namespace or2 {

/** Helper implementation class for Options */
struct Options::Data {
  /* Supported option types */
  enum OptionType { eBool, eInt, eUInt, eLong, eULong, eDouble, eString };
  /** Information for a single option */
  class OptionInfo {
  public:
    OptionInfo(std::string const &option, void *pValue, OptionType eType,
               std::string const &helpString)
        : option(option), pValue(pValue), eType(eType), helpString(helpString) {
    }

    std::string option;
    void *pValue;
    OptionType eType;
    std::string helpString;
  };

  Data() {}

  void set(std::string const &option, void *pValue, OptionType eType,
           std::string const &helpString) {
    options.push_back(OptionInfo(option, pValue, eType, helpString));
  }

  std::string rcsId;             // copy of RCS ID for printing version info
  std::string pName;             // program name
  std::vector<std::string> args; // additional arguments
  int argCountMin{-1};
  int argCountMax{-1};
  std::string argHelp;
  std::vector<OptionInfo> options; // option information
};

/** Construct from rcs ID - for version info */
inline Options::Options(char const *pRcsId) {
  pData = new Data;
  pData->rcsId = pRcsId;
}

/** Destructor  */
inline Options::~Options() { delete pData; }

/** Set a 'bool' option */
inline void Options::set(std::string const &option, bool *pValue,
                         std::string const &helpString) {
  pData->set(option, pValue, Data::eBool, helpString);
}

/** Set an 'int' option */
inline void Options::set(std::string const &option, int *pValue,
                         std::string const &helpString) {
  pData->set(option, pValue, Data::eInt, helpString);
}

/** Set an 'unsigned int' option */
inline void Options::set(std::string const &option, unsigned int *pValue,
                         std::string const &helpString) {
  pData->set(option, pValue, Data::eUInt, helpString);
}

/** Set a 'long' option */
inline void Options::set(std::string const &option, long *pValue,
                         std::string const &helpString) {
  pData->set(option, pValue, Data::eLong, helpString);
}

/** Set an 'unsigned long' option */
inline void Options::set(std::string const &option, unsigned long *pValue,
                         std::string const &helpString) {
  pData->set(option, pValue, Data::eULong, helpString);
}

/** Set an 'double' option */
inline void Options::set(std::string const &option, double *pValue,
                         std::string const &helpString) {
  pData->set(option, pValue, Data::eDouble, helpString);
}

/** Set a 'string' option */
inline void Options::set(std::string const &option, std::string *pValue,
                         std::string const &helpString) {
  pData->set(option, pValue, Data::eString, helpString);
}

/** Set argument count (-1 => any number) */
inline void Options::setArgs(int argCount, std::string const &helpString) {
  pData->argCountMin = argCount;
  pData->argCountMax = argCount;
  pData->argHelp = helpString;
}

/** Set argument count - min and max (-1 => any number) */
inline void Options::setArgs(int argCountMin, int argCountMax,
                             std::string const &helpString) {
  pData->argCountMin = argCountMin;
  pData->argCountMax = argCountMax;
  pData->argHelp = helpString;
}

/** Process the command line arguments */
inline bool Options::process(
    int argc,                 ///< count of arguments
    char **argv,              ///< pointer to arguments
    std::string const &usage) ///< usage string to print if help requested
{
  bool bRet = true;
  int argNum = 0;
  pData->pName = argv[argNum++];

  while (argNum < argc) {
    char const *pArg = argv[argNum];
    char firstCh = *pArg++;
    if ((firstCh != '-') && (firstCh != '/'))
      break;
    argNum++; // skip this argument

    // support "--" or "//" to mean end of options
    if (*pArg == firstCh)
      break;

    bool bFound = false;
    for (const auto &option : pData->options) {
      if (option.option == pArg) {
        bFound = true;
        if (option.eType != Data::eBool) {
          if (argNum == argc)
            pArg = "";
          else
            pArg = argv[argNum++];
        }

        switch (option.eType) {
        case Data::eBool:
          *((bool *)(option.pValue)) = true;
          break;
        case Data::eInt: {
          if (sscanf(pArg, "%i", ((int *)(option.pValue))) != 1) {
            std::cerr << "Invalid numeric value '" << pArg << "' found for "
                      << option.option << std::endl;
            bRet = false;
          }
        } break;
        case Data::eUInt: {
          if (sscanf(pArg, "%u", ((unsigned int *)(option.pValue))) != 1) {
            std::cerr << "Invalid numeric value '" << pArg << "' found for "
                      << option.option << std::endl;
            bRet = false;
          }
        } break;
        case Data::eLong: {
          if (sscanf(pArg, "%li", ((long *)(option.pValue))) != 1) {
            std::cerr << "Invalid numeric value '" << pArg << "' found for "
                      << option.option << std::endl;
            bRet = false;
          }
        } break;
        case Data::eULong: {
          if (sscanf(pArg, "%lu", ((unsigned long *)(option.pValue))) != 1) {
            std::cerr << "Invalid numeric value '" << pArg << "' found for "
                      << option.option << std::endl;
            bRet = false;
          }
        } break;
        case Data::eDouble: {
          if (sscanf(pArg, "%lf", ((double *)(option.pValue))) != 1) {
            std::cerr << "Invalid numeric value '" << pArg << "' found for "
                      << option.option << std::endl;
            bRet = false;
          }
        } break;
        case Data::eString:
          *((std::string *)(option.pValue)) = pArg;
          break;
        default:
          std::cerr << "Ignored option " << option.option << ", type "
                    << option.eType << std::endl;
          bRet = false;
          break;
        }
        break;
      }
    }

    if (!bFound) {
      std::string arg(pArg);
      if (arg == "ver") {
        std::cerr << pData->rcsId;
      } else if ((arg == "h") || (arg == "?") || (arg == "help")) {
        std::cerr << "Syntax: " << pData->pName;
        for (const auto &option : pData->options) {
          std::cerr << " [-" << option.option;
          switch (option.eType) {
          case Data::eBool:
            break;
          case Data::eInt:
          case Data::eUInt:
          case Data::eLong:
          case Data::eULong:
          case Data::eDouble:
            std::cerr << " #";
            break;
          case Data::eString:
            std::cerr << " *";
            break;
          }
          std::cerr << "]";
        }
        if (pData->argHelp.length() != 0) {
          std::cerr << " " << pData->argHelp;
        }

        std::cerr << std::endl;
        if (!pData->options.empty()) {
          std::cerr << std::endl << "Options:" << std::endl;
          size_t maxWidth = 0;
          {
            for (const auto &option : pData->options) {
              if (option.option.length() > maxWidth)
                maxWidth = option.option.length();
            }
          }
          for (const auto &option : pData->options) {
            std::cerr << "  -" << option.option;
            if (option.helpString.length()) {
              std::streamsize const width(static_cast<std::streamsize>(
                  maxWidth + 1 - option.option.length()));
              std::cerr << std::setw(width) << " " << option.helpString;
            }
            switch (option.eType) {
            case Data::eBool:
              break;
            case Data::eInt: {
              int def = *((int *)(option.pValue));
              if (def)
                std::cerr << " (default: " << def << ")";
            } break;
            case Data::eUInt: {
              unsigned int def = *((unsigned int *)(option.pValue));
              if (def)
                std::cerr << " (default: " << def << ")";
            } break;
            case Data::eLong: {
              long def = *((long *)(option.pValue));
              if (def)
                std::cerr << " (default: " << def << ")";
            } break;
            case Data::eULong: {
              unsigned long def = *((unsigned long *)(option.pValue));
              if (def)
                std::cerr << " (default: " << def << ")";
            } break;
            case Data::eDouble: {
              double def = *((double *)(option.pValue));
              if (def)
                std::cerr << " (default: " << def << ")";
            } break;
            case Data::eString: {
              std::string def = *((std::string *)(option.pValue));
              if (def.length())
                std::cerr << " (default: " << def << ")";
            } break;
            }
            std::cerr << std::endl;
          }
        }
        if (usage.length()) {
          std::cerr << std::endl << usage << std::endl;
        }
      } else {
        std::cerr << "Unknown option " << pArg << ": use -h for help"
                  << std::endl;
      }
      bRet = false;
    }
  }

  while (argNum < argc) {
    pData->args.push_back(argv[argNum++]);
  }

  // Verify argument count - unless already failed
  if (bRet) {
    if ((pData->argCountMin != -1) &&
        ((int)pData->args.size() < pData->argCountMin)) {
      std::cerr << "Missing arguments: " << pData->argCountMin << " argument"
                << (pData->argCountMin == 1 ? "" : "s") << " required"
                << std::endl;
      bRet = false;
    }
    if ((pData->argCountMax != -1) &&
        ((int)pData->args.size() > pData->argCountMax)) {
      std::cerr << "Too many arguments: " << pData->argCountMax << " argument"
                << (pData->argCountMax == 1 ? "" : "s") << " expected"
                << std::endl;
      bRet = false;
    }
  }

  return bRet;
}

/** Get program name */
inline std::string Options::pname() const { return pData->pName; }

/** Start of non-option arguments */
inline Options::const_iterator Options::begin() const {
  return pData->args.begin();
}

/** End of non-option arguments */
inline Options::const_iterator Options::end() const {
  return pData->args.end();
}

} // namespace or2

#endif // OPTIONS_INL_
