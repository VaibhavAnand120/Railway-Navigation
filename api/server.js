const express = require('express');
const cors = require('cors');
const path = require('path');
const { createProxyMiddleware } = require('http-proxy-middleware');
const app = express();
const PORT = process.env.PORT || 3000;

app.use(cors());
app.use(express.json());
app.use(express.static(path.join(__dirname, 'public')));

// In-memory map state storage
let mapState = {
  highlighted: [],
  path: [],
  paths: [],
  nearby: []
};

// Routes
app.use('/api/search', require('./routes/search'));
app.use('/api/bookings', require('./routes/bookings'));
app.use('/api/trains', require('./routes/trains'));

// Map state endpoints
app.post('/api/map-state', (req, res) => {
  mapState = {
    highlighted: req.body.highlighted || [],
    path: req.body.path || [],
    paths: req.body.paths || [],
    nearby: req.body.nearby || []
  };
  res.json({ success: true });
});

app.get('/api/map-state', (req, res) => {
  res.json(mapState);
});

// View map route
app.get('/map', (req, res) => {
  res.sendFile(path.join(__dirname, 'public', 'map.html'));
});

// Health check
app.get('/api/health', (req, res) => {
  res.json({ status: 'ok', message: 'Rail Navigation API running' });
});

// Fallback reverse proxy to Gradio UI running internally on port 8000
app.use('/', createProxyMiddleware({
  target: 'http://127.0.0.1:8000',
  changeOrigin: true,
  ws: true,
  logLevel: 'silent'
}));

app.listen(PORT, () => {
  console.log(`Rail Navigation API running on http://localhost:${PORT}`);
});

module.exports = app;
