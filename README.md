# STL Viewer

A simple STL file viewer built with Qt and OpenGL.

## Features

- Load and display STL files (both ASCII and binary formats)
- Interactive 3D viewing with mouse controls
- Automatic model centering and scaling
- Modern OpenGL rendering with lighting
- Clean, intuitive user interface

## Controls

- **Left click + drag**: Rotate the model
- **Mouse wheel**: Zoom in/out  
- **Ctrl+R**: Reset view to default
- **Ctrl+O**: Open STL file
- **Ctrl+Q**: Quit application

## Requirements

- Qt 6.x
- OpenGL 3.3 or higher
- CMake 3.16 or higher
- C++17 compatible compiler

## Building

### Ubuntu/Debian

1. Install Qt6 and dependencies:
```bash
sudo apt update
sudo apt install qt6-base-dev cmake build-essential
```

Note: In Ubuntu 24.04 (Noble) and later, the OpenGL development headers are included with `qt6-base-dev`, so no separate OpenGL package is needed.

2. Build the project:
```bash
mkdir build
cd build
cmake ..
make
```

3. Run the application:
```bash
./STLViewer
```

### Other Linux Distributions

Install Qt6 development packages using your distribution's package manager, then follow the same build steps.

### Windows

1. Install Qt6 from the official Qt website
2. Install CMake and a C++ compiler (Visual Studio or MinGW)
3. Use Qt Creator or command line to build:
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### macOS

1. Install Qt6 using Homebrew:
```bash
brew install qt6
```

2. Build the project:
```bash
mkdir build
cd build
cmake ..
make
```

## Usage

1. Launch the application
2. Click "Open STL File" or use Ctrl+O to load an STL file
3. Use mouse controls to navigate around the 3D model
4. Use the "Reset View" button or Ctrl+R to return to the default view

## Example Files

The `examples/` directory contains sample STL files for testing:
- `cube.stl` - A simple cube model

## Project Structure

```
├── CMakeLists.txt          # CMake build configuration
├── README.md              # This file
├── include/               # Header files
│   ├── mainwindow.h      # Main window class
│   ├── stlviewer.h       # OpenGL viewer widget
│   └── stlloader.h       # STL file loader
├── src/                  # Source files
│   ├── main.cpp          # Application entry point
│   ├── mainwindow.cpp    # Main window implementation
│   ├── stlviewer.cpp     # OpenGL viewer implementation
│   └── stlloader.cpp     # STL file loader implementation
└── examples/             # Sample STL files
    └── cube.stl          # Example cube model
```

## Supported STL Formats

- **ASCII STL**: Text-based format starting with "solid" keyword
- **Binary STL**: Binary format with 80-byte header and triangle data

## Troubleshooting

### Black screen or no model visible
- Check that the STL file is valid and contains triangles
- Try resetting the view (Ctrl+R)
- Ensure your graphics drivers support OpenGL 3.3+

### Build errors
- Make sure Qt6 is properly installed and in your PATH
- Check that CMake can find Qt6 (`cmake --find-package Qt6`)
- Verify you have OpenGL development headers installed

### Performance issues
- Large STL files (>100k triangles) may render slowly
- Consider using STL files with fewer triangles for better performance
- Ensure hardware acceleration is enabled

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## Screenshots

*Screenshots will be added once the application is running in a graphical environment.*
