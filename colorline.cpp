#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

std::map<std::string, std::vector<std::string> > AnsiColorMap = {
    {"Black", {"\033[38;5;0m", "\033[48;5;0m"}}, {"Blue", {"\033[38;5;21m", "\033[48;5;21m"}}, 
    {"Green", {"\033[38;5;34m", "\033[48;5;34m"}}, {"Aqua", {"\033[38;5;45m", "\033[48;5;45m"}}, 
    {"Red", {"\033[38;5;196m", "\033[48;5;196m"}}, {"Purple", {"\033[38;5;57m", "\033[48;5;57m"}}, 
    {"Yellow", {"\033[38;5;226m", "\033[48;5;226m"}}, {"White", {"\033[38;5;15m", "\033[48;5;15m"}},
    {"Gray", {"\033[38;5;59m", "\033[48;5;59m"}}, {"Light Blue", {"\033[38;5;39m", "\033[48;5;39m"}}, 
    {"Light Green", {"\033[38;5;77m", "\033[48;5;77m"}}, {"Light Aqua", {"\033[38;5;87m", "\033[48;5;87m"}}, 
    {"Light Red", {"\033[38;5;203m", "\033[48;5;203m"}}, {"Light Purple", {"\033[38;5;141m", "\033[48;5;141m"}}, 
    {"Light Yellow", {"\033[38;5;227m", "\033[48;5;227m"}}, {"Bright White", {"\033[38;5;231m", "\033[48;5;231m"}}
};

void setColor(unsigned int color, const std::string& os = "Linux");
unsigned int getColorCode(const std::string& color);
std::string getColorName(unsigned int color);

int main(int argc, char *argv[]) {
    std::string os = "";
    std::string prefix = "";
#ifdef _WIN32
    os = "Windows";
    setColor(0x07);
#else
    os = "Linux";
    std::cout << "\033[0m";
    prefix = "./";
#endif

    if(argc >= 2 && std::string(argv[1]) == "all")
    {
        if(argc == 3)
        {
            os = std::string(argv[2]);
        }
        for(int i = 0; i < 16; i++)
        {
            setColor(i, os);
            std::cout << prefix + "colorline.exe " << std::hex << i << " (" << getColorName(i) << ")\n";
        }
        std::cout << std::endl;
        return 0;
    }
    else if (argc < 2) {
        std::cout << "Usage:" + prefix + "colorline.exe [attr] [os_name](optional)\n";
        std::cout << "Example:" + prefix + "colorline.exe all Windows\n";
        return 1;
    }

    unsigned int color = getColorCode(argv[1]);

    setColor(color, os);
    return 0;
}


void setColor(unsigned int color, const std::string& os) {
    if (color > 0xff || color < 0x00) 
    {
        color = 0x07;
    }
    unsigned int FgNum = color % 0x10;
    unsigned int BgNum = (color - FgNum) / 0x10;
    if(os == "Windows" && FgNum != 4 && FgNum != 5 && FgNum != 6)
    {
        #ifdef _WIN32
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
        #endif
    }
    else
    {
        if(color <= 0xF)
        {
            std::string FgColor = AnsiColorMap[getColorName(color)][0];
            std::cout << FgColor;
        }
        else
        {
            std::string BgColor = AnsiColorMap[getColorName(BgNum)][1];
            std::string FgColor = AnsiColorMap[getColorName(FgNum)][0];
            std::cout << BgColor;
            std::cout << FgColor;
        }
    }
}


unsigned int getColorCode(const std::string& color) {
    std::istringstream iss(color);
    int value;
    if (!(iss >> std::hex >> value)) {
        return 0x07; 
    }
    if (value > 0xff) {
        return 0x07; 
    }
    return static_cast<unsigned int>(value);
}


std::string getColorName(unsigned int color) {
    static const std::map<unsigned int, std::string> colorNames = {
        {0x0, "Black"}, {0x1, "Blue"}, {0x2, "Green"}, {0x3, "Aqua"},
        {0x4, "Red"}, {0x5, "Purple"}, {0x6, "Yellow"}, {0x7, "White"},
        {0x8, "Gray"}, {0x9, "Light Blue"}, {0xA, "Light Green"}, {0xB, "Light Aqua"},
        {0xC, "Light Red"}, {0xD, "Light Purple"}, {0xE, "Light Yellow"}, {0xF, "Bright White"}
    };
    auto it = colorNames.find(color);
    return (it != colorNames.end()) ? it->second : "Unknown";
}