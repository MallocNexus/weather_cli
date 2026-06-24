#pragma once

#include <vector>
#include <string>

namespace weather_cli {
namespace icons {

// 1. Rainy (Rain Cloud)
const std::vector<std::string> kRainy = {
    "   .------.   ",
    "  /  /  / |   ",
    "   '------'   ",
    "  /  /  /     "
};

// 2. Sunny (Clear Sky)
const std::vector<std::string> kSunny = {
    "    \\   /    ",
    "     .-.     ",
    "  ― (   ) ―  ",
    "     '-'     ",
    "    /   \\    "
};

// 3. Cloudy (Partly/Overcast Sky)
const std::vector<std::string> kCloudy = {
    "      .--.     ",
    "   .-(    ).   ",
    "  (___.___)__) ",
    "               "
};

// 4. Stormy (Thunderstorm)
const std::vector<std::string> kStormy = {
    "   .------.   ",
    "  /  /  / |   ",
    "   '------'   ",
    "      ⚡      "
};

// 5. Snowy (Snow Fall)
const std::vector<std::string> kSnowy = {
    "   .------.   ",
    "  /  /  / |   ",
    "   '------'   ",
    "   *  *  *    "
};

// 6. Clear Night (Crescent moon + stars)
const std::vector<std::string> kClearNight = {
    "  .  *    . ",
    "    .----.  ",
    "  * ( `)  . ",
    "   `----'   "
};

}  // namespace icons
}  // namespace weather_cli
