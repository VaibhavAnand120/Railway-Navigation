# India Rail Navigation System

[![Hugging Face](https://img.shields.io/badge/🤗%20Hugging%20Face-Live%20Demo-blue)](https://huggingface.co/spaces/VaibhavAnand120/india-rail-navigaiton)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

A graph-based real-time railway navigation system for India built with **C++**, **Node.js**, and **Gradio** — deployed on Hugging Face Spaces via Docker.

🔗 **Live Demo:** [https://vaibhavanand120-india-rail-navigaiton.hf.space](https://vaibhavanand120-india-rail-navigaiton.hf.space)

---

## Features

- **Route Search** — Dijkstra's algorithm finds the shortest path between 25 major Indian stations
- **Nearby Stations** — BFS finds all stations reachable within a given distance
- **Alternative Routes** — BFS K-path finder for multiple route options
- **Live Interactive Map** — Leaflet.js map of India updates dynamically on every search

---

## Tech Stack

| Layer | Technology |
|-------|-----------|
| Graph Engine | C++14 — Dijkstra + BFS, adjacency list |
| API Server | Node.js + Express, JSON file storage |
| Web UI | Python + Gradio + FastAPI |
| Map | Leaflet.js + CartoDB dark tiles |
| Deployment | Docker on Hugging Face Spaces |

---

## Project Structure

```
Railway-Navigation/
├── cpp/
│   ├── graph.h          # Graph data structures
│   ├── graph.cpp        # Dijkstra, BFS, fare calculator
│   ├── main.cpp         # CLI interface
│   ├── stations.csv     # 25 major Indian stations
│   └── edges.csv        # Rail connections with distances
├── routes/
│   ├── search.js        # Route search endpoints
│   ├── bookings.js      # Ticket booking
│   └── trains.js        # Train management
├── app.py               # Gradio + FastAPI main app
├── server.js            # Node.js Express API
├── db.js                # JSON file database
├── map.html             # Interactive Leaflet map
├── india.geojson        # India boundary GeoJSON
├── Dockerfile           # Docker deployment config
└── package.json
```

---

## How It Works

```
User Search (Gradio UI)
        ↓
FastAPI (port 7860)
        ↓
Node.js REST API (port 3000)
        ↓
C++ Engine (Dijkstra / BFS)
        ↓
stations.csv + edges.csv
```

- C++ engine is compiled at Docker build time
- Node.js calls the C++ binary via `execFile()`
- Gradio posts map state updates after each search
- Map polls `/static/api/map-state` every second and redraws

---

## Algorithms

- **Dijkstra's Algorithm** — shortest path by distance using a min-heap priority queue
- **BFS (Nearby)** — level-by-level expansion to find stations within km limit
- **BFS K-Paths** — finds K alternate routes while avoiding revisited nodes
- **Graph** — adjacency list with 25 nodes and 50+ bidirectional edges

---

## Stations Covered (25)

Delhi · Mumbai · Chennai · Kolkata · Bangalore · Hyderabad · Ahmedabad · Pune · Jaipur · Lucknow · Bhopal · Nagpur · Patna · Guwahati · Bhubaneswar · Amritsar · Chandigarh · Visakhapatnam · Varanasi · Jammu · Udaipur · Indore · Kanpur · Prayagraj · Jodhpur

---

## Local Setup

### Prerequisites
- g++ (GCC)
- Node.js v18+
- Python 3.9+

### Run locally

```bash
# Clone
git clone https://github.com/VaibhavAnand120/Railway-Navigation.git
cd Railway-Navigation

# Compile C++
cd cpp
g++ -std=c++14 -O2 -o railnav main.cpp graph.cpp
cd ..

# Install Node deps
npm install

# Install Python deps
pip install gradio requests fastapi uvicorn

# Terminal 1 - Start API
node server.js

# Terminal 2 - Start UI
python app.py
```

Open `http://localhost:7860`

---

## Author

**Vaibhav Anand** — [GitHub](https://github.com/VaibhavAnand120)
