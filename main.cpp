#include <cstdio>
#include <cerrno>
#include <iostream>

int main(int argc, const char * argv[]) {

    if (argc != 4) {
        std::cout << "Usage: ./brightness <filename> [sum|set|minus] <brightness-level%>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    std::string operation = argv[2];
    std::string brightness_level = argv[3];
    FILE * max_brightness = fopen((filename + "/max_brightness").c_str(), "r");
    FILE * brightness = fopen((filename + "/brightness").c_str(), "r");
    if (brightness == nullptr || max_brightness == nullptr) {
        std::cerr << "Error opening file" << std::endl;
        return 1;
    }

    int max_brightness_value;
    int mb_scan = fscanf(max_brightness, "%d", &max_brightness_value);
    if (mb_scan != 1){
        std::cerr << "Error scanning max_brightness" << std::endl;
        return 1;
    }
    int brightness_value;
    int b_scan = fscanf(brightness, "%d", &brightness_value);
    if (b_scan != 1){
        std::cerr << "Error scanning brightness" << std::endl;
        return 1;
    }

    fclose(max_brightness);
    fclose(brightness);

    int target_value = -1;
    if (operation == "sum") {
        target_value = brightness_value + std::stoi(brightness_level) * max_brightness_value / 100;
    } else if (operation == "set") {
        target_value = std::stoi(brightness_level) * max_brightness_value / 100;
    } else if (operation == "minus") {
        target_value = brightness_value - std::stoi(brightness_level) * max_brightness_value / 100;
    } else {
        std::cerr << "Invalid operation" << std::endl;
        return 1;
    }
    int f_percent = max_brightness_value * 0.05;
    if (target_value < f_percent) {
        target_value = f_percent;
    } else if (target_value > max_brightness_value) {
        target_value = max_brightness_value;
    }

    FILE * brightness_output = fopen((filename + "/brightness").c_str(), "w");
    if (brightness_output == nullptr) {
        if (errno == EACCES) {
            std::cerr << "Permission denied. Try adding your user to 'video' group or running as root." << std::endl;
        } else {
            std::cerr << "Error opening file." << std::endl;
        }

        return 1;
    }
    fwrite(std::to_string(target_value).c_str(), sizeof(char), std::to_string(target_value).length(), brightness_output);
    fclose(brightness_output);

    return 0;
}
