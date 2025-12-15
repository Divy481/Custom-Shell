# MinitosShell

A custom, lightweight Linux shell implemented in C++. MinitosShell (or MintOS Shell) provides a robust command-line interface with support for process management, piping, redirection, and custom implementations of common core utilities.

## Features

### 🚀 Core Functionality
- **Command Execution**: Execute standard external Linux commands.
- **Pipelines (`|`)**: Chain multiple commands together using pipes (e.g., `ls | grep cpp`).
- **I/O Redirection**:
  - Input Redirection (`<`)
  - Output Redirection (`>`) for overwriting.
  - Append Redirection (`>>`) for appending to files.
- **Background Processes**: Run commands in the background using `&` (e.g., `sleep 10 &`).

### 🛠 Built-in Commands
MinitosShell includes valid implementations of several standard commands directly within the shell:
- **`cd <directory>`**: Change the current working directory.
- **`ls [-a] [directory]`**: Custom implementation of `ls`. Supports colored output for directories, executables, and symlinks. Use `-a` to show hidden files.
- **`cat [files...]`**: Concatenate and display file content.
- **`exit`**: Terminate the shell session.

### ⚙️ Job Control
Manage active processes directly from the shell:
- **`jobs`**: List all current background and stopped jobs with their IDs and status.
- **`fg <job_id>`**: Bring a background/stopped job to the foreground.
- **`bg <job_id>`**: Resume a stopped job in the background.
- **Signal Handling**: Appropriately handles `SIGINT` (Ctrl+C) and `SIGTSTP` (Ctrl+Z) for child processes without crashing the shell itself.

## Getting Started

### Prerequisites
- Linux Environment
- CMake (3.28 or higher)
- C++ Compiler (GCC/Clang) with C++17 support recommended.

### Building
1. Clone the repository:
   ```bash
   git clone <repository_url>
   cd customShell
   ```

2. Create a build directory and compile:
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

### Running
After compiling, start the shell using:
```bash
./run
```

## Usage Examples

**Navigation & Listing:**
```bash
mintOS[SHELL]> [ /home/user ] ~ ls -a
mintOS[SHELL]> [ /home/user ] ~ cd src
```

**Piping & Redirection:**
```bash
mintOS[SHELL]> [ /home/user/src ] ~ ls | grep .cpp > source_files.txt
mintOS[SHELL]> [ /home/user/src ] ~ cat source_files.txt
```

**Job Control:**
```bash
mintOS[SHELL]> [ /home/user ] ~ sleep 50 &
[1] 12345
mintOS[SHELL]> [ /home/user ] ~ jobs
[1] Running  sleep 50
mintOS[SHELL]> [ /home/user ] ~ fg 1
```

## Project Structure
- **`src/main.cpp`**: Main shell loop and command parsing logic.
- **`src/job_control.cpp`**: Implementation of job management (signals, reaping, fg/bg).
- **`src/ls.cpp`**: Custom implementation of the `ls` command.
- **`src/cat.cpp`**: Custom implementation of the `cat` command.
- **`src/pipe.cpp`**: Logic for handling piped commands.
- **`src/redirection.cpp`**: Logic for file descriptor manipulation for redirects.
