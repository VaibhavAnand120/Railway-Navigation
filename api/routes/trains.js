const express = require('express');
const router = express.Router();
const { data, save } = require('../db');

router.get('/', (req, res) => {
  res.json({ trains: data.trains });
});

router.get('/:id', (req, res) => {
  const train = data.trains.find(t => t.id === req.params.id);
  if (!train) return res.status(404).json({ error: 'Train not found' });
  res.json(train);
});

router.post('/', (req, res) => {
  const { id, name, from_station, to_station, departure, arrival, distance, available_seats, fare_sl, fare_3a, fare_2a, fare_1a } = req.body;
  if (!id || !name || !from_station || !to_station) {
    return res.status(400).json({ error: 'Missing required fields' });
  }
  if (data.trains.find(t => t.id === id)) {
    return res.status(400).json({ error: 'Train ID already exists' });
  }
  data.trains.push({ id, name, from_station, to_station, departure, arrival, distance, available_seats: available_seats || 100, fare_sl, fare_3a, fare_2a, fare_1a });
  save();
  res.json({ success: true, message: 'Train added' });
});

router.put('/:id/seats', (req, res) => {
  const train = data.trains.find(t => t.id === req.params.id);
  if (!train) return res.status(404).json({ error: 'Train not found' });
  train.available_seats = req.body.available_seats;
  save();
  res.json({ success: true });
});

router.delete('/:id', (req, res) => {
  data.trains = data.trains.filter(t => t.id !== req.params.id);
  save();
  res.json({ success: true, message: 'Train removed' });
});

module.exports = router;
