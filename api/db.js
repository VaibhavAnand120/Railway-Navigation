const path = require('path');
const fs = require('fs');

const DB_PATH = path.join(__dirname, 'railnav.json');

let data = {
  bookings: [],
  trains: [],
  route_history: [],
  nextBookingId: 1,
  nextHistoryId: 1
};

if (fs.existsSync(DB_PATH)) {
  try {
    data = JSON.parse(fs.readFileSync(DB_PATH, 'utf8'));
  } catch(e) {
    console.log('Fresh database created');
  }
}

function save() {
  fs.writeFileSync(DB_PATH, JSON.stringify(data, null, 2));
}

if (data.trains.length === 0) {
  data.trains = [
    { id: '12951', name: 'Rajdhani Express', from_station: 'New Delhi', to_station: 'Mumbai Central', departure: '16:55', arrival: '08:35', distance: 1384, available_seats: 80, fare_sl: 755, fare_3a: 1995, fare_2a: 2850, fare_1a: 4785 },
    { id: '12301', name: 'Howrah Rajdhani', from_station: 'New Delhi', to_station: 'Howrah Junction', departure: '16:55', arrival: '09:55', distance: 1453, available_seats: 95, fare_sl: 810, fare_3a: 2145, fare_2a: 3065, fare_1a: 5145 },
    { id: '12615', name: 'Grand Trunk Express', from_station: 'New Delhi', to_station: 'Chennai Central', departure: '18:30', arrival: '06:30', distance: 2180, available_seats: 60, fare_sl: 1200, fare_3a: 3175, fare_2a: 4535, fare_1a: 7615 },
    { id: '12691', name: 'Chennai-Bangalore Exp', from_station: 'Chennai Central', to_station: 'Bangalore City', departure: '06:10', arrival: '11:45', distance: 362, available_seats: 120, fare_sl: 210, fare_3a: 555, fare_2a: 795, fare_1a: 1335 },
    { id: '12723', name: 'Telangana Express', from_station: 'New Delhi', to_station: 'Hyderabad Deccan', departure: '06:25', arrival: '10:05', distance: 1563, available_seats: 75, fare_sl: 870, fare_3a: 2300, fare_2a: 3285, fare_1a: 5520 },
    { id: '12015', name: 'Ajmer Shatabdi', from_station: 'New Delhi', to_station: 'Jaipur Junction', departure: '06:05', arrival: '10:40', distance: 308, available_seats: 150, fare_sl: 175, fare_3a: 465, fare_2a: 665, fare_1a: 1115 },
    { id: '14033', name: 'Jammu Tawi Express', from_station: 'New Delhi', to_station: 'Jammu Tawi', departure: '21:40', arrival: '05:45', distance: 577, available_seats: 90, fare_sl: 330, fare_3a: 875, fare_2a: 1250, fare_1a: 2100 },
  ];
  save();
}

module.exports = { data, save };
