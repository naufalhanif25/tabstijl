// MIT License

// Copyright (c) 2025 Naufal Hanif

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

/**
 * @file main.cpp
 * 
 * @name tabstijl
 * @version 0.0.1
 * 
 * @brief Parses tabular data from standard input and displays it as a formatted table.
 * 
 * This program reads raw character input from `stdin`, tokenizes it based on a user-defined 
 * whitespace separator (e.g., spaces, tabs, newlines), and structures the input into rows and columns.
 * It then calculates the optimal column widths and renders the output as a styled ASCII/Unicode table 
 * using user-specified styling options.
 * 
 * Special handling is provided for:
 * - Skipping the first line (commonly used for headers)
 * - Adding table borders (top, bottom, separators)
 * - Coloring headers and body cells
 * - Customizing table and text styles
 * - Specify the header data (columns name)
 * - Setting header and body text alignment
 * - Setting whitespace separator between columns (e.g., spaces, tabs, newlines)
 * - Dynamic column width alignment with padding
 * 
 * @author Naufal Hanif
 * @date 2025
 */

// --------------------------------------------------
// Standard Library Includes
// --------------------------------------------------

#include <iostream>
#include <queue>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
#include <cctype>
#include <algorithm>

// Namespace declarations
using namespace std;  // Use standard namespace for simplicity in CLI utilities

// Program identity
#define PROGRAM_NAME "tabstijl"    // Name of the program shown in help/version
#define PROGRAM_VERSION "0.0.1"  // Semantic version of the CLI tool

// --------------------------------------------------
// ANSI Color and Text Style Escape Sequences
// --------------------------------------------------

// Text styles
#define BOLD "\e[1m"
#define DEFAULT "\e[0m"
#define INVERSE "\e[7m"
#define ITALIC "\e[3m"
#define STRIKETHROUGH "\e[9m"
#define UNDERLINE "\e[4m"

// Text colors
#define BLACK "\e[30m"
#define BLUE "\e[34m"
#define CYAN "\e[36m"
#define GREEN "\e[32m"
#define MAGENTA "\e[35m"
#define RED "\e[31m"
#define WHITE "\e[37m"
#define YELLOW "\e[33m"

// Background colors
#define BG_BLACK "\e[40m"
#define BG_BLUE "\e[44m"
#define BG_CYAN "\e[46m"
#define BG_GREEN "\e[42m"
#define BG_MAGENTA "\e[45m"
#define BG_RED "\e[41m"
#define BG_WHITE "\e[47m"
#define BG_YELLOW "\e[43m"

// Whitespace characters
#define NEWLINE '\n'
#define SPACE ' '
#define TAB '\t'
#define VOID '\0'

// Text alignments
#define ALIGN_LEFT "left"
#define ALIGN_CENTER "center"
#define ALIGN_RIGHT "right"

// Define the border style structs
// Top border style struct
typedef struct border_top {
    string left_char_unicode;
    string mid_char_unicode;
    string right_char_unicode;
    string fill_char_unicode;
} border_top;

// Middle/separator border style struct
typedef struct border_separator {
    string left_char_unicode;
    string mid_char_unicode;
    string right_char_unicode;
    string fill_char_unicode;
} border_separator;

// Bottom border style struct
typedef struct border_bottom {
    string left_char_unicode;
    string mid_char_unicode;
    string right_char_unicode;
    string fill_char_unicode;
} border_bottom;

// Table border style struct
typedef struct tab_border {
    border_top top;
    border_separator separator;
    border_bottom bottom;
    string vertical_line;
} tab_border;

/**
 * @brief Constructs a single table border row using specified Unicode characters.
 *
 * This function generates a string representing one horizontal border row 
 * (top, separator, or bottom) in a table. It takes into account the width of 
 * each column and the total number of columns. Each cell's border is drawn 
 * using the given fill character and separated using a middle junction character.
 *
 * The resulting string includes the left and right boundary characters, colored 
 * with the provided ANSI color code.
 * 
 * @param max_col_count      The total number of columns in the table.
 * @param col_width          A vector of column widths for each column (includes padding).
 * @param left_char_unicode  The Unicode character used at the left-most edge of the border.
 * @param mid_char_unicode   The Unicode character used between columns (junction point).
 * @param right_char_unicode The Unicode character used at the right-most edge of the border.
 * @param fill_char_unicode  The character used to fill horizontal space in the border line.
 * @param table_color        ANSI color escape sequence to apply to the entire border line.
 * 
 * @return A string representing a complete table border row, terminated with a newline.
 */
string get_tab_border(
    const size_t &max_col_count, 
    const vector<size_t> &col_width, 
    string left_char_unicode, 
    string mid_char_unicode, 
    string right_char_unicode, 
    string fill_char_unicode,
    string table_color
) {    
    int temp_col_width = 0, max_col_width = 0;
    
    ostringstream result_oss;

    // Calculate the total width of the border by summing column widths
    for (size_t index = 0; index < col_width.size(); index++) max_col_width += col_width[index];

    max_col_width -= col_width.size() - 1;  // Adjust for junctions

    result_oss << table_color;  // Apply color to the border
    result_oss << left_char_unicode;
    
    for (size_t st_index = 0; st_index < max_col_count; ++st_index) {
        temp_col_width += col_width[st_index] - 1;

        for (size_t nd_index = 0; nd_index < col_width[st_index]; ++nd_index) {
            result_oss << fill_char_unicode;
            
            // Insert a mid junction character between columns when appropriate
            if (nd_index == col_width[st_index] - 1 && temp_col_width < max_col_width - 1) result_oss << mid_char_unicode;
        }
    }
    
    result_oss << right_char_unicode << DEFAULT << endl;

    return result_oss.str();
}

/**
 * @brief Checks whether a string begins with a specified prefix.
 *
 * This utility function is used to verify if `domain_string` starts with
 * the content of `string_to_find`. It performs a prefix match from index 0.
 *
 * @param domain_string    The full string in which to search.
 * @param string_to_find   The prefix string to search for.
 * 
 * @return true if `domain_string` starts with `string_to_find`, false otherwise.
 */
bool starts_with(const string &domain_string, const string &string_to_find) {
    return domain_string.rfind(string_to_find, 0) == 0;
}

/**
 * @brief Determines whether a character is considered a column separator or whitespace.
 *
 * This function evaluates if the given character qualifies as a field delimiter
 * based on the selected `col_separator`. If `col_separator` is one of the standard
 * whitespace characters, it is treated explicitly. Otherwise, fallback to checking
 * general whitespace using `isspace()`.
 *
 * @param current_char     The character to evaluate.
 * @param col_separator    The user-defined or default column delimiter.
 * 
 * @return true if the character is a recognized whitespace or matches the delimiter.
 */
bool is_wspace(
    const char &current_char,
    const char &col_separator
) {
    vector<char> separators = { NEWLINE, SPACE, TAB };

    if (find(separators.begin(), separators.end(), col_separator) != separators.end()) {
        separators.clear();

        return current_char == col_separator || current_char == NEWLINE;
    } else {
        separators.clear();

        return isspace(current_char);
    }
}

/**
 * @brief Aligns a given string within a specified column width based on the desired alignment.
 *
 * This function takes a string and formats it within a given width, aligning it either 
 * to the left, center, or right. It pads the string with spaces accordingly and returns 
 * a pair containing the original column width and the formatted (aligned) string.
 *
 * @param string_to_align     The text content to align.
 * @param tab_col_width       The width of the column the text should fit into.
 * @param text_alignment      The alignment directive: "left", "center", or "right". Defaults to "left".
 * 
 * @return pair<size_t, string> A pair containing:
 *                              - The column width (as a `size_t`)
 *                              - The aligned string with space-padding as needed
 */
pair<size_t, string> align_text(
    string string_to_align,
    size_t tab_col_width,
    string text_alignment = ALIGN_LEFT
) {
    // Output stream to accumulate the final aligned string
    ostringstream result_oss;

    size_t string_length = string_to_align.length();

    // Handle right alignment
    if (text_alignment == ALIGN_RIGHT) {
        // Calculate the number of leading spaces required
        size_t col_padding = tab_col_width > string_length ? tab_col_width - string_length : 0;

        // Append leading spaces followed by the original string
        result_oss << string(col_padding, SPACE) << string_to_align;
        
        return { tab_col_width, result_oss.str() };
    }
    // Handle center alignment
    else if (text_alignment == ALIGN_CENTER) {
        // Calculate the total padding required
        size_t col_total_padding = tab_col_width > string_length ? tab_col_width - string_length : 0;

        // Divide the total padding into left and right padding
        size_t col_left_padding = col_total_padding / 2;
        size_t col_right_padding = col_total_padding - col_left_padding;

        // Append left padding, the string, then right padding
        result_oss << string(col_left_padding, ' ') << string_to_align << string(col_right_padding, SPACE);

        return { tab_col_width, result_oss.str() };
    }
    // Default to left alignment
    else {
        // Append the original string followed by trailing spaces
        result_oss << string_to_align << string(tab_col_width > string_length ? tab_col_width - string_length : 0, SPACE);
        
        return { tab_col_width, result_oss.str() };
    }
}

/**
 * @brief Clears all Unicode border characters in a given tab_border structure.
 * 
 * This function deletes all string values used to define the boundary component to minimise memory usage.
 * 
 * The structure includes definitions for the top, separator (between header and body),
 * and bottom border lines, as well as the vertical line between columns.
 * 
 * @param tab_border_struct A reference to the tab_border struct containing border characters.
 */
void clear_tab_border(tab_border &tab_border_struct) {
    // Clear bottom border characters
    tab_border_struct.bottom.fill_char_unicode.clear();
    tab_border_struct.bottom.left_char_unicode.clear();
    tab_border_struct.bottom.mid_char_unicode.clear();
    tab_border_struct.bottom.right_char_unicode.clear();

    // Clear separator line characters (between header and body)
    tab_border_struct.separator.fill_char_unicode.clear();
    tab_border_struct.separator.left_char_unicode.clear();
    tab_border_struct.separator.mid_char_unicode.clear();
    tab_border_struct.separator.right_char_unicode.clear();

    // Clear top border characters
    tab_border_struct.top.fill_char_unicode.clear();
    tab_border_struct.top.left_char_unicode.clear();
    tab_border_struct.top.mid_char_unicode.clear();
    tab_border_struct.top.right_char_unicode.clear();

    // Clear vertical line character (used between cells in rows)
    tab_border_struct.vertical_line.clear();
}

int main(
    int argc, 
    char *argv[]
) {
    // Table formatting and parsing flags
    bool headerless = false;               // Disable table header rendering
    bool exclude_first_line = headerless;  // Whether to skip the first input line during processing
    bool first_line = true;                // Flag to indicate current parsing line is the first
    bool use_border = true;                // Enable table border rendering output
    bool use_separator = true;             // Enable a horizontal line separator between header and body

    // ANSI Color and style configuration for table elements
    string table_color;                     // Color for the outer table border
    string header_text_color;               // Text color used for header row
    string body_text_color;                 // Text color used for body rows
    string header_bg_color;                 // Background color for header row
    string body_bg_color;                   // Background color for body rows
    string header_text_style;               // Text style for header row
    string body_text_style;                 // Text style for body rows
    string header_text_align = ALIGN_LEFT;  // Text aligmnet for header rows
    string body_text_align = ALIGN_LEFT;    // Text aligmnet for body rows

    vector<string> usrinput_header_data;  // Holds the header data from user input

    // Column separator character
    char col_separator = SPACE;  // A character used to separate the header and body rows of the table

    // Default single-line table border style using Unicode characters
    tab_border tab_border_style = {
        { "\u250C", "\u252C", "\u2510", "\u2500" },  // Top border: left, mid, right, horizontal line
        { "\u251C", "\u253C", "\u2524", "\u2500" },  // Separator: left, mid, right, horizontal line
        { "\u2514", "\u2534", "\u2518", "\u2500" },  // Bottom border: left, mid, right, horizontal line
        "\u2502"                                     // Vertical line between columns
    };

    // Optional double-line border style
    tab_border double_border_style = {
        { "\u2554", "\u2566", "\u2557", "\u2550" },
        { "\u2560", "\u256C", "\u2563", "\u2550" },
        { "\u255A", "\u2569", "\u255D", "\u2550" },
        "\u2551"
    };

    // Optional heavy-line border style
    tab_border heavy_border_style = {
        { "\u250F", "\u2533", "\u2513", "\u2501" },
        { "\u2523", "\u254B", "\u252B", "\u2501" },
        { "\u2517", "\u253B", "\u251B", "\u2501" },
        "\u2503"
    };

    // Optional star border style
    tab_border star_border_style = {
        { "\u2732", "\u2732", "\u2732", "\u2732" },
        { "\u2732", "\u2732", "\u2732", "\u2732" },
        { "\u2732", "\u2732", "\u2732", "\u2732" },
        "\u2551"
    };

    // Left padding around table columns
    int col_padding = 2;  // Number of spaces added on each side of a table cell value

    string PROGRAM_LOGO = "\t\t _____     _   _____ _   _   _ _ \n"
                          "\t\t|_   _|___| |_|   __| |_|_| |_| |\n"
                          "\t\t  | | | .'| . |__   |  _| | | | |\n"
                          "\t\t  |_| |__,|___|_____|_| |_|_| |_|\n"
                          "\t\t                          |___|  \n";

    // Command-line usage and help guide
    string HELP_MESSAGE = 
        "Usage: [program] [...OPTIONS]\n\n"
        "OPTIONS\n"
        "      --bbg-color=COLOR       Set body background color\n"
        "                              Available background colors:\n"
        "                                - black     - blue\n"
        "                                - cyan      - green\n"
        "                                - magenta   - red\n"
        "                                - white     - yellow\n"
        "                              Example:\n"
        "                                --bbg-color=green  # sets the body background color to green\n"
        "      --bg-color=COLOR        Set header and body background color\n"
        "                              Available background colors:\n"
        "                                - black     - blue\n"
        "                                - cyan      - green\n"
        "                                - magenta   - red\n"
        "                                - white     - yellow\n"
        "                              Example:\n"
        "                                --text-color=cyan  # sets the background color to cyan\n"
        "-b or --borderless            Hide table border\n"
        "      --border-style=STYLE    Set border style\n"
        "                              Available border styles:\n"
        "                                - double  # Double line border style\n"
        "                                - heavy   # Heavy line border style\n"
        "                                - star\n"
        "                              Example:\n"
        "                                --border-style=heavy  # sets the border style to heavy line\n"
        "      --btext-align=ALIGN     Set body text alignment\n"
        "                              Available alignments:\n"
        "                                - left\n"
        "                                - center\n"
        "                                - right\n"
        "                              Example:\n"
        "                                --btext-align=left  # sets the body text alignment to left\n"
        "      --btext-color=COLOR     Set body text color\n"
        "                              Available text colors:\n"
        "                                - black     - blue\n"
        "                                - cyan      - green\n"
        "                                - magenta   - red\n"
        "                                - white     - yellow\n"
        "                              Example:\n"
        "                                --btext-color=red  # sets the body text color to red\n"
        "      --btext-style=STYLE     Set body text style\n"
        "                              Available text styles:\n"
        "                                - bold      - inverse\n"
        "                                - italic    - strike\n"
        "                                - underline\n"
        "                              Example:\n"
        "                                --btext-style=bold  # sets the body text style to bold\n"
        "-f or --fusion                Hide the separator between header and body\n"
        "      --hbg-color=COLOR       Set header background color\n"
        "                              Available background colors:\n"
        "                                - black     - blue\n"
        "                                - cyan      - green\n"
        "                                - magenta   - red\n"
        "                                - white     - yellow\n"
        "                              Example:\n"
        "                                --hbg-color=green  # sets the header background color to green\n"
        "      --hdata                 Set header data (columns name)\n"
        "                              Example:\n"
        "                                --hdata=permission,username,group,size,filename  # each column name separated by a comma\n"
        "-h or --help                  Show help message\n"
        "      --htext-align=ALIGN     Set header text alignment\n"
        "                              Available alignments:\n"
        "                                - left\n"
        "                                - center\n"
        "                                - right\n"
        "                              Example:\n"
        "                                --htext-align=center  # sets the header text alignment to center\n"
        "      --htext-color=COLOR     Set header text color\n"
        "                              Available text colors:\n"
        "                                - black     - blue\n"
        "                                - cyan      - green\n"
        "                                - magenta   - red\n"
        "                                - white     - yellow\n"
        "                              Example:\n"
        "                                --htext-color=red  # sets the header text color to red\n"
        "      --htext-style=STYLE     Set header text style\n"
        "                              Available text styles:\n"
        "                                - bold      - inverse\n"
        "                                - italic    - strike\n"
        "                                - underline\n"
        "                              Example:\n"
        "                                --htext-style=bold  # sets the header text style to bold\n"
        "      --padding=VALUE         Set column padding\n"
        "                              Example:\n"
        "                                --padding=8  # padding 8 spaces to left\n"
        "      --separator             Set column separator\n"
        "                              Available separators:\n"
        "                                - newln   # Newline\n"
        "                                - space\n"
        "                                - tab\n"
        "                                - wspace  # every whitespace\n"
        "                              Example:\n"
        "                                --separator=wspace  # sets the separator to whitespace\n"
        "-s or --simplify              Show table in simple form (without header)\n"
        "      --tab-color=COLOR       Set table border color\n"
        "                              Available table border colors:\n"
        "                                - black     - blue\n"
        "                                - cyan      - green\n"
        "                                - magenta   - red\n"
        "                                - white     - yellow\n"
        "                              Example:\n"
        "                                --tab-color=yellow  # sets the border color to yellow\n"
        "      --text-align=ALIGN      Set header and body text alignment\n"
        "                              Available alignments:\n"
        "                                - left\n"
        "                                - center\n"
        "                                - right\n"
        "                              Example:\n"
        "                                --text-align=right  # sets the text alignment to right\n"
        "      --text-color=COLOR      Set header and body text color\n"
        "                              Available text colors:\n"
        "                                - black     - blue\n"
        "                                - cyan      - green\n"
        "                                - magenta   - red\n"
        "                                - white     - yellow\n"
        "                              Example:\n"
        "                                --text-color=red  # sets the text color to red\n"
        "      --text-style=STYLE      Set header and body text style\n"
        "                              Available text styles:\n"
        "                                - bold      - inverse\n"
        "                                - italic    - strike\n"
        "                                - underline\n"
        "                              Example:\n"
        "                                --text-style=underline  # sets the text style to underline\n"
        "      --theme=THEME           Set table theme\n"
        "                              Available themes:\n"
        "                                - matrix    - mecha\n"
        "                                - myth      - retro\n"
        "                                - sticky\n"
        "                              Example:\n"
        "                                --theme=matrix  # sets the theme to matrix\n"
        "-v or --version               Show [program] version\n\n"
        "See the GitHub page at <https://github.com/naufalhanif25/tabstijl.git>\n";

    // --------------------------------------------------
    // Option Handlers
    // --------------------------------------------------

    // Loop through all command-line arguments
    for (int index = 1; index < argc; index++) {
        const string option = static_cast<string>(argv[index]);

        // Handle configuration of -b or --borderless options
        // Disables table borders entirely
        if (option == "-b" || option == "--borderless") {
            use_border = false;

            continue;
        }
        // Handle configuration of --border-style option
        // Sets table border style configuration
        else if (starts_with(option, "--border-style")) {
            size_t equal_sign_pos = option.find("=");
            string option_key = option;

            if (equal_sign_pos != string::npos) {
                string option_value = option.substr(equal_sign_pos + 1);

                if (option_value == "double") tab_border_style = double_border_style;
                else if (option_value == "heavy") tab_border_style = heavy_border_style;
                else if (option_value == "star") tab_border_style = star_border_style;
                else {
                    // Handle invalid border style value
                    cerr << "Error: Invalid '" << option_value << "' value in '" << option_key << "' option" << endl << endl;
                    cerr << "Type '-h' or '--help' to show the help message" << endl;

                    return 1;  // Exit with error
                }

                continue;
            }
            else {
                // Handle missing '=' and value
                cerr << "Error: The '" << option_key << "' option has no value assigned" << endl << endl;
                cerr << "Type '-h' or '--help' to show the help message" << endl;

                return 1;  // Exit with error
            }
        }
        // Handle configuration of -f or --fusion options
        // Disables column separation
        else if (option == "-f" || option == "--fusion") {
            use_separator = false;

            continue;
        }
        // Handle configuration of --hdata option
        // Sets the header data
        else if (
            starts_with(option, "--hdata")
        ) {
            size_t equal_sign_pos = option.find("=");
            string option_key = option;

            if (equal_sign_pos != string::npos) {
                option_key = option.substr(0, equal_sign_pos);

                string option_value = option.substr(equal_sign_pos + 1);

                // Map received header data strings
                if (option_value != "\0") {
                    string option_value_token;
                    stringstream option_value_ss (option_value);

                    while (getline(option_value_ss, option_value_token, ',')) usrinput_header_data.push_back(option_value_token);
                }
                else {
                    // Handle invalid value
                    cerr << "Error: Invalid '" << option_value << "' value in '" << option_key << "' option" << endl << endl;
                    cerr << "Type '-h' or '--help' to show the help message" << endl;

                    return 1;  // Exit with error
                }

                continue;
            }
            else {
                // Handle missing '=' and value
                cerr << "Error: The '" << option_key << "' option has no value assigned" << endl << endl;
                cerr << "Type '-h' or '--help' to show the help message" << endl;

                return 1;  // Exit with error
            }
        }
        // Handle configuration of --htext-align, --btext-align, or --text-align options
        // Sets the alignment configuration of header or body or both
        else if (
            starts_with(option, "--htext-align") || 
            starts_with(option, "--btext-align") || 
            starts_with(option, "--text-align")
        ) {
            size_t equal_sign_pos = option.find("=");
            string option_key = option;

            if (equal_sign_pos != string::npos) {
                option_key = option.substr(0, equal_sign_pos);

                string temp_text_align;
                string option_value = option.substr(equal_sign_pos + 1);
                string &target_var = (
                    option_key == "--text-align" ? temp_text_align : 
                    option_key == "--htext-align" ? header_text_align : 
                    body_text_align
                );

                // Map received text alignment strings to text alignment constants
                if (option_value == ALIGN_LEFT) target_var = ALIGN_LEFT;
                else if (option_value == ALIGN_CENTER) target_var = ALIGN_CENTER;
                else if (option_value == ALIGN_RIGHT) target_var = ALIGN_RIGHT;
                else {
                    // Handle invalid value
                    cerr << "Error: Invalid '" << option_value << "' value in '" << option_key << "' option" << endl << endl;
                    cerr << "Type '-h' or '--help' to show the help message" << endl;

                    return 1;  // Exit with error
                }

                // Apply same alignment to both header and body if --text-align is used
                if (!temp_text_align.empty()) {
                    header_text_align = temp_text_align;
                    body_text_align = temp_text_align;
                }

                continue;
            }
            else {
                // Handle missing '=' and value
                cerr << "Error: The '" << option_key << "' option has no value assigned" << endl << endl;
                cerr << "Type '-h' or '--help' to show the help message" << endl;

                return 1;  // Exit with error
            }
        }
        // Handle configuration of --hbg-color, --bbg-color, or --bg-color options
        // Sets the background color configuration of header or body or both
        else if (
            starts_with(option, "--hbg-color") || 
            starts_with(option, "--bbg-color") || 
            starts_with(option, "--bg-color")
        ) {
            size_t equal_sign_pos = option.find("=");
            string option_key = option;

            if (equal_sign_pos != string::npos) {
                option_key = option.substr(0, equal_sign_pos);

                string temp_bg_color;
                string option_value = option.substr(equal_sign_pos + 1);
                string &target_var = (
                    option_key == "--bg-color" ? temp_bg_color : 
                    option_key == "--hbg-color" ? header_bg_color : 
                    body_bg_color
                );

                // Map received color strings to ANSI background color constants
                if (option_value == "black") target_var = BG_BLACK;
                else if (option_value == "blue") target_var = BG_BLUE;
                else if (option_value == "cyan") target_var = BG_CYAN;
                else if (option_value == "green") target_var = BG_GREEN;
                else if (option_value == "magenta") target_var = BG_MAGENTA;
                else if (option_value == "red") target_var = BG_RED;
                else if (option_value == "white") target_var = BG_WHITE;
                else if (option_value == "yellow") target_var = BG_YELLOW;
                else {
                    // Handle invalid value
                    cerr << "Error: Invalid '" << option_value << "' value in '" << option_key << "' option" << endl << endl;
                    cerr << "Type '-h' or '--help' to show the help message" << endl;

                    return 1;  // Exit with error
                }

                // Apply same color to both header and body if --bg-color is used
                if (!temp_bg_color.empty()) {
                    header_bg_color = temp_bg_color;
                    body_bg_color = temp_bg_color;
                }

                continue;
            }
            else {
                // Handle missing '=' and value
                cerr << "Error: The '" << option_key << "' option has no value assigned" << endl << endl;
                cerr << "Type '-h' or '--help' to show the help message" << endl;

                return 1;  // Exit with error
            }
        }
        // Handle configuration of --htext-style, --btext-style, or --text-style options
        // Sets the text style configuration of header or body or both
        else if (
            starts_with(option, "--htext-style") || 
            starts_with(option, "--btext-style") ||
            starts_with(option, "--text-style")
        ) {
            size_t equal_sign_pos = option.find("=");
            string option_key = option;

            if (equal_sign_pos != string::npos) {
                option_key = option.substr(0, equal_sign_pos);

                string temp_text_style;
                string option_value = option.substr(equal_sign_pos + 1);
                string &target_var = (
                    option_key == "--text-style" ? temp_text_style :
                    option_key == "--htext-style" ? header_text_style : 
                    body_text_style
                );

                // Map valid style strings to ANSI style constants
                if (option_value == "bold") target_var = BOLD;
                else if (option_value == "inverse") target_var = INVERSE;
                else if (option_value == "italic") target_var = ITALIC;
                else if (option_value == "strike") target_var = STRIKETHROUGH;
                else if (option_value == "underline") target_var = UNDERLINE;
                else {
                    // Handle invalid value
                    cerr << "Error: Invalid '" << option_value << "' value in '" << option_key << "' option" << endl << endl;
                    cerr << "Type '-h' or '--help' to show the help message" << endl;

                    return 1;  // Exit with error
                }

                // Apply same style to both header and body if --text-style is used
                if (!temp_text_style.empty()) {
                    header_text_style = temp_text_style;
                    body_text_style = temp_text_style;
                }

                continue;
            }
            else {
                // Handle missing '=' and value
                cerr << "Error: The '" << option_key << "' option has no value assigned" << endl << endl;
                cerr << "Type '-h' or '--help' to show the help message" << endl;

                return 1;  // Exit with error
            }
        }
        // Handle configuration of -h or --help options
        // Prints help message with program name injected in placeholder
        else if (option == "-h" || option == "--help") {
            size_t current_position = 0;

            string str_to_replace = "[program]";
            string str_target = PROGRAM_NAME;

            while ((current_position = HELP_MESSAGE.find(str_to_replace, current_position)) != string::npos) {
                HELP_MESSAGE.replace(current_position, str_to_replace.length(), str_target);

                current_position += str_target.length();
            }

            cout << endl << PROGRAM_LOGO << endl;
            cout << HELP_MESSAGE;

            return 0;
        }
        // Handle configuration of --padding option
        // Sets the number of spaces between table columns
        else if (starts_with(option, "--padding")) {
            size_t equal_sign_pos = option.find("=");
            string option_key = option;

            if (equal_sign_pos != string::npos) {
                option_key = option.substr(0, equal_sign_pos);
                
                try {
                    int option_value = stoi(option.substr(equal_sign_pos + 1));

                    if (option_value < 0) {
                        // Handle value less than 0
                        cerr << "Error: The value of '" << option_key << "' cannot be less than 0" << endl << endl;
                        cerr << "Type '-h' or '--help' to show the help message" << endl;

                        return 1;  // Exit with error
                    }

                    col_padding = option_value;

                    continue;
                }
                catch (const invalid_argument &error_message) {
                    // Handle invalid argument
                    cerr << "Error: Invalid value for '" << option_key << "' option" << endl << endl;
                    cerr << "Type '-h' or '--help' to show the help message" << endl;

                    return 1;  // Exit with error
                }
                catch (const out_of_range &error_message) {
                    // Handle out of range value
                    cerr << "Error: The value for the '" << option_key << "' option is out of range" << endl << endl;
                    cerr << "Type '-h' or '--help' to show the help message" << endl;

                    return 1;  // Exit with error
                }
            }
            else {
                // Handle missing '=' and value
                cerr << "Error: The '" << option_key << "' option has no value assigned" << endl << endl;
                cerr << "Type '-h' or '--help' to show the help message" << endl;

                return 1;  // Exit with error
            }
        }
        // Handle configuration of --separator option
        // Sets the character used to separate columns
        else if (starts_with(option, "--separator")) {
            size_t equal_sign_pos = option.find("=");
            string option_key = option;

            if (equal_sign_pos != string::npos) {
                option_key = option.substr(0, equal_sign_pos);

                string option_value = option.substr(equal_sign_pos + 1);

                // Map valid separator chars to ANSI separator constants
                if (option_value == "newln") col_separator = NEWLINE;
                else if (option_value == "space") col_separator = SPACE;
                else if (option_value == "tab") col_separator = TAB;
                else if (option_value == "wspace") col_separator = VOID;
                else {
                    // Handle invalid separator value
                    cerr << "Error: Invalid '" << option_value << "' value in '" << option_key << "' option" << endl << endl;
                    cerr << "Type '-h' or '--help' to show the help message" << endl;

                    return 1;  // Exit with error
                }

                continue;
            }
            else {
                // Handle missing '=' and value
                cerr << "Error: The '" << option_key << "' option has no value assigned" << endl << endl;
                cerr << "Type '-h' or '--help' to show the help message" << endl;

                return 1;  // Exit with error
            }
        }
        // Handle configuration of -h or --simplify options
        // Disables the header row and skips the first line of input
        else if (option == "-s" || option == "--simplify") {
            headerless = true;
            exclude_first_line = headerless;

            continue;
        }
        // Handle configuration of --tab-color, --btext-color, --htext-color, or --text-color options
        // Sets the text color configuration for various parts of the table
        else if (
            starts_with(option, "--tab-color") || 
            starts_with(option, "--btext-color") || 
            starts_with(option, "--htext-color") || 
            starts_with(option, "--text-color")
        ) {
            size_t equal_sign_pos = option.find("=");
            string option_key = option;

            if (equal_sign_pos != string::npos) {
                option_key = option.substr(0, equal_sign_pos);

                string temp_text_color;
                string option_value = option.substr(equal_sign_pos + 1);

                // Provides the text color constant to be use
                string &target_var = (
                    option_key == "--tab-color" ? table_color : 
                    option_key == "--text-color" ? temp_text_color : 
                    option_key == "--btext-color" ? body_text_color : 
                    header_text_color
                );

                // Map received color strings to ANSI text color constants
                if (option_value == "black") target_var = BLACK;
                else if (option_value == "blue") target_var = BLUE;
                else if (option_value == "cyan") target_var = CYAN;
                else if (option_value == "green") target_var = GREEN;
                else if (option_value == "magenta") target_var = MAGENTA;
                else if (option_value == "red") target_var = RED;
                else if (option_value == "white") target_var = WHITE;
                else if (option_value == "yellow") target_var = YELLOW;
                else {
                    // Handle invalid value
                    cerr << "Error: Invalid '" << option_value << "' value in '" << option_key << "' option" << endl << endl;
                    cerr << "Type '-h' or '--help' to show the help message" << endl;

                    return 1;  // Exit with error
                }

                // Apply same color to both header and body if --text-color is used
                if (!temp_text_color.empty()) {
                    header_text_color = temp_text_color;
                    body_text_color = temp_text_color;
                }

                continue;
            }
            else {
                // Handle missing '=' and value
                cerr << "Error: The '" << option_key << "' option has no value assigned" << endl << endl;
                cerr << "Type '-h' or '--help' to show the help message" << endl;

                return 1;  // Exit with error
            }
        }
        // Handle configuration of --theme option
        // Applies predefined formatting presets based on selected theme name
        else if (starts_with(option, "--theme")) {
            size_t equal_sign_pos = option.find("=");
            string option_key = option;

            if (equal_sign_pos != string::npos) {
                option_key = option.substr(0, equal_sign_pos);

                string option_value = option.substr(equal_sign_pos + 1);

                if (option_value == "matrix") {
                    header_text_align = ALIGN_CENTER;
                    tab_border_style = heavy_border_style;
                    table_color = GREEN;
                    header_text_style = BOLD;
                    header_text_color = GREEN;
                    body_text_color = GREEN;
                    body_text_style = BOLD;
                }
                else if (option_value == "mecha") {
                    header_text_align = ALIGN_CENTER;
                    body_text_align = ALIGN_CENTER;
                    tab_border_style = double_border_style;
                    header_text_style = BOLD;
                    header_bg_color = BG_CYAN;
                    body_bg_color = BG_MAGENTA;
                    body_text_style = UNDERLINE;
                }
                else if (option_value == "myth") {
                    header_text_align = ALIGN_CENTER;
                    body_text_align = ALIGN_CENTER;
                    tab_border_style = double_border_style;
                    table_color = RED;
                    header_bg_color = BG_RED;
                    header_text_style = BOLD;
                    header_text_color = WHITE;
                    body_text_color = MAGENTA;
                    body_bg_color = BG_BLACK;
                }
                else if (option_value == "retro") {
                    header_text_align = ALIGN_CENTER;
                    body_text_align = ALIGN_CENTER;
                    tab_border_style = star_border_style;
                    header_text_style = BOLD;
                    header_bg_color = BG_RED;
                    body_bg_color = BG_YELLOW;
                    body_text_style = ITALIC;
                }
                else if (option_value == "sticky") {
                    header_text_align = ALIGN_CENTER;
                    col_separator = TAB;
                    tab_border_style = double_border_style;
                    header_text_style = BOLD;
                    header_bg_color = BG_GREEN;
                    body_bg_color = BG_YELLOW;
                    body_text_style = UNDERLINE;
                }
                else {
                    // Handle invalid value
                    cerr << "Error: Invalid '" << option_value << "' value in '" << option_key << "' option" << endl << endl;
                    cerr << "Type '-h' or '--help' to show the help message" << endl;

                    return 1;  // Exit with error
                }

                continue;
            }
            else {
                // Handle missing '=' and value
                cerr << "Error: The '" << option_key << "' option has no value assigned" << endl << endl;
                cerr << "Type '-h' or '--help' to show the help message" << endl;

                return 1;  // Exit with error
            }
        }
        // Handle configuration of -v or --version options
        // Prints the predefined version of the program
        else if (option == "-v" || option == "--version") {
            cout << PROGRAM_NAME << " " << PROGRAM_VERSION << endl;

            return 0;
        }
        // Any unknown or invalid argument results in an error
        else {
            cerr << "Error: The '" << option << "' option is not available" << endl << endl;
            cerr << "Type '-h' or '--help' to show the help message" << endl;

            return 1;  // Exit with error
        }
    }

    // Cleanup the memory
    clear_tab_border(heavy_border_style);
    clear_tab_border(double_border_style);
    clear_tab_border(star_border_style);

    PROGRAM_LOGO.clear();
    HELP_MESSAGE.clear();

    // --------------------------------------------------
    // Variable Initialization
    // --------------------------------------------------

    int cmdout_char;                         // Stores the current character read from stdin

    queue<char> cmdout_char_queue;           // Queue to accumulate characters of a single token (cell)
    vector<string> temp_row_data;            // Temporarily stores a row of parsed tokens (strings)
    vector<vector<string>> cmdout_tab_data;  // 2D vector to store all parsed table rows
    vector<size_t> tab_col_width;            // Holds the maximum width of each column for alignment

    size_t max_col_count = 0;                // Tracks the maximum number of columns across all rows

    // --------------------------------------------------
    // Standard Input Parsing Loop
    // --------------------------------------------------

    // Reads characters from stdin and splits them into tokens and rows
    while ((cmdout_char = getchar()) != EOF) {
        // If current character is considered whitespace (based on separator rules)
        if (is_wspace(cmdout_char, col_separator)) {
            // Convert queued characters into a string token
            if (!cmdout_char_queue.empty()) {
                ostringstream cmdout_oss;

                while (!cmdout_char_queue.empty()) {
                    cmdout_oss << cmdout_char_queue.front();
                    cmdout_char_queue.pop();
                }

                string cmdout_token = cmdout_oss.str();
                
                // Push token into row data unless excluded as a header
                if (!cmdout_token.empty() && (!first_line || !exclude_first_line)) temp_row_data.push_back(cmdout_token);
            }

            // If newline, treat as end of row
            if (cmdout_char == NEWLINE) {
                // Skip the first line if headerless mode is on
                if (first_line && exclude_first_line) {
                    first_line = false;

                    continue;
                }

                // Push completed row into table data
                if (!temp_row_data.empty()) {
                    cmdout_tab_data.push_back(temp_row_data);
                    temp_row_data.clear();
                }
            }

            continue;
        }

        // If not whitespace, queue the character for token building
        cmdout_char_queue.push(static_cast<char>(cmdout_char));
    }

    // --------------------------------------------------
    // Final Token and Row Flush (after EOF)
    // --------------------------------------------------

    // Ensure any remaining characters are captured as the last token
    first_line = true;

    if (!cmdout_char_queue.empty()) {
        ostringstream cmdout_oss;

        while (!cmdout_char_queue.empty()) {
            cmdout_oss << cmdout_char_queue.front();
            cmdout_char_queue.pop();
        }

        string cmdout_token = cmdout_oss.str();

        if (!cmdout_token.empty()) temp_row_data.push_back(cmdout_token);
    }

    // Push last row if not empty
    if (!temp_row_data.empty()) cmdout_tab_data.push_back(temp_row_data);

    // --------------------------------------------------
    // Determine Maximum Column Count
    // --------------------------------------------------

    // Sets the header data
    if (!usrinput_header_data.empty() && !exclude_first_line) {
        cmdout_tab_data[0] = usrinput_header_data;

        // Cleanup the memory
        usrinput_header_data.clear();
    }

    // Finds the row with the most columns to standardize layout
    for (const auto &tab_row : cmdout_tab_data) max_col_count = max(max_col_count, tab_row.size());

    // Resize column width vector based on max column count
    tab_col_width.resize(max_col_count, 0);

    // --------------------------------------------------
    // Compute Column Widths for Alignment
    // --------------------------------------------------

    // Iterates through all cells to calculate the maximum width needed per column
    for (const auto &tab_row : cmdout_tab_data) {
        for (size_t index = 0; index < tab_row.size(); ++index) tab_col_width[index] = max(tab_col_width[index], tab_row[index].length());
    }

    // Add padding to each column width for spacing
    for (auto& col_width : tab_col_width) col_width += col_padding;

    // --------------------------------------------------
    // Render Output Table
    // --------------------------------------------------

    for (const auto &tab_row : cmdout_tab_data) {
        // Render top border if it's the first line and table borders are enabled
        if (first_line && use_border) cout << get_tab_border(
            max_col_count, 
            tab_col_width, 
            tab_border_style.top.left_char_unicode, 
            tab_border_style.top.mid_char_unicode, 
            tab_border_style.top.right_char_unicode, 
            tab_border_style.top.fill_char_unicode,
            table_color
        );

        // Print left vertical border if borders are enabled
        if (use_border) cout << table_color << tab_border_style.vertical_line << DEFAULT;

        // Print each cell in the current row
        for (size_t index = 0; index < max_col_count; ++index) {
            // Get content for current cell or empty string if missing
            string tab_cell = (index < tab_row.size()) ? tab_row[index] : "";
            
            // Get text alignment data for header or body
            pair<size_t, string> tab_col_data = align_text(
                tab_cell, 
                tab_col_width[index], 
                (first_line && !headerless ? header_text_align : !first_line || headerless ? body_text_align : ALIGN_LEFT)
            );

            // Apply styles depending on whether it's a header or body row
            cout << (first_line && !headerless ? header_text_style + header_bg_color + header_text_color : "\0") 
                 << (!first_line || headerless ? body_text_style + body_bg_color + body_text_color : "\0") 
                 << setw(tab_col_data.first) << left << tab_col_data.second << DEFAULT << table_color 
                 << (use_border ? tab_border_style.vertical_line : "\0");
        }

        // End of row
        cout << endl;

        // Render header-body separator after the first line if enabled
        if (first_line && !headerless && use_border && use_separator) cout << get_tab_border(
            max_col_count, 
            tab_col_width, 
            tab_border_style.separator.left_char_unicode, 
            tab_border_style.separator.mid_char_unicode, 
            tab_border_style.separator.right_char_unicode, 
            tab_border_style.separator.fill_char_unicode,
            table_color
        );

        // Mark first line as processed
        first_line = false;
    }

    // Cleanup the memory
    if (!header_text_color.empty()) header_text_color;
    if (!body_text_color.empty()) body_text_color;
    if (!header_bg_color.empty()) header_bg_color;
    if (!body_bg_color.empty()) body_bg_color;
    if (!header_text_style.empty()) header_text_style;
    if (!body_text_style.empty()) body_text_style;
    if (!header_text_align.empty()) header_text_align;
    if (!body_text_align.empty()) body_text_align;

    cmdout_tab_data.clear();

    // Render bottom border of the table if enabled
    if (use_border) cout << get_tab_border(
        max_col_count, 
        tab_col_width, 
        tab_border_style.bottom.left_char_unicode, 
        tab_border_style.bottom.mid_char_unicode, 
        tab_border_style.bottom.right_char_unicode, 
        tab_border_style.bottom.fill_char_unicode,
        table_color
    );

    // Cleanup the memory
    if (!table_color.empty()) table_color.clear();

    tab_col_width.clear();

    // Exit successfully
    return 0;
}
