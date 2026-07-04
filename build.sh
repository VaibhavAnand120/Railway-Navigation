#!/bin/bash
set -e

echo "═══════════════════════════════════════"
echo "  India Rail Navigation System — Build"
echo "═══════════════════════════════════════"

# 1. Compile C++ engine
echo ""
echo "▶ Compiling C++ engine..."
cd cpp
g++ -std=c++17 -O2 -o railnav main.cpp graph.cpp
echo "✅ C++ engine compiled → cpp/railnav"
cd ..

# 2. Install Node.js dependencies
echo ""
echo "▶ Installing Node.js dependencies..."
cd api
npm install
echo "✅ Node.js dependencies installed"
cd ..

# 3. Install Python dependencies
echo ""
echo "▶ Installing Python dependencies..."
pip install gradio requests --quiet
echo "✅ Python dependencies installed"

echo ""
echo "═══════════════════════════════════════"
echo "  Build complete! To run:"
echo ""
echo "  Terminal 1 (API):      cd api && node server.js"
echo "  Terminal 2 (Frontend): cd frontend && python app.py"
echo ""
echo "  Gradio UI: http://localhost:7860"
echo "  API:       http://localhost:3000"
echo "═══════════════════════════════════════"
