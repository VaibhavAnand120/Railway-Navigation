import gradio as gr
import requests
import subprocess
import os
import time
from fastapi import FastAPI, Request
from fastapi.responses import HTMLResponse, JSONResponse, FileResponse
import uvicorn

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
CPP_DIR  = os.path.join(BASE_DIR, "cpp")
API_BASE = "http://localhost:3000/api"

# ── Startup ────────────────────────────────────────────────────────────────────
def compile_cpp():
    binary = os.path.join(CPP_DIR, "railnav")
    if os.path.exists(binary):
        print("C++ binary already compiled.")
        return
    print("Compiling C++ engine...")
    result = subprocess.run(
        ["g++", "-std=c++14", "-O2", "-o", "railnav", "main.cpp", "graph.cpp"],
        cwd=CPP_DIR, capture_output=True, text=True
    )
    if result.returncode != 0:
        print("C++ compilation failed:", result.stderr)
    else:
        print("C++ engine compiled.")

def start_node():
    subprocess.run(["npm", "install"], cwd=BASE_DIR, capture_output=True)
    subprocess.Popen(["node", "server.js"], cwd=BASE_DIR,
                     stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    time.sleep(3)
    print("Node.js API started on port 3000.")

compile_cpp()
start_node()

# ── Station data ───────────────────────────────────────────────────────────────
STATIONS = [
    "New Delhi", "Mumbai Central", "Chennai Central", "Howrah Junction",
    "Bangalore City", "Hyderabad Deccan", "Ahmedabad Junction", "Pune Junction",
    "Jaipur Junction", "Lucknow", "Bhopal Junction", "Nagpur Junction",
    "Patna Junction", "Guwahati", "Bhubaneswar", "Amritsar Junction",
    "Chandigarh", "Visakhapatnam", "Varanasi Junction", "Jammu Tawi",
    "Udaipur City", "Indore Junction", "Kanpur Central", "Allahabad Junction",
    "Jodhpur Junction"
]

def update_map(highlighted=None, path=None, paths=None, nearby=None):
    try:
        requests.post(f"{API_BASE}/map-state", json={
            "highlighted": highlighted or [],
            "path": path or [],
            "paths": paths or [],
            "nearby": nearby or []
        }, timeout=3)
    except:
        pass

def search_route(from_station, to_station):
    if not from_station or not to_station:
        return "Please select both stations."
    if from_station == to_station:
        return "Source and destination cannot be the same."
    try:
        r = requests.get(f"{API_BASE}/search/route", params={
            "from": from_station, "to": to_station, "class": "SL"
        }, timeout=10)
        data = r.json()
        if "error" in data:
            return f"Error: {data['error']}"
        path = data.get("path", [])
        update_map(highlighted=[from_station, to_station], path=path)
        return (
            f"Route:     {' → '.join(path)}\n"
            f"Distance:  {data.get('distance', 'N/A')} km"
        )
    except Exception as e:
        return f"Connection error: {str(e)}"

def find_nearby(station, max_dist):
    try:
        r = requests.get(f"{API_BASE}/search/nearby", params={
            "station": station, "maxDistance": int(max_dist)
        }, timeout=10)
        data = r.json()
        nearby = data.get("nearby", [])
        update_map(highlighted=[station], nearby=nearby)
        if not nearby:
            return f"No stations within {int(max_dist)} km of {station}."
        return "\n".join(f"• {s}" for s in nearby)
    except Exception as e:
        return f"Error: {str(e)}"

def find_alternatives(from_station, to_station, k):
    try:
        r = requests.get(f"{API_BASE}/search/alternatives", params={
            "from": from_station, "to": to_station, "k": int(k)
        }, timeout=10)
        data = r.json()
        routes = data.get("routes", [])
        if not routes:
            return "No alternative routes found."
        result = ""
        for i, route in enumerate(routes):
            path_str = " → ".join(route.get("path", []))
            dist = route.get("totalDistance") or route.get("distance", "N/A")
            result += f"Route {i+1}: {path_str}\n  Distance: {dist} km\n\n"
        update_map(
            highlighted=[from_station, to_station],
            paths=[r.get("path", []) for r in routes]
        )
        return result.strip()
    except Exception as e:
        return f"Error: {str(e)}"

# ── Gradio UI ──────────────────────────────────────────────────────────────────
css = """
body, .gradio-container { background: #0f1117 !important; font-family: Inter, sans-serif !important; }
.gradio-container { max-width: 750px !important; margin: 0 auto !important; padding: 2rem !important; }
label { color: #9ca3af !important; font-size: 0.85rem !important; font-weight: 500 !important; }
button.primary { background: #4f46e5 !important; border: none !important; border-radius: 8px !important; color: white !important; font-weight: 600 !important; }
textarea { background: #12151e !important; border: 1px solid #2a2d3a !important; border-radius: 8px !important; color: #e5e7eb !important; }
.tab-nav button { color: #6b7280 !important; }
.tab-nav button.selected { color: #4f46e5 !important; border-bottom: 2px solid #4f46e5 !important; }
input[type=range] { accent-color: #4f46e5 !important; }
footer { display: none !important; }
"""

with gr.Blocks(title="India Rail Navigation") as gradio_app:
    gr.HTML("""
    <h1 style='font-family:Inter,sans-serif;color:#fff;font-size:1.8rem;font-weight:600;margin-bottom:0.25rem'>
        India Rail Navigation
    </h1>
    <p style='font-family:Inter,sans-serif;color:#6b7280;margin-bottom:1rem'>
        Graph-based route finder using Dijkstra &amp; BFS across 25 major Indian stations
    </p>
    <p style='font-family:Inter,sans-serif;font-size:0.85rem;margin-bottom:1.5rem'>
        <a href='javascript:window.open(window.location.origin+"/static/map","_blank")'
           style='color:#4f46e5;text-decoration:none;font-weight:500;
                  background:#1e1b4b;padding:6px 14px;border-radius:6px;cursor:pointer;'>
            🗺️ Open Live Map
        </a>
    </p>
    """)

    with gr.Tabs():
        with gr.TabItem("Route Search"):
            with gr.Row():
                from_st = gr.Dropdown(STATIONS, label="From", value="New Delhi", scale=1)
                to_st   = gr.Dropdown(STATIONS, label="To", value="Mumbai Central", scale=1)
            search_btn   = gr.Button("Find Route", variant="primary")
            route_result = gr.Textbox(label="Result", lines=3, interactive=False)
            search_btn.click(search_route, [from_st, to_st], route_result)

        with gr.TabItem("Nearby Stations"):
            with gr.Row():
                nb_station = gr.Dropdown(STATIONS, label="Station", value="New Delhi", scale=3)
                nb_dist    = gr.Slider(50, 1000, value=400, step=50, label="Max km", scale=4)
            nb_btn    = gr.Button("Search", variant="primary")
            nb_result = gr.Textbox(label="Nearby stations", lines=5, interactive=False)
            nb_btn.click(find_nearby, [nb_station, nb_dist], nb_result)

        with gr.TabItem("Alternatives"):
            with gr.Row():
                alt_from = gr.Dropdown(STATIONS, label="From", value="New Delhi", scale=2)
                alt_to   = gr.Dropdown(STATIONS, label="To", value="Chennai Central", scale=2)
                alt_k    = gr.Slider(1, 5, value=3, step=1, label="Routes", scale=1)
            alt_btn    = gr.Button("Search", variant="primary")
            alt_result = gr.Textbox(label="Routes", lines=6, interactive=False)
            alt_btn.click(find_alternatives, [alt_from, alt_to, alt_k], alt_result)

# ── FastAPI with Gradio at root ────────────────────────────────────────────────
app = FastAPI()

# Custom routes BEFORE mounting Gradio
@app.get("/static/map", response_class=HTMLResponse)
async def serve_map():
    with open(os.path.join(BASE_DIR, "map.html"), "r") as f:
        return f.read()

@app.get("/static/india.geojson")
async def serve_geojson():
    return FileResponse(os.path.join(BASE_DIR, "india.geojson"),
                        media_type="application/json")

@app.get("/static/api/map-state")
async def get_map_state():
    r = requests.get(f"{API_BASE}/map-state", timeout=3)
    return JSONResponse(r.json())

@app.post("/static/api/map-state")
async def set_map_state(request: Request):
    body = await request.json()
    r = requests.post(f"{API_BASE}/map-state", json=body, timeout=3)
    return JSONResponse(r.json())

@app.get("/static/api/search/stations")
async def proxy_stations():
    r = requests.get(f"{API_BASE}/search/stations", timeout=10)
    return JSONResponse(r.json())

@app.get("/static/api/search/edges")
async def proxy_edges():
    r = requests.get(f"{API_BASE}/search/edges", timeout=10)
    return JSONResponse(r.json())

# Mount Gradio at root LAST
app = gr.mount_gradio_app(app, gradio_app, path="/")

if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=7860)
