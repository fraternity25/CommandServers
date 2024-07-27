#include <iostream>
#include <optional>
#include <algorithm>
#include <filesystem> 
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <ctime>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

std::string getOperatingSystem();
std::string help(std::string command = "");
int run(const std::string& filetype, const std::string& filename, bool temp = false, std::string os = "Linux");
void read(const std::string& filename);
void write(const std::string& filetype, const std::string& filename, bool temp = false, std::string os = "Linux");
void append(const std::string& filetype, const std::string& filename);
int command(std::string command, std::string os = "Linux");
bool handle_args(int argc, char* argv[], std::string os = "Linux");

void trim(std::string& str);
void strip(std::string& str, char charToStrip);
void insert_after(std::string& str, const std::string& target, const std::string& insert_str);
void delete_backslashes(std::string& str, const std::vector<char> conditions = {});
std::string get_cmd_output(const std::string& command, int buffer_size = 256);
std::string echo(const std::string& input, bool& contains_variable);
size_t search_string(const std::string& input_str, const std::string& searched_str, int start = 0, int end = -1, const std::string& escape = "\\");
bool in(const std::vector<std::string>& vec, const std::string& str);
std::vector<std::string> split(const std::string& str, const std::optional<std::string>& delimiter, const std::pair<std::vector<std::string>, 
                               std::vector<std::string> >& enclose = {}, const std::string& ignore_enclose = "\\");
std::string lower_str(const std::string& str);
void getInput(std::string& input);
void sleep_ms(unsigned int milliseconds);

int ARGC;
std::vector<std::string> ARGV;
int returnCode = 0;
std::string line;
std::string filetype;
std::string filename;
std::string compileCmd;
std::string runCmd;

clock_t start_time, end_time;
double execution_time;
std::fstream fp;

int main(int argc, char* argv[]) 
{
    ARGC = argc;
    for(int i = 0; i < argc; i++)
    {
        ARGV.push_back(argv[i]);
    }
    std::string os = getOperatingSystem();
    std::string input;
    int answer = 0;

    if(argc > 1)
    {
        bool continue_program = handle_args(argc, argv, os);
        if(!continue_program)
        {
            return 0;
        }
    }
    
    while(true)
    {
        std::cout << "1) Run File 2) Read File 3) Write Into File 4) Append To The File 5) Enter Command 6) Exit\n Choose: ";
        std::getline(std::cin, input);
        try
    	{
    		answer = std::stoi(input);
		}
		catch(const std::invalid_argument& e)
		{
		    std::cerr << "Error: " << e.what() << "\n"; 
		    continue; 
		}
        
        if(answer == 1 || answer == 2 || answer == 3 || answer == 4 || answer == 5 || answer == 6)
        {
            if(answer == 1) 
            {
                bool temp_state = false;
                std::cout << "supported file types: (.exe), (.c), (.cpp), (.py), (.mp3)\n";
                std::cout << "Enter file type(.extension) or \"back\" to return to options: ";
                getInput(filetype);

                if(filetype == "back")
                {
                    continue;
                }

                std::cout << "Enter file path(type \"temp\" to create temporary file for testing): ";
                getInput(filename);

                if(filename == "temp")
                {
                    filename += "_from_cmd" + filetype ;
                    temp_state = true;
                }

                run(filetype, filename, temp_state, os);
                continue;
            }
            else if(answer == 2)
            {
                std::cout << "Enter file path, variable name or \"back\" to return to options: ";
                getInput(filename);
                
                if(filename == "back")
                {
                    continue;
                }
                
                read(filename);
                continue;
            }
            else if(answer == 3)
            {
                std::cout << "Enter file type(.extension) or \"back\" to return to options: ";
                getInput(filetype);
                
                if(filetype == "back")
                {
                    continue;
                }
                
                std::cout << "Enter file path: ";
                getInput(filename);
                
                write(filetype, filename, false, os);
                continue;
            }
            else if(answer == 4)
            {
                std::cout << "Enter file type(.extension) or \"back\" to return to options: ";
                getInput(filetype);
            
                if (filetype == "back")
                {
                    continue;
                }
            
                std::cout << "Enter file path: ";
                getInput(filename);
                
                append(filetype, filename);
                continue;
            }
            else if(answer == 5)
            {
                command("", os);
                continue;
            }
            else if(answer == 6) 
            {
                std::cout << "Program ended.\n";
                break;
            }
        }
        else 
        {
            std::cout << "Error: Invalid choice\n";
            continue;
        }
    }

    return 0;
}

void trim(std::string& str)
{
    // Trim leading whitespaces
    str.erase(str.begin(), std::find_if_not(str.begin(), str.end(), [](unsigned char c) { return std::isspace(c); }));

    // Trim trailing whitespaces
    str.erase(std::find_if_not(str.rbegin(), str.rend(), [](unsigned char c) { return std::isspace(c); }).base(), str.end());
}

void strip(std::string& str, char charToStrip) 
{
    size_t startpos = str.find_first_not_of(charToStrip);
    size_t endpos = str.find_last_not_of(charToStrip);

    if(startpos != std::string::npos && endpos != std::string::npos)
    {
        str = str.substr(startpos, endpos - startpos + 1);
    }
    else
    {
        str.clear();
    }
}

void insert_after(std::string& str, const std::string& target, const std::string& insert_str) 
{
    size_t pos = 0;
    while ((pos = str.find(target, pos)) != std::string::npos) 
    {
        pos += target.length(); 
        str.insert(pos, insert_str); 
        pos += insert_str.length(); 
    }
}

void delete_backslashes(std::string& str, const std::vector<char> conditions) 
{
    for (size_t i = 0; i < str.length() - 1; ++i) 
    {
        if (str[i] == '\\' && (conditions.empty() || 
        std::find(conditions.begin(), conditions.end(), str[i + 1]) != conditions.end())) 
        {
            str.erase(i, 1);
        }
    }
}

std::string get_cmd_output(const std::string& command, int buffer_size) 
{
    // Open a pipe to read the output of the command
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) 
    {
        return "Error: Unable to run command";
    }

    // Read the output of the command
    char buffer[buffer_size];
    std::string result;
    while (!feof(pipe)) 
    {
        if (fgets(buffer, buffer_size, pipe) != nullptr) 
        {
            result += buffer;
        }
    }

    // Close the pipe
    pclose(pipe);

    // Remove trailing newline character, if any
    if (!result.empty() && result.back() == '\n') 
    {
        result.pop_back();
    }

    return result;
}

std::string echo(const std::string& input, bool& contains_variable)
{
    if(input.empty())
    {
        return "";
    }
    std::string command = "echo " + input;
    std::string output = get_cmd_output(command);
    contains_variable = output != input;
    return output;
}

size_t search_string(const std::string& input_str, const std::string& searched_str, int start, int end, const std::string& escape) 
{
    //check absolute value of start and end
    if((start > 0 && abs(start) >= input_str.length()) || (end > 0 && abs(end) >= input_str.length()) ||
        abs(start) > input_str.length() || abs(end) > input_str.length())
    {
        return std::string::npos;
    }
    else if(start < 0)
    {
        start = input_str.length() + start;
    }
    else if(end < 0)
    {
        end = input_str.length() + end;
    }

    int pharantes_count = 0;
    size_t string_found = std::string::npos;
    bool in_pharantes = false;
    bool in_quotes = false;
    bool loop = true;
    int step = end > start ? 1 : -1;
    int searched_index = step == 1 ? 0: searched_str.length() - 1;

    for (int i = start; loop; i+= step) 
	{
        if (input_str[i] == '"' || input_str[i] == '\'') 
		{
            int escape_length = escape.length();
            if(searched_str[0] != '\"' && searched_str[0] != '\'')
            {
                for(int k = 0; k < escape_length; k++)
                {
                    if(step == 1 && i - k - 1 < start)
                    {
                        in_quotes = !in_quotes;
                        break;
                    }
                    else if(step == -1 && i - k - 1 < end)
                    {
                        in_quotes = !in_quotes;
                        break;
                    }

                    if(input_str[i - k - 1] == escape[escape_length - k - 1])
                    {
                        continue;
                    }
                    else
                    {
                        in_quotes = !in_quotes;
                        break;
                    }
                }
            }
        } 
		else if (!in_quotes && searched_str[0] != '(' && searched_str[0] != '[' && searched_str[0] != '{' && 
                               (input_str[i] == '(' || input_str[i] == '[' || input_str[i] == '{')) 
		{
            if(!in_pharantes)
            {
                in_pharantes = !in_pharantes;
            }
            pharantes_count++;
        } 
		else if ((!in_quotes && in_pharantes) && (input_str[i] == ')' || input_str[i] == ']' || input_str[i] == '}') ) 
		{
            pharantes_count--;
            if (pharantes_count == 0)
            {
                in_pharantes = !in_pharantes;
            }
        } 
		
        if (input_str[i] == searched_str[searched_index] && (!in_quotes && !in_pharantes)) 
		{
            if(searched_index == 0)
            {
                string_found = i;
                for (int j = 1; j < searched_str.length(); j++) 
                {
                    if (input_str[i + j] == searched_str[j]) 
                    {
                        continue;
                    } 
                    else 
                    {
                        string_found = std::string::npos;
                        i = i + j - 1;
                        break;
                    }
                }
            }
            else if(step == -1)
            {
                for(int j = 1; j <= searched_index; j++)
                {
                    if(input_str[i - j] == searched_str[searched_index - j])
                    {
                        if(j == searched_index)
                        {
                            string_found = i - searched_index;
                            break;
                        }
                        continue;
                    }
                    else
                    {
                        string_found = std::string::npos;
                        i = i - j + 1;
                        break;
                    }
                }
            }

            if (string_found != std::string::npos) 
			{
                return string_found;  // Return first index of searched string
            } 
        }
        loop = step == 1 ? i < end : i > end;
    }

    return std::string::npos;
}

bool in(const std::vector<std::string>& vec, const std::string& str)
{
    std::string trimmed = str;
    trim(trimmed);
    for(const auto& elem : vec)
    {
        if(trimmed == elem)
        {
            return true;
        }
    }
    return false;
}

std::vector<std::string> split(const std::string& str, const std::optional<std::string>& delimiter_opt, const std::pair<std::vector<std::string>, std::vector<std::string>>& enclose, const std::string& ignore_enclose) 
{
    std::vector<std::string> tokens;
    int open_count = 0;
    bool in_enclose = false;
    bool trim = false;
    bool empty = false;
    if(delimiter_opt.has_value())
    {
        empty = delimiter_opt.value().empty();
    }
    else
    {
        trim = true;
    }
    std::string new_str = "";

    for (int i = 0; i < str.size(); ++i)
    {
        // Check if current character is an opening or closing enclose character
        bool in_open = in(enclose.first, std::string(1, str[i]));
        bool in_close = in(enclose.second, std::string(1, str[i]));

        // Handle enclosure and escape logic
        if (in_open && in_close && open_count == 0 && (i == 0 || str.substr(i - ignore_enclose.size(), ignore_enclose.size()) != ignore_enclose))
        {
            in_enclose = !in_enclose;  // Toggle enclosure state
            new_str += str[i];
            continue;
        }
        else if (in_open && !in_close && (i == 0 || str.substr(i - ignore_enclose.size(), ignore_enclose.size()) != ignore_enclose))
        {
            open_count++;
            in_enclose = true;  // Enter into enclosure
            new_str += str[i];
            continue;
        }
        else if (in_close && !in_open && (i == 0 || str.substr(i - ignore_enclose.size(), ignore_enclose.size()) != ignore_enclose))
        {
            open_count--;
            if (open_count == 0)
            {
                in_enclose = false;  // Exit from enclosure
                new_str += str[i];
                continue;
            }
        }

        // Split based on delimiter when not in enclosure and not escaped
        if (open_count == 0 && !in_enclose)
        {
            if(trim)
            {
                if(std::isspace(str[i]))
                {
                    if(!new_str.empty())
                    {
                        tokens.push_back(new_str);
                        new_str = "";
                    }
                    continue;
                }
                else
                {
                    new_str += str[i];
                }
            }
            else if(empty)
            {
                tokens.push_back(std::string(1, str[i]));
            }
            else if (delimiter_opt && str.substr(i, delimiter_opt.value().size()) == delimiter_opt.value())
            {
                // Found delimiter outside of enclosure and not escaped
                if (!new_str.empty())
                {
                    tokens.push_back(new_str);
                    new_str = "";
                }
                i += delimiter_opt.value().size() - 1;  // Skip delimiter length
            }
            else
            {
                new_str += str[i];
            }
        }
        else if(in_enclose)
        {
            new_str += str[i];
        }
    }

    if (!new_str.empty())
    {
        tokens.push_back(new_str);  // Push back the last token
    }

    return tokens;
}

std::string lower_str(const std::string& str) 
{
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return result;
}

void getInput(std::string& input) 
{
    std::getline(std::cin, input);
    bool contains_variable = false;
    input = echo(input, contains_variable);
    trim(input);
    //strip(input, '"');
}

void sleep_ms(unsigned int milliseconds) 
{
    #ifdef _WIN32
    Sleep(milliseconds);
    #else
    usleep(milliseconds * 1000); // usleep takes microseconds
    #endif
}

std::string getOperatingSystem() 
{
#ifdef __linux__
    return "Linux";
#elif _WIN32
    return "Windows";
#elif __APPLE__
    return "macOS";
#else
    return "Unknown";
#endif
}

std::string help(std::string command)
{
    std::string info = "";
    if(command == "")
    {
        info += "You can run this program in the following ways:\n";
        info += "1) Run the program without any arguments to enter the interactive mode.\n";
        info += "2) Run the program with the following arguments:\n";
        info += "   -help or /h: Display help information\n";
        info += "   -run or /x <fileNameWithExt>: Run a file (if there is no extension, it will be treated as an executable file)\n";
        info += "   -read or /r <fileNameWithExt>: Read a file\n";
        info += "   -write or /w <fileNameWithExt>: Write to a file\n";
        info += "   -append or /a <fileNameWithExt>: Append to a file\n";
        info += "   -command, /c, or /k <command>: Enter a command\n";
        info += "   -command is same with /c. It will close the program after the command is executed.\n";
        info += "   If you want to run a command and continue using the program, use /k.\n\n";
        info += "   run \"-help <command>\" to get help information about a specific command.\n";
        info += "   example: -help run\n";
    }
    else if(command == "run")
    {
        info += "run <filename>: Run a file\n";
        info += "Enter the file path or variable name to run the file.\n";
        info += "Supported file types: (.exe), (.c), (.cpp), (.py), (.mp3)\n";
        info += "Type \"temp\" to create a temporary file for testing.\n";
    }
    else if(command == "read")
    {
        info += "read <filename>: Read a file\n";
        info += "Enter the file path or variable name to read the file.\n";
    }
    else if(command == "write")
    {
        info += "write <filename>: Write to a file\n";
        info += "Enter the file path or variable name to write to the file.\n";
    }
    else if(command == "append")
    {
        info += "append <filename>: Append to a file\n";
        info += "Enter the file path or variable name to append to the file.\n";
    }
    else if(command == "command")
    {
        info += "command <command>: Enter a command\n";
        info += "Enter a command to run in the terminal.\n";
    }
    std::cout << info;
    return info;
}

int run(const std::string& filetype, const std::string& filename, bool temp, std::string os)
{
    if(filename.empty() || filetype.empty())
    {
        std::cout << "Error: File name or file type is empty.\n";
        return 0;
    }
    std::string _filename = filename;
    _filename.erase(std::remove(_filename.begin(), _filename.end(), '"'), _filename.end());
    std::string lowercaseFiletype = lower_str(filetype); 

    if(lowercaseFiletype != ".exe" && temp)
    {
        write(lowercaseFiletype, _filename, temp, os);
    }
    
    if(lowercaseFiletype == ".exe" || lowercaseFiletype == "(.exe)")
    {
        std::string runCmd = _filename;
        std::cout << "Attempting to run ...\n\n";
        sleep_ms(250);
        start_time = clock();
        if(os == "Linux")
        {
            runCmd = "./\"" + runCmd + "\"";
        }
        else if(os == "Windows")
        {
            runCmd = "cmd.exe /c \"" + runCmd + "\"";
        }
        returnCode = system(runCmd.c_str());
        end_time = clock();
        execution_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
        std::cout << "\n--------------------------------\nProcess exited after ";
        std::cout << execution_time << " seconds with return value " << returnCode << std::endl;
    }
    else if(lowercaseFiletype == ".c" || lowercaseFiletype == "(.c)" || lowercaseFiletype == ".cpp" ||
       lowercaseFiletype == "(.cpp)" || lowercaseFiletype == ".c++" || lowercaseFiletype == "(.c++)")
    {
        if(lowercaseFiletype == ".c" || lowercaseFiletype == "(.c)")
        {
            compileCmd = "gcc \\\"" + _filename + "\\\" -o output.exe";
        }
        else if(lowercaseFiletype == ".cpp" || lowercaseFiletype == "(.cpp)" || 
                lowercaseFiletype == ".c++" || lowercaseFiletype == "(.c++)")
        {
            compileCmd = "g++ \\\"" + _filename + "\\\" -o output.exe";
            //bash -c "g++ 'Cpp linux/hello_world.cpp' -o 'Cpp linux/hello_world.exe'"
        }
        std::cout << "Enter linker arguments (type \"0\" for no arguments): ";
        std::string linkerArgs;
        std::getline(std::cin, linkerArgs);
        bool contains_variable = false;
        linkerArgs = echo(linkerArgs, contains_variable);
        trim(linkerArgs);
        if(linkerArgs != "0")
        {
            compileCmd += " " + linkerArgs;
        }

        if(os == "Linux")
        {
            compileCmd = "bash -c \"" + compileCmd + "\"";
        }
        else if(os == "Windows")
        {
            compileCmd = "powershell.exe -command \"" + compileCmd + "\"";
        }
        std::cout << "command = " << compileCmd << std::endl;
        //insert_after(compileCmd, "output.exe", " 2>&1");
        returnCode = system(compileCmd.c_str());

        if (returnCode != 0) 
        {
            perror("Error while compiling ");
            std::cout << "errno: " << errno << std::endl;
            if (fp.is_open()) 
            {
                fp.close();
            }
            return returnCode;
        } 
        else 
        {
            std::cout << "Compiling ..." << std::endl;
            sleep_ms(250);
            std::cout << "Compilation successful." << std::endl;
        }

        // Run the compiled program
        std::cout << "Enter the arguments to pass to the program (type \"none\" for no arguments): ";
        std::string argument_str;
        std::getline(std::cin, argument_str);
        trim(argument_str);
        std::vector arguments = split(argument_str, {}, std::pair<std::vector<std::string>, std::vector<std::string>>({"\""}, {"\""}), "\\");
        std::string runCmd;
        if(os == "Linux")
        {
            runCmd = "./output.exe";
        }
        else if(os == "Windows")
        {
            runCmd = "cmd.exe /c output.exe";
        }
        if(arguments.size() > 0 && arguments[0] != "none")
        {
            for(const auto& arg : arguments)
            {
                runCmd += " " + arg;
            }
        }
        trim(runCmd);
        std::cout << "command = " << runCmd << std::endl;
        std::cout << "clear screen(y/n): ";
        char clear;
        std::cin >> clear;
        if(clear == 'y' && os == "Windows")
        {
            system("cls");
        }
        else if(clear == 'y' && os == "Linux")
        {
            system("clear");
        }
        std::cin.ignore();
        std::cout << "Attempting to run ...\n\n";
        sleep_ms(75);
        start_time = clock();
        returnCode = system(runCmd.c_str());
        // Record the end time after the process
        end_time = clock();
        // Calculate the execution time
        execution_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
        std::cout << "\n--------------------------------\nProcess exited after ";
        std::cout << execution_time << " seconds with return value " << returnCode << std::endl;
        std::remove("output.exe");
    }
    else if(lowercaseFiletype == ".py" || lowercaseFiletype == "(.py)")
    {
        // Replace with the appropriate command to run the Python script
        //C:\\Users\\husey\\OneDrive\\Python Dosyalari\\Run Files.py
        std::cout << "Enter the arguments to pass to the Python script (type \"none\" for no arguments): ";
        std::string argument_str;
        std::getline(std::cin, argument_str);
        trim(argument_str);
        std::vector arguments = split(argument_str, {}, std::pair<std::vector<std::string>, std::vector<std::string>>({"\""}, {"\""}), "\\"); 
        std::string runCmd = "python3 " + _filename;
        if(arguments.size() > 0 && arguments[0] != "none")
        {
            for(const auto& arg : arguments)
            {
                runCmd += " " + arg;
            }
        }
        trim(runCmd);
        std::cout << "command = " << runCmd << std::endl;
        std::cout << "clear screen(y/n): ";
        char clear;
        std::cin >> clear;
        if(clear == 'y' && os == "Windows")
        {
            system("cls");
        }
        else if(clear == 'y' && os == "Linux")
        {
            system("clear");
        }
        std::cin.ignore();
        std::cout << "Attempting to run ...\n\n";
        sleep_ms(250);
        start_time = clock();
        returnCode = system(runCmd.c_str());
        end_time = clock();
        execution_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
        std::cout << "\n--------------------------------\nProcess exited after ";
        std::cout << execution_time << " seconds with return value " << returnCode << std::endl;
    }
    else if(lowercaseFiletype == ".mp3" || lowercaseFiletype == "(.mp3)")
    {
        std::string runCmd = "python3 \"C:\\Users\\husey\\OneDrive\\Python Dosyalari\\Ses Calma.py\"";
        std::cout << "Attempting to play audio ...\n";
        sleep_ms(250);
        start_time = clock();
        returnCode = system(runCmd.c_str());
        end_time = clock();
        execution_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
        std::cout << "\n--------------------------------\nProcess exited after ";
        std::cout << execution_time << " seconds with return value " << returnCode << std::endl;
    }
    /*else  // sound, image and video files
    {
        
    }*/
    if(lowercaseFiletype != ".exe" && temp)
    {
        if(!std::filesystem::remove(_filename))
        {
            perror("Error deleting the temporary file ");
            std::cout << "errno: " << errno << std::endl;
        }
    }
    return returnCode;
}

void read(const std::string& filename)
{
    std::string _filename = filename;
    trim(_filename);
    
    fp.open(_filename, std::ios::in);
    if(!fp.is_open()) 
    {
        perror("Error: Can't opened the file to display ");
        std::cout << "errno: " << errno << std::endl;
        return;
    }
    else if(fp.is_open())
    {
        std::cout << "Content of the file:\n\n";
        while (std::getline(fp, line)) 
        {
            std::cout << line << std::endl;
        }
        std::cout << std::endl;
        fp.close();
    }
}

void write(const std::string& filetype, const std::string& filename, bool temp, std::string os)
{
    fp.open(filename, std::ios::out);
    if (!fp.is_open()) 
    {
        perror("Error: Can't create/open file ");
        std::cout << "errno: " << errno << std::endl;
        return;
    }

    std::string lowercaseFiletype = lower_str(filetype); 
    if(lowercaseFiletype == ".c" || lowercaseFiletype == "(.c)" || lowercaseFiletype == ".cpp" ||
       lowercaseFiletype == "(.cpp)" || lowercaseFiletype == ".c++" || lowercaseFiletype == "(.c++)")
    {
        std::cout << "Enter the text to write to the file ";
        if(!temp)
        {
            std::cout << "(type \"out\" to save it without running.";
            std::cout << " Type \"run\" to run it after saving):";
        }
        else
        {
            std::cout << "(type \"out\" or \"run\" to stop writing):";
        }
        std::cout << "\n\n";
        while (std::getline(std::cin, line)) 
        {
            line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
            if (line == "run" || line == "run;" || line == "out" || line == "out;") 
            {
                break;
            }
            else if (!line.empty())
            {
                fp << line << std::endl;
            }
        }
        
        fp.close();
        std::cout << "Text written to the file successfully.\n\n";
        
        if((line == "run" || line == "run;") && !temp)
        {
            run(filetype, filename, false, os);
        }
    }
    else if(lowercaseFiletype == ".py" || lowercaseFiletype == "(.py)")
    {
        std::cout << "Enter the text to write to the file ";
        if(!temp)
        {
            std::cout << "(type \"out\" to save it without running.";
            std::cout << " Type \"run\" to run it after saving):";
        }
        else
        {
            std::cout << "(type \"out\" or \"run\" to stop writing):";
        }
        std::cout << "\n\n";
        /*std::cout << "Enter the text to write to the file (type \"out\" to save it ";
        std::cout << "without running. Type \"run\" to run it after saving):\n\n";*/
        while (std::getline(std::cin, line)) 
        {
            if (line == "run" || line == "run;" || line == "out" || line == "out;")
            {
                break;
            }
            fp << line << std::endl;
        }
        
        fp.close();
        std::cout << "Text written to the file successfully.\n\n";
        
        if((line == "run" || line == "run;") && !temp)
        {
            //C:\\Users\\husey\\OneDrive\\Python Dosyalari\\Run Files.py
            run(".py", filename);
        }
    }
    else
    {
        std::cout << "Enter the text to write to the file (type \"exit\" to stop writing):\n\n";
        while (std::getline(std::cin, line)) 
        {
            if (line == "exit" || line == "exit;")
            {
                break;
            }
            
            fp << "\033[31m" << line << "\033[0m" << std::endl;
            fp << "\033[32m" << line << "\033[0m" << std::endl;
            fp << "\033[34m" << line << "\033[0m" << std::endl;
        }
        
        fp.close();
        std::cout << "Text written to the file successfully.\n\n";
    }
}

void append(const std::string& filetype, const std::string& filename)
{
    fp.open(filename, std::ios::app); 
            
    if (!fp.is_open())
    {
        perror("Error: Can't create/open file ");
        std::cout << "errno: " << errno << std::endl;
        return;
    }

    std::string lowercaseFiletype = lower_str(filetype);
    if(lowercaseFiletype == ".c" || lowercaseFiletype == "(.c)" || lowercaseFiletype == ".cpp" ||
    lowercaseFiletype == "(.cpp)" || lowercaseFiletype == ".c++" || lowercaseFiletype == "(.c++)")
    {
        std::cout << "Enter the text to append to the file (type \"exit\" to stop appending):\n\n";

        while (std::getline(std::cin, line))
        {
            trim(line);

            if (line == "exit" || line == "exit;")
            {
                break;
            }

            fp << line << std::endl; 
        }

        fp.close(); // Close the file after appending

        std::cout << "Text appended to the file successfully.\n\n";
    }

    while (std::getline(std::cin, line))
    {
        trim(line);

        if (line == "exit" || line == "exit;")
        {
            break;
        }

        fp << line << std::endl; 
    }

    fp.close(); // Close the file after appending

    std::cout << "Text appended to the file successfully.\n\n";
}

int command(std::string command, std::string os)
{
    bool continue_command = true;
    bool empty = command.empty();
    std::string prefix = os == "Linux" ? "./" : "cmd.exe /c ";
    std::string clear = os == "Linux" ? "clear" : "cls";
    system((prefix + "colorline.exe 0A").c_str());
    if(empty)
    {
        std::cout << "Type \"command_list\" to see all valid commands in this program.\n";
        std::cout << "Type \"exit\" to exit.\n";
    }
    do
    {
        std::cout << "Command: ";
        if(empty)
        {
            std::getline(std::cin, command);
        }
        bool contains_variable = false;
        command = echo(command, contains_variable);
        trim(command);
        std::vector<std::string> command_args = split(command, {}, std::pair<std::vector<std::string>, std::vector<std::string>>({"\""}, {"\""}), "\\");
        /*if(contains_variable)
        {
            command.erase(std::remove(command.begin(), command.end(), '"'), command.end());
        }*/

        if(!empty)
        {
            continue_command = false;
            std::cout << command << std::endl;
        }
        
        if(command == "exit")
        {
            system((prefix + "colorline.exe 7").c_str());
            system(clear.c_str());
            break;
        }
        else if(command == "command_list")
        {
            std::cout << "List of valid commands:\n";
            std::cout << "1) run <filename>: Run a file\n";
            std::cout << "2) read <filename>: Read a file\n";
            std::cout << "3) write <filename>: Write to a file\n";
            std::cout << "4) append <filename>: Append to a file\n";
            std::cout << "5) command: Enter a command\n";
            std::cout << "6) exit: Exit the program\n";
            std::cout << "delete <filename>: Delete a file\n";
            std::cout << "colors: Display the available colors\n";
            continue;
        }
        else if(command == "colors")
        {
            system((prefix + "colorline.exe all").c_str());
            continue;
        }
        else if (command_args[0] == "cd") 
        {
            std::string directory = command.substr(2);
            trim(directory);
            strip(directory, '\"');
            if(!directory.empty())
            {
                try
                {
                    std::filesystem::current_path(directory);
                }
                catch(const std::exception& e)
                {
                    std::cerr << e.what() << '\n';
                    std::cout << directory << " is not a valid directory.\n";
                }                
            }
            else
            {
                system("cd");
            }
            continue;
        }
        else if (command_args[0] == "run") 
        {
            std::string fileName = command.substr(3);
            trim(fileName);
            std::string fileType = "";
            size_t pos = fileName.find_last_of(".");
            if(pos == std::string::npos)
            {
                fileType = ".exe";
            }
            else
            {
                fileType = fileName.substr(pos);
            }
            run(fileType, fileName, false, os);
            continue;
        }
        else if (command_args[0] == "read") 
        {
            std::string fileName = command.substr(4);
            trim(fileName);
            read(fileName);
            continue;
        }
        else if (command_args[0] == "write") 
        {
            std::string fileName = command.substr(5);
            trim(fileName);
            std::string fileType = "";
            size_t pos = fileName.find_last_of(".");
            if(pos == std::string::npos)
            {
                fileType = ".exe";
            }
            else
            {
                fileType = fileName.substr(pos);
            }
            write(fileType, fileName, false, os);
            continue;
        }
        else if (command_args[0] == "append") 
        {
            std::string fileName = command.substr(6);
            trim(fileName);
            std::string fileType = "";
            size_t pos = fileName.find_last_of(".");
            if(pos == std::string::npos)
            {
                fileType = ".exe";
            }
            else
            {
                fileType = fileName.substr(pos);
            }
            append(fileType, fileName);
            continue;
        }
        else if (command_args[0] == "delete") 
        {
            // Check if the command starts with "delete"
            std::string filePath = command.substr(6); // Extract the file path (skip "delete ")
            trim(filePath);

            // Use the remove function to delete the file
            if (std::remove(filePath.c_str()) == 0) 
            {
                std::cout << "File '" << filePath << "' deleted successfully.\n";
            } 
            else 
            {
                std::cout << "filepath = " << filePath << std::endl;
                perror("Error deleting the file");
                std::cout << "errno: " << errno << std::endl;
                // Additional error handling if needed
            }
            continue;
        }
        else if(command == "-printargs")
        {
            std::cout << "argc = " << ARGC << std::endl;
            for(int i = 0; i < ARGC; i++)
            {
                std::cout << "argv[" << i << "] = " << ARGV[i] << std::endl;
            }
            continue;
        }
        else
        {
            system(command.c_str());
            continue;
        }
    }
    while(continue_command);
    return 1;
}

bool handle_args(int argc, char* argv[], std::string os)
{
    bool continue_program = false;
    std::string arg = argv[1];
    if(argc >= 2 && (arg == "-help" || arg == "/h"))
    {
        if(argc == 2)
        {
            help("");
        }
        else
        {
            for(int i = 2; i < argc; i++)
            {
                std::string command = argv[i];
                help(command);
            }
        }
    }
    else if(argc > 2 && arg == "-run" || arg == "/x")
    {
        for(int i = 2; i < argc; i++)
        {
            std::string fileName = argv[i];
            std::string fileType = "";
            std::size_t pos = fileName.find_last_of(".");
            if (pos != std::string::npos) 
            {
                fileType = fileName.substr(pos);
            } 
            else 
            {
                fileType = ".exe"; 
            }
            run(fileType, fileName, false, os);
            i++;
        }
    }
    else if(argc > 2 && arg == "-read" || arg == "/r")
    {
        for(int i = 2; i < argc; i++)
        {
            std::string fileName = argv[i];
            read(fileName);
            i++;
        }
    }
    else if(argc > 2 && arg == "-write" || arg == "/w")
    {
        for(int i = 2; i < argc; i++)
        {
            std::string fileName = argv[i];
            std::string fileType = "";
            std::size_t pos = fileName.find_last_of(".");
            if (pos != std::string::npos) 
            {
                fileType = fileName.substr(pos);
            } 
            else 
            {
                fileType = ".exe"; 
            }
            write(fileType, fileName, false, os);
            i++;
        }
    }
    else if(argc > 2 && arg == "-append" || arg == "/a")
    {
        for(int i = 2; i < argc; i++)
        {
            std::string fileName = argv[i];
            std::string fileType = "";
            std::size_t pos = fileName.find_last_of(".");
            if (pos != std::string::npos) 
            {
                fileType = fileName.substr(pos);
            } 
            else 
            {
                fileType = ".exe"; 
            }
            append(fileType, fileName);
            i++;
        }
    }
    else if(argc > 2 && arg == "-command" || arg == "/c" || arg == "/k")
    {
        if(arg == "/k")
        {
            continue_program = true;
        }

        for(int i = 2; i < argc; i++)
        {
            std::string _command = argv[i];
            command(_command, os);
        }
    }
    return continue_program;
}
