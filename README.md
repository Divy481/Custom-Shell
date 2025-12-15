# 🐚 MinitosShell (MintOS)

![Status](https://img.shields.io/badge/status-active-success.svg)
![Language](https://img.shields.io/badge/language-C++17-blue.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)

> **A robust, lightweight, and custom Linux shell built from scratch in C++.**

MinitosShell (or **MintOS**) is not just another wrapper around `system()`. It is a fully functional shell implementation that manually parses commands, manages processes, handles signals, and manipulates file descriptors for redirection and piping. Designed for educational purposes and lightweight usage.

---

## ⚡ Key Features

### 🖥️ Built-in Commands
Unlike standard shells that rely entirely on external binaries, MintOS includes custom implementations for speed and control:
- **`cd`**: Advanced directory navigation.
  - `cd ~`: Go to home directory.
  - `cd -`: Go to previous directory.
  - `cd -P <path>`: Resolve symlinks physically before changing directories.
  - Supports environment variable expansion (e.g., `cd $HOME`).
- **`ls`**: Colored output listing.
  - Handles directories (BLUE), executables (GREEN), and symlinks (CYAN).
  - Supports `-a` for hidden files.
- **`cat`**: file concatenator.
- **`exit`**: Graceful shutdown.

### 🔧 Core Shell Mechanics
- **Tokenization**: Smart parsing respecting double quotes (`"My Folder/file.txt"`).
- **Pipelines (`|`)**: Seamlessly chain commands.
  - Example: `ls -a | grep ".cpp" | wc -l`
- **I/O Redirection**:
  - `>` : Overwrite output to file.
  - `>>`: Append output to file.
  - `<` : Read input from file.
- **Job Control**:
  - Run background jobs with `&`.
  - Detailed job listing with `jobs`.
  - Foreground control with `fg <id>`.
  - Background resumption with `bg <id>`.
  - Full signal handling (`SIGINT`, `SIGTSTP`, `SIGCHLD`) to prevent zombie processes.

---

## 🏗 Architecture

The shell operates on a continuous REPL (Read-Eval-Print Loop) cycle:

1.  **Initialize**: Sets up signal handlers (ignoring generic `SIGINT`/`SIGTSTP` for the shell itself) and initializes the job table.
2.  **Prompt**: Displays a colored prompt `mintOS[SHELL]> [cwd] ~`.
3.  **Tokenize**: Splits input strings while respecting quoted arguments.
4.  **Parse**: Detects piping `|`, redirection `< >`, or background flags `&`.
5.  **Execute**:
    - **Built-ins**: Executed directly in the parent process (essential for `cd`).
    - **External**: Forked via `execvp`.
    - **Pipes**: Constructed using `pipe()` and `dup2()` with recursive forking.

---

## 🚀 Getting Started

### Prerequisites
- Linux / macOS (Unix-like environment)
- C++ Compiler (g++ / clang++)
- CMake

### Installation
```bash
# 1. Clone the repository
git clone https://github.com/yourusername/MinitosShell.git
cd MinitosShell

# 2. Build using CMake
mkdir build && cd build
cmake ..
make

# 3. Run
./run
```

---

## 📚 Usage Examples

### Navigation & File Management
```bash
mintOS[SHELL]> [ /home ] ~ cd Desktop
mintOS[SHELL]> [ /home/Desktop ] ~ ls -a
.  ..  customShell  resume.pdf
```

### Advanced Piping
```bash
mintOS[SHELL]> [ /src ] ~ cat main.cpp | grep "include" > headers.txt
mintOS[SHELL]> [ /src ] ~ cat headers.txt
#include <iostream>
#include <vector>
```

### Job Management
```bash
mintOS[SHELL]> [ / ] ~ sleep 100 &
[1] 14520
mintOS[SHELL]> [ / ] ~ jobs
[1] Running   sleep 100
mintOS[SHELL]> [ / ] ~ fg 1
# (Process brought to foreground)
```

---

## 🔮 Future Roadmap
- [ ] Tab Autocompletion
- [ ] Command History (Up/Down arrow navigation)
- [ ] Environment Variable (`export`) support
- [ ] `.mintrc` configuration file support

---

## 🤝 Contributing
Contributions are welcome! Please fork the repository and submit a pull request.

**Authors**: Devil & Team
