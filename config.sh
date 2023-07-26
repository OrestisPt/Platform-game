#!/bin/bash

# Configuration script for VS Code on windows for the Introduction to Programming course.
#
# Operations it performs:
# - Installs apt packages (gcc, gdb, etc)
# - Installs the WSL and C/C++ vscode extensions
# - Creates the Documents/c_programs dir (and ~/c_programs symlink)
# - Opens c_programs in WSL

# Workaround a weird WSL issue: launching the script as curl ... | bash, and running some windows command (eg powershell.exe)
# from within the script, causes the command to close the standard input, which is the script itself, effectively
# causing the script to stop. This is avoided by running everything in a subshell
(	# subshell

case "$(uname -a)" in
    *[Mm]icrosoft*)current_os=wsl;;
    [Ll]inux*)     current_os=linux;;
    [Dd]arwin*)    current_os=macos;;
    *)          echo "Unknown OS, aborting"; exit
esac

# Detect whether the script runs in the linux labs by the presense of a /home/users3 dir (not perfect, but ok)
if [ $current_os == "linux" ] && [ -d /home/users3 ]; then
	current_os=linux-lab
	export LC_ALL=en_US.UTF-8
fi
echo Detected OS: ${current_os}


echo
echo "Checking that VS Code is installed"

if [ $current_os == "macos" ]; then
	# make sure code is in path
	export PATH="$PATH:/Applications/Visual Studio Code.app/Contents/Resources/app/bin"
fi

which code
if [ $? -ne 0 ]; then
	echo "Error: 'code' command not found, have you installed VS Code?"
	exit 1
fi

# pipeing the output of code causes a weird uv_pipe_open error, so we redirect to file
code --version | perl -wln -e '/(\d+\.\d+\.\d+)/ and print $1;' &> /tmp/vscode-log
cat /tmp/vscode-log | perl -mversion -e 'exit(version->declare("v".<>) >= version->declare("v1.53.0") ? 0 : 1)'
if [ $? -ne 0 ]; then
	echo "VS Code 1.53.0 or later is required, please upgrade"
	echo "Output of code --version :"
	code --version
	echo -n "Detected VS Code version: "
	cat /tmp/vscode-log
	exit 1
fi


echo
echo "Installing gcc and other tools. For this you will need to enter your password."
echo

if [ $current_os == "macos" ]; then
	which brew || /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install.sh)"
	brew install gcc make git lcov

elif [ $current_os != "linux-lab" ]; then
	sudo add-apt-repository universe
	sudo apt update
	sudo apt install -y gcc gdb make git valgrind lcov graphviz libgl-dev
fi

# Find WSL path of Documents dir, eg /mnt/c/Users/<windows-username>/Documents
# Fallback to Desktop or User dir.
if [ $current_os == "wsl" ]; then
	for FOLDER_NAME in MyDocuments Desktop UserProfile; do
		DOC_DIR=/mnt/$(
			powershell.exe "[Environment]::GetFolderPath('$FOLDER_NAME')" |
			sed '
				s/[\r\n]//g;
				s/\([A-Z]\):/\L\1/;
				s/\\/\//g;
			'
		)
		if [ "$DOC_DIR" != "/mnt/" ] && [ -d "$DOC_DIR" ]; then
			break
		fi
	done
else
	# Localized folder names on Linux are stored in ~/.config/user-dirs.dirs 
	# We source this and read XDG_DOCUMENTS_DIR, falling back to $HOME/Documents
	# if not set, and to $HOME if the dir does not exist.
	if [ -f ~/.config/user-dirs.dirs ]; then
		source ~/.config/user-dirs.dirs
	fi
	DOC_DIR=${XDG_DOCUMENTS_DIR:-"$HOME/Documents"}

	if [ ! -d "$DOC_DIR" ]; then
		DOC_DIR="$HOME"
	fi
fi
if [ ! -d "$DOC_DIR" ]; then
	echo -e "\n\nThe installation directory $DOC_DIR does not exist, aborting.\nPlease create it and try again.\n"
	exit
fi
echo

echo
echo "Installing VS Code extensions."
echo

if [ $current_os != "linux-lab" ]; then
	# --install-extension now works even inside the WSL
	(code --install-extension ms-vscode-remote.remote-wsl &&
	code --install-extension ms-vscode-remote.remote-ssh &&
	code --install-extension ms-vscode.cpptools &&
	code --install-extension hediet.debug-visualizer) &> /tmp/vscode-log

	exit_code=$?
	cat /tmp/vscode-log

	if [ $exit_code -ne 0 ]; then
		echo "Cannot install VS Code exensions"
		exit 1
	fi
fi


echo
echo



echo
echo "Installation successful."
echo

# start vscode (unless running via ssh)
if [ -z "$SSH_CONNECTION" ]; then
	echo "Starting VS Code in 5 seconds"
	echo

	sleep 5
	code 
fi

) # subshell (see workaroung at the beginning of the file)