# STL Example Files

This directory contains example STL files to test and demonstrate the STL Viewer application.

## Available Examples

### 1. **cube.stl** 
- A simple unit cube (1x1x1)
- 12 triangles (2 per face)
- Good for basic testing

### 2. **pyramid.stl**
- Square-based pyramid 
- 6 triangles (4 sides + 2 base triangles)
- Tests sloped surfaces and sharp edges

### 3. **tetrahedron.stl**
- Regular tetrahedron (4-sided polyhedron)
- 4 triangles
- Minimal geometric shape, good for testing

### 4. **octahedron.stl**
- Regular octahedron (8-sided polyhedron)
- 8 triangles
- Tests more complex geometry with multiple faces

### 5. **cylinder.stl**
- Cylindrical approximation using triangles
- 72 triangles (12-sided circular approximation)
- Tests curved surfaces and higher triangle counts

## Usage

1. Launch the STL Viewer application
2. Click "Open STL File" or press Ctrl+O
3. Navigate to the `examples/` directory
4. Select any of the `.stl` files to load

## File Formats

All examples are in **ASCII STL** format for readability and educational purposes. The STL Viewer also supports binary STL files.

## Testing Different Features

- **Basic Loading**: Use `cube.stl` or `tetrahedron.stl`
- **Sharp Edges**: Use `pyramid.stl` or `octahedron.stl`
- **Curved Surfaces**: Use `cylinder.stl`
- **Triangle Count**: Progress from `tetrahedron.stl` (4 triangles) to `cylinder.stl` (72 triangles)

## Creating Your Own Examples

You can add your own STL files to this directory. The STL Viewer supports:
- ASCII STL format (human-readable)
- Binary STL format (compact)
- Models with any number of triangles
- Both positive and negative coordinates

## Model Dimensions

All example models are scaled to fit roughly within a 2x2x2 unit cube for consistent viewing experience.
