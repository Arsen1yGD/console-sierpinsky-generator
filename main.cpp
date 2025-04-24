#include "libs/utfcpp/utf8/checked.h"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <exception>
#include <format>
#include <iostream>
#include <optional>
#include <ostream>
#include <string>
#include <vector>

struct Color {
  unsigned char r;
  unsigned char g;
  unsigned char b;
};

struct Settings {
  int count = std::pow(2, 4);
  std::string fill = "█";
  std::string empty = "░";
  char flips = 0;
  bool doubled = false;
  std::optional<Color> color = {};
};

void print_bit(const bool &bit, const std::string &fill,
               const std::string &empty, const bool &doubled) {
  for (int i = 0; i <= doubled; i++) {
    std::cout << (bit ? fill : empty);
  }
}

void fill_bitmap(std::vector<std::vector<bool>> &bitmap,
                 const Settings &settings) {
  for (int x = 0; x < settings.count; x++) {

    auto &row = bitmap[x];

    for (int y = 0; y < settings.count; y++) {
      row.push_back(!(x & y));
    }
  }
}

void print_bitmap(const std::vector<std::vector<bool>> &bitmap,
                  const Settings &settings) {
  const int count = settings.count;
  const std::string fill = settings.fill;
  const std::string empty = settings.empty;
  const bool doubled = settings.doubled;

  if (settings.color.has_value()) {
    const Color color = settings.color.value();
    std::cout << std::format("\e[38;2;{};{};{}m", (int)color.r, (int)color.g,
                             (int)color.b);
  }

  switch (settings.flips) {
  case 0:
    for (int x = 0; x < count; x++) {
      for (int y = 0; y < count; y++) {
        print_bit(bitmap[count - x - 1][count - y - 1], fill, empty, doubled);
      }

      std::cout << std::endl;
    }
    break;
  case 1:
    for (int x = 0; x < count; x++) {
      for (int y = 0; y < count; y++) {
        print_bit(bitmap[x][count - y - 1], fill, empty, doubled);
      }

      std::cout << std::endl;
    }
    break;
  case 2:
    for (int x = 0; x < count; x++) {
      for (int y = 0; y < count; y++) {
        print_bit(bitmap[count - x - 1][y], fill, empty, doubled);
      }

      std::cout << std::endl;
    }
    break;
  case 3:
    for (int x = 0; x < count; x++) {
      for (int y = 0; y < count; y++) {
        print_bit(bitmap[x][y], fill, empty, doubled);
      }

      std::cout << std::endl;
    }
    break;
  }

  if (settings.color.has_value()) {
    std::cout << "\e[0m";
  }
}

Settings check_args(std::vector<std::string> &args) {

  Settings settings = {};

  std::vector<std::string> findArgs = {}; // #1

  for (auto &arg : findArgs) {
    std::transform(arg.begin(), arg.end(), arg.begin(),
                                                 ::tolower);
  }

  if (auto found = std::find_if(findArgs.begin(), findArgs.end(),
                                [](auto &x) {
                                  std::transform(x.begin(), x.end(), x.begin(),
                                                 ::tolower);
                                  return x == "-s" || x == "--size";
                                });
      found != findArgs.end()) {
    int flagPos = found - findArgs.begin();

    if (flagPos + 1 < findArgs.size()) {

      std::string value = findArgs[flagPos + 1];

      int parsed = 6;

      try {
        parsed = std::stoi(value);
      } catch (const std::exception &e) {
        std::cout << std::format("Bad integer: {} ({})", value, e.what())
                  << std::endl;
        throw 1;
      }

      if (parsed > 31 || parsed < 0) {
        std::cout
            << std::format(
                   "{} is too big; Argument needs to be within 0 inclusive "
                   "and 31 exclusive. (2 ** 31 can't be a value of int)",
                   settings.count)
            << std::endl;
        throw 1;
      }

      if (parsed > 11) {
        std::cout
            << std::format(
                   "The resulting output could be large enough to "
                   "crash this program, or fill up your entire RAM (Input: 2 "
                   "** {} > 2 ** 11, {} MB usage estimated), "
                   "do you want to continue? (y/n)",
                   settings.count,
                   std::pow(4, static_cast<double>(settings.count)) *
                       sizeof(bool) / 1048576)
            << std::endl;
        std::string answer = "";

        std::cin >> answer;
        std::transform(answer.begin(), answer.end(), answer.begin(), ::tolower);

        if (answer != "y")
          throw 1;
      }

      settings.count = std::pow(2, parsed);
    } else {
      std::cout << "No value for size was provided." << std::endl;
      throw 1;
    }
  }

  if (auto found = std::find_if(findArgs.begin(), findArgs.end(),
                                [](auto &x) {
                                  std::transform(x.begin(), x.end(), x.begin(),
                                                 ::tolower);
                                  return x == "-f" || x == "--fill";
                                });
      found != findArgs.end()) {
    int flagPos = found - findArgs.begin();

    if (flagPos + 1 < findArgs.size()) {
      std::string value = findArgs[flagPos + 1];

      if (utf8::distance(value.begin(), value.end()) != 1) {
        std::cout << std::format("Character's \"{}\" length shall be 1.", value)
                  << std::endl;
        throw 1;
      }

      settings.fill = value;
    } else {
      std::cout << "No value for fill character was provided." << std::endl;
      throw 1;
    }
  }

  settings.flips = 0;

  if (auto found = std::find_if(findArgs.begin(), findArgs.end(),
                                [](auto &x) {
                                  std::transform(x.begin(), x.end(), x.begin(),
                                                 ::tolower);
                                  return x == "-e" || x == "--empty";
                                });
      found != findArgs.end()) {
    int flagPos = found - findArgs.begin();

    if (flagPos + 1 < findArgs.size()) {
      std::string value = findArgs[flagPos + 1];

      if (utf8::distance(value.begin(), value.end()) != 1) {
        std::cout << std::format("Character's \"{}\" length shall be 1.", value)
                  << std::endl;
        throw 1;
      }

      settings.empty = value;
    } else {
      std::cout << "No value for empty character was provided." << std::endl;
      throw 1;
    }
  }

  if (std::find_if(findArgs.begin(), findArgs.end(), [](auto &x) {
        std::transform(x.begin(), x.end(), x.begin(), ::tolower);
        return x == "-fx" || x == "--flipx";
      }) != findArgs.end()) {
    settings.flips |= 0b10;
  }

  if (std::find_if(findArgs.begin(), findArgs.end(), [](auto &x) {
        std::transform(x.begin(), x.end(), x.begin(), ::tolower);
        return x == "-fy" || x == "--flipx";
      }) != findArgs.end()) {
    settings.flips |= 0b1;
  }

  if (std::find_if(findArgs.begin(), findArgs.end(), [](auto &x) {
        std::transform(x.begin(), x.end(), x.begin(), ::tolower);
        return x == "-d" || x == "--doubled";
      }) != findArgs.end()) {
    settings.doubled = true;
  }

  if (auto found = std::find_if(findArgs.begin(), findArgs.end(),
                                [](auto &x) {
                                  std::transform(x.begin(), x.end(), x.begin(),
                                                 ::tolower);
                                  return x == "-c" || x == "--color";
                                });
      found != findArgs.end()) {
    int flagPos = found - findArgs.begin();

    if (flagPos + 1 < findArgs.size()) {
      std::string value = findArgs[flagPos + 1];

      std::transform(value.begin(), value.end(), value.begin(), ::tolower);

      if (value != "none") {

        int parsed;

        try {
          parsed = std::stoi(value, nullptr, 16);
        } catch (const std::exception &e) {
          std::cout << std::format("Bad integer: {} ({})", value, e.what())
                    << std::endl;
          throw 1;
        }

        Color color = {};

        color.r = (parsed >> 16) & 255;
        color.g = (parsed >> 8) & 255;
        color.b = parsed & 255;

        settings.color = color;
      }
    } else {
      std::cout << "No value for color was provided." << std::endl;
      throw 1;
    }
  }

  return settings;
}

int main(int argc, char *argv[]) {

  std::vector<std::string> findArgs = {};

  std::string program_name = argv[0];

  for (int i = 1; i < argc; i++) {
    findArgs.push_back(argv[i]);
  }

  if (std::find_if(findArgs.begin(), findArgs.end(), [](auto &x) {
        std::transform(x.begin(), x.end(), x.begin(), ::tolower);
        return x == "-h" || x == "--help";
      }) != findArgs.end()) {
    std::cout << std::format("Usage: {} [OPTIONS]", program_name) << std::endl;
    std::cout << std::endl << "Avaiable options:" << std::endl;
    std::cout << "-s, --size      size of the console image (default: 6)"
              << std::endl;
    std::cout
        << "-e, --empty     character used for emptying the image (default: ░)"
        << std::endl;
    std::cout
        << "-f, --fill      character used for filling the image (default: █)"
        << std::endl;
    std::cout << "-fx, --flipx    sets whenever flip the image by X axis"
              << std::endl;
    std::cout << "-fy, --flipy    sets whenever flip the image by Y axis"
              << std::endl;
    std::cout << "-c, --color     sets the color of an output (example: FF0000 "
                 "(red), EBBB09 (yellow), default: None)"
              << std::endl;
    std::cout << "-d, --doubled   tries to achieve 1x1 aspeect ratio of a "
                 "bitmap w/ doubling width of a bitmap"
              << std::endl;
    std::cout << std::endl
              << "Note: all of the parameters are case insensitive. That means "
                 "--empty, --Empty, --EMPTY, etc. are threated the same way."
              << std::endl;

    return 0;
  }

  Settings settings;

  try {
    settings = check_args(findArgs);
  } catch (int exception) {
    return exception;
  }

  std::vector<std::vector<bool>> bitmap(settings.count);

  fill_bitmap(bitmap, settings);

  print_bitmap(bitmap, settings);
}