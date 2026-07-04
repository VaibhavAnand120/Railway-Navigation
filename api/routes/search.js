const express = require('express');
const router = express.Router();
const { exec } = require('child_process');
const path = require('path');
const { data, save } = require('../db');

const CPP_DIR = path.join(__dirname, '..', '..', 'cpp');
const isWindows = process.platform === 'win32';
const CPP_BIN = isWindows ? 'railnav.exe' : './railnav';

function runCpp(args) {
  return new Promise((resolve, reject) => {
    const cmd = `${CPP_BIN} ${args}`;
    exec(cmd, { cwd: CPP_DIR }, (err, stdout, stderr) => {
      if (err) return reject(stderr || err.message);
      try {
        resolve(JSON.parse(stdout.trim()));
      } catch(e) {
        reject('Invalid output: ' + stdout);
      }
    });
  });
}

router.get('/route', async (req, res) => {
  const { from, to } = req.query;
  const travelClass = req.query.class || 'SL';
  if (!from || !to) return res.status(400).json({ error: 'from and to required' });

  try {
    const result = await runCpp(`route "${from}" "${to}" "${travelClass}"`);
    data.route_history.unshift({
      id: data.nextHistoryId++,
      from_station: from, to_station: to,
      distance: result.distance, duration: result.duration,
      fare: result.fare, travel_class: travelClass,
      searched_at: new Date().toISOString()
    });
    if (data.route_history.length > 50) data.route_history.pop();
    save();
    res.json(result);
  } catch(e) {
    res.status(500).json({ error: e.toString() });
  }
});

router.get('/nearby', async (req, res) => {
  const { station, maxDistance = 300 } = req.query;
  if (!station) return res.status(400).json({ error: 'station required' });
  try {
    const result = await runCpp(`nearby "${station}" ${maxDistance}`);
    res.json(result);
  } catch(e) {
    res.status(500).json({ error: e.toString() });
  }
});

router.get('/alternatives', async (req, res) => {
  const { from, to, k = 3 } = req.query;
  if (!from || !to) return res.status(400).json({ error: 'from and to required' });
  try {
    const result = await runCpp(`alternatives "${from}" "${to}" ${k}`);
    res.json(result);
  } catch(e) {
    res.status(500).json({ error: e.toString() });
  }
});

router.get('/fare', async (req, res) => {
  const { distance } = req.query;
  const travelClass = req.query.class || 'SL';
  if (!distance) return res.status(400).json({ error: 'distance required' });
  try {
    const result = await runCpp(`fare ${distance} ${travelClass}`);
    res.json(result);
  } catch(e) {
    res.status(500).json({ error: e.toString() });
  }
});

router.get('/stations', async (req, res) => {
  try {
    const result = await runCpp('stations');
    res.json(result);
  } catch(e) {
    res.status(500).json({ error: e.toString() });
  }
});

router.get('/edges', (req, res) => {
  const fs = require('fs');
  const csvPath = path.join(CPP_DIR, 'edges.csv');
  try {
    const content = fs.readFileSync(csvPath, 'utf8');
    const lines = content.trim().split('\n');
    const edges = [];
    for (let i = 1; i < lines.length; i++) {
      const line = lines[i].trim();
      if (!line) continue;
      const parts = line.split(',');
      if (parts.length >= 2) {
        edges.push({
          from: parseInt(parts[0]),
          to: parseInt(parts[1]),
          distance: parseInt(parts[2]),
          duration: parseInt(parts[3]),
          fare: parseFloat(parts[4]),
          trainId: parts[5]
        });
      }
    }
    res.json({ edges });
  } catch (e) {
    res.status(500).json({ error: e.toString() });
  }
});

router.get('/history', (req, res) => {
  res.json({ history: data.route_history.slice(0, 50) });
});

module.exports = router;
