#include <cstdio>
#include <cerrno>
#include <iostream>

FILE * read_file (std::string filename) {
    FILE * file = fopen(filename.c_str(), "r");
    if (file == nullptr) {
        std::cerr << "Error opening file" << std::endl;
        exit(1);
    }
    return file;
}

int read_file_value (std::string filename) {
    FILE * file = read_file(filename);
    int destination;
    int file_scan = fscanf(file, "%d", &destination);
    if (file_scan != 1){
        std::cerr << "Error scanning max_brightness" << std::endl;
        exit(1);
    }
    fclose(file);
    return destination;
}

FILE * edit_file (std::string filename) {
    FILE * brightness_output = fopen(filename.c_str(), "w");
    if (brightness_output == nullptr) {
        if (errno == EACCES) {
            std::cerr << "Permission denied. Try adding your user to 'video' group or running as root." << std::endl;
        } else {
            std::cerr << "Error opening file." << std::endl;
        }
        exit(1);
    }
    return brightness_output;
}

void set_brightness (std::string filename, int brightness_value) {
    FILE * brightness_output = edit_file(filename + "/brightness");
    fwrite(std::to_string(brightness_value).c_str(), sizeof(char), std::to_string(brightness_value).length(), brightness_output);
    fclose(brightness_output);
}

int main(int argc, const char * argv[]) {

    std::string usage = "Usage: ./brightness <filename> [sum|set|minus|get|off] <brightness-level%>";

    if (argc < 3 || argc > 4) {
        std::cout << usage << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    std::string operation = argv[2];

    if (argc == 3) {
        if (operation == "off") {
            set_brightness(filename, 0);
            return 0;
        } else if (operation == "get") {
            std::cout << read_file_value(filename + "/brightness") * 100 / read_file_value(filename + "/max_brightness")
                      << std::endl;
            return 0;
        } else {
            std::cout << usage << std::endl;
            return 1;
        }
    }

    if (operation == "get" || operation == "off") {
        std::cout << usage << std::endl;
        return 1;
    }

    int brightness_level = std::stoi(argv[3]);

    int max_brightness_value = read_file_value(filename + "/max_brightness");
    int brightness_value = read_file_value(filename + "/brightness");

    int value_length = max_brightness_value * (1 - 0.05);
    int capped_min_value = max_brightness_value * 0.05;

    int target_value;
    if (operation == "sum") {
        target_value = brightness_value + brightness_level * value_length / 100;
    } else if (operation == "set") {
        target_value = brightness_level * max_brightness_value / 100;
    } else if (operation == "minus") {
        target_value = brightness_value - brightness_level * value_length / 100;
    } else {
        std::cerr << "Invalid operation" << std::endl;
        return 1;
    }

    target_value = std::max(std::min(target_value, max_brightness_value), capped_min_value);

    set_brightness(filename, target_value);

    return 0;
}
