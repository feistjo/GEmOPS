#include "web_interface.h"

// std::vector<Display::Point> *WebInterface::points_ = new std::vector<Display::Point>;
Display *WebInterface::display_ = nullptr;
std::vector<char> WebInterface::aws_data{};