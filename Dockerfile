# Use a lightweight python image with build tools
FROM python:3.9-slim

# Install system dependencies (build-essential for compiling C++, curl for Node)
RUN apt-get update && apt-get install -y \
    build-essential \
    curl \
    && curl -sL https://deb.nodesource.com/setup_18.x | bash - \
    && apt-get install -y nodejs \
    && rm -rf /var/lib/apt/lists/*

# Set working directory inside the container
WORKDIR /app

# Copy all project files into the container
COPY . .

# Compile the C++ navigation engine for Linux
RUN cd cpp && g++ -std=c++17 -O2 -o railnav main.cpp graph.cpp

# Install Node.js API dependencies
RUN cd api && npm install

# Install Gradio and requests dependencies
RUN pip install --no-cache-dir gradio requests

# Hugging Face Spaces will expose port 7860
EXPOSE 7860

# Run both the python UI (internally on port 8000) and the Node API (acting as proxy on port 7860)
CMD ["sh", "-c", "cd /app/frontend && python app.py & cd /app/api && PORT=7860 node server.js"]
