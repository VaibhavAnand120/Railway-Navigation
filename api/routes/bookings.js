const express = require('express');
const router = express.Router();
const { data, save } = require('../db');

function generatePNR() {
  return Math.random().toString(36).substring(2, 12).toUpperCase();
}

// POST /api/bookings
router.post('/', (req, res) => {
  const { from_station, to_station, travel_class, passenger_name, passenger_age, fare, distance, train_id, journey_date } = req.body;
  if (!from_station || !to_station || !passenger_name || !fare || !journey_date) {
    return res.status(400).json({ error: 'Missing required fields' });
  }

  // Check seat availability
  if (train_id) {
    const train = data.trains.find(t => t.id === train_id);
    if (train && train.available_seats <= 0) {
      return res.status(400).json({ error: 'No seats available' });
    }
    if (train) train.available_seats--;
  }

  const booking = {
    id: data.nextBookingId++,
    pnr: generatePNR(),
    from_station, to_station,
    travel_class: travel_class || 'SL',
    passenger_name,
    passenger_age: passenger_age || null,
    fare, distance, train_id,
    journey_date,
    status: 'CONFIRMED',
    created_at: new Date().toISOString()
  };

  data.bookings.push(booking);
  save();
  res.json({ success: true, pnr: booking.pnr, message: 'Booking confirmed!' });
});

// GET /api/bookings/pnr/:pnr
router.get('/pnr/:pnr', (req, res) => {
  const booking = data.bookings.find(b => b.pnr === req.params.pnr);
  if (!booking) return res.status(404).json({ error: 'PNR not found' });
  res.json(booking);
});

// GET /api/bookings
router.get('/', (req, res) => {
  res.json({ bookings: [...data.bookings].reverse() });
});

// DELETE /api/bookings/:pnr
router.delete('/:pnr', (req, res) => {
  const booking = data.bookings.find(b => b.pnr === req.params.pnr);
  if (!booking) return res.status(404).json({ error: 'PNR not found' });
  if (booking.status === 'CANCELLED') return res.status(400).json({ error: 'Already cancelled' });

  booking.status = 'CANCELLED';
  if (booking.train_id) {
    const train = data.trains.find(t => t.id === booking.train_id);
    if (train) train.available_seats++;
  }
  save();
  res.json({ success: true, message: 'Booking cancelled' });
});

module.exports = router;
