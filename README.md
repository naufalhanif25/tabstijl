<h1 align="center">TabStijl</h1>

<div align="center">
    <img src="https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white" alt="C++"></img>
</div>

## Overview

**TabStijl** is a lightweight, modern, and customizable command-line tool that formats raw textual data into beautiful tabular outputs. Designed to "wrap" command-line output with style and structure, it supports a wide range of formatting features including themes, colorization, padding, borders, and text alignment.

Inspired by the minimalism and structure of *De Stijl* design philosophy, **TabStijl** transforms messy command output into readable, aesthetic tables — right in your terminal.

## Getting Started

1. Download the executable file from the following link: 
<a href="https://github.com/naufalhanif25/tabstijl/releases/download/0.0.1/tabstijl" download>Download tabstijl</a>

2. Download `install.sh` from the following link:
<a href="https://github.com/naufalhanif25/tabstijl/releases/download/0.0.1/install.sh" download>Download install.sh</a>

3. Change `install.sh` permission to executable
    ```bash
    chmod +x install.sh
    ```

4. Run `install.sh` and follow the installation process
    ```bash
    ./install.sh
    ```

5. Check the **TabStijl** version to ensure that the installation process was successful
    ```bash
    tabstijl -v
    ```
    or 
    ```bash
    tabstijl --version
    ```

#### Example

```bash
ls -l | tabstijl --hdata=permission,hard_links,owner,group,size,month,day,time,name --text-align=center --theme=matrix --border-style=double
```

## Features

- Wraps any `stdin` stream into styled tables
- Rich styling for text and borders (color, bold, italic, etc.)
- Customizable border styles (`heavy`, `double`, `star`)
- Independent styling for header and body
- Accepts space, tab, newline, or whitespace separated data
- Alignment controls (left, center, right)
- Supports headers, with manual override (`--hdata`)
- Predefined themes: `matrix`, `mecha`, `myth`, `retro`, `sticky`
- Output simplification options (`--simplify`, `--borderless`)

## Themes

#### Matrix

![Matrix](https://github.com/naufalhanif25/tabstijl/blob/main/public/matrix.png?raw=true)

#### Mecha

![Mecha](https://github.com/naufalhanif25/tabstijl/blob/main/public/mecha.png?raw=true)

#### Myth

![Myth](https://github.com/naufalhanif25/tabstijl/blob/main/public/myth.png?raw=true)

#### Retro

![Retro](https://github.com/naufalhanif25/tabstijl/blob/main/public/retro.png?raw=true)

#### Sticky

![Sticky](https://github.com/naufalhanif25/tabstijl/blob/main/public/sticky.png?raw=true)

## Contributing

TabStijl is an open-source project — contributions, suggestions, and feedback are highly welcome! Whether you're fixing a bug, adding a new theme, or just improving the documentation, feel free to open an issue or pull request. Let's make the command line beautiful together.