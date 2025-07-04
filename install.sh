#!/bin/bash

# Define the program name
NAME="TabStijl"

# Define the program version
VERSION="0.0.1"

# Define default installation path
DEF_PATH="/usr/local/bin"

# Detect the operating system
OS=$(uname)

if [ "$OS" == "Linux" ]; then
    BIN_NAME="tabstijl"
else
    echo -e "Unsupported OS: $OS"
    
    exit 1
fi

# Show welcome message
echo -e "Welcome to $NAME $VERSION Installer"

# Print ASCII art logo
cat << "EOF"
         _____     _   _____ _   _   _ _ 
        |_   _|___| |_|   __| |_|_| |_| |
          | | | .'| . |__   |  _| | | | |
          |_| |__,|___|_____|_| |_|_| |_|
                                  |___|

EOF

# Check if TabStijl is already installed
INSTALLED=$(find "$DEF_PATH" | grep "$BIN_NAME")

# Set the operation mode
if [ "$INSTALLED" == "" ]; then
    OPS="install"
    OPSING="installing"
    OPSED="installed"
else
    OPS="update"
    OPSING="updating"
    OPSED="updated"
fi

# Notify user if TabStijl is already installed
if [ "$OPS" == "update" ]; then
    echo -e "$NAME $VERSION already installed in $DEF_PATH"

    PREFIX=$'\u2514\u2500 '
else
    PREFIX=""
fi

# Prompt the user for confirmation
read -rp "$(echo -e "$PREFIX")Are you sure you want to $OPS $NAME $VERSION [y/n] (default is 'y')? " INPUT

# Assume the input is ‘y’ if it's empty
if [ "$INPUT" == "" ]; then
    INPUT="y"
fi

# Validate user input
while [[ ! "$INPUT" =~ ^[yYnN]$ ]]; do
    echo -e "\nThe '$INPUT' option is not recognized. Try again"
    read -rp "$(echo -e "\u2514\u2500") Are you sure you want to $OPS $NAME $VERSION [y/n] (default is 'y')? " INPUT

    # Assume the input is ‘y’ if it's empty
    if [ "$INPUT" == "" ]; then
        INPUT="y"
    fi
done

# Proceed if user confirms with y or Y
if [[ "$INPUT" =~ ^[yY]$ ]]; then
    # Ask user to specify the installation path or use the default path
    read -rp "$(echo -e "\u2514\u2500") pecify the installation path (default is '$DEF_PATH'): " IN_PATH

    # Assume as default path if the input path is empty
    if [ "$IN_PATH" == "" ]; then
        IN_PATH=$DEF_PATH
    fi

    echo -e "\n${OPSING^} $NAME $VERSION"

    # Copy the binary to the path
    echo -e "$(echo -e "\u2514\u2500") Copying the $NAME $VERSION binary..."

    sudo cp "$BIN_NAME" "$IN_PATH/$BIN_NAME"
    sleep 1
    
    # Make the binary executable
    echo -e "$(echo -e "\u2514\u2500") Making the $NAME binary executable..."
    
    sudo chmod +x "$IN_PATH/$BIN_NAME"
    sleep 1

    # Show success message
    echo -e "\n$NAME $VERSION $VERSION successfully ${OPSED}!"
    echo -e "\nYou can type '$BIN_NAME -v' or '$BIN_NAME --version' to check the version."
    echo -e "See the GitHub page at <https://github.com/naufalhanif25/TabStijl.git>"
    
    exit 0
else
    # Show abort message
    if [ "$OPS" == "install" ]; then
        echo -e "$(echo -e "\u2514\u2500") Installation aborted"
    else
        echo -e "$(echo -e "\u2514\u2500") Update aborted"
    fi
    
    exit 1
fi
