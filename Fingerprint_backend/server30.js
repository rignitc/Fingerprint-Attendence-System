const mysql = require('mysql2');
const SerialPort = require('serialport');
const Readline = require('@serialport/parser-readline');
const moment = require('moment-timezone');

// MySQL database configuration
const dbConfig = {
  host: 'localhost',
  user: 'root',
  password: 'root',
  database: 'rig_database',
};

// Create a MySQL connection
const dbConnection = mysql.createConnection(dbConfig);

// Function to handle MySQL connection errors
function handleConnectionError(err) {
  console.error('MySQL connection error:', err);
  if (err.code === 'PROTOCOL_CONNECTION_LOST' || err.code === 'ECONNRESET') {
    dbConnection.connect((err) => {
      if (err) {
        console.error('Error reconnecting to the database:', err);
      } else {
        console.log('Reconnected to the database');
      }
    });
  } else {
    throw err;
  }
}

// Open the MySQL connection
dbConnection.connect((err) => {
  if (err) {
    handleConnectionError(err);
    return;
  }
  console.log('Connected to the database');
});

// Set the timezone to the desired timezone (e.g., 'Asia/Kolkata' for Indian Standard Time)
moment.tz.setDefault('Asia/Kolkata');

// Map to store the last inserted timestamp for each ID
const lastTimestamps = new Map();

// Create a new SerialPort instance to listen for data from Arduino
const port = new SerialPort('/dev/ttyUSB0', { baudRate: 115200 });
const parser = port.pipe(new Readline({ delimiter: '\r\n' }));

// Function to update or insert into main_data table
function updateMainData(id, weekNumber, monthName, yearNumber) {
  const sumQuery = `
    SELECT SEC_TO_TIME(SUM(TIME_TO_SEC(time_difference))) AS weekly_total
    FROM weekly_data
    WHERE id = ? AND week_number = ? AND year_number = ?
  `;
  dbConnection.query(sumQuery, [id, weekNumber, yearNumber], (err, results) => {
    if (err) {
      handleConnectionError(err);
      return;
    }
    if (results.length > 0) {
      const weeklyTotal = results[0].weekly_total;
      console.log(`ID ${id}: weekly total time for week ${weekNumber} is ${weeklyTotal}`);

      // Convert weeklyTotal to hours
      const timeParts = weeklyTotal.split(':');
      const hoursSpent = parseFloat(timeParts[0]) + parseFloat(timeParts[1]) / 60 + parseFloat(timeParts[2]) / 3600;
      const workHoursSatisfied = hoursSpent > 25;

      // Get Name and Batch from enrolled_data
      const enrolledQuery = `
        SELECT Name, Batch
        FROM enrolled_data
        WHERE ID = ?
      `;
      dbConnection.query(enrolledQuery, [id], (err, results) => {
        if (err) {
          handleConnectionError(err);
          return;
        }
        if (results.length > 0) {
          const { Name, Batch } = results[0];

          // Insert or update main_data
          const insertOrUpdateQuery = `
            INSERT INTO main_data (ID, Name, Batch, week_number, month_name, Hours_spent, Work_hours_satisfied, year_number)
            VALUES (?, ?, ?, ?, ?, ?, ?, ?)
            ON DUPLICATE KEY UPDATE
              Hours_spent = VALUES(Hours_spent),
              Work_hours_satisfied = VALUES(Work_hours_satisfied),
              year_number = VALUES(year_number)
          `;
          dbConnection.query(insertOrUpdateQuery, [id, Name, Batch, weekNumber, monthName, weeklyTotal, workHoursSatisfied, yearNumber], (err, results) => {
            if (err) {
              handleConnectionError(err);
              return;
            }
            console.log(`ID ${id}: main_data updated for week ${weekNumber}`);
          });
        }
      });
    }
  });
}

// Function to check and update records with time_in but no time_out after 12 hours
function checkAndUpdateOverdueRecords() {
  const twelveHoursAgo = moment().subtract(12, 'hours').format('YYYY-MM-DD HH:mm:ss');
  const currentTime = moment().format('YYYY-MM-DD HH:mm:ss');

  const checkQuery = `
    SELECT id, time_in
    FROM weekly_data
    WHERE time_out IS NULL AND time_in < ?
  `;
  dbConnection.query(checkQuery, [twelveHoursAgo], (err, results) => {
    if (err) {
      handleConnectionError(err);
      return;
    }
    results.forEach(row => {
      const { id, time_in } = row;
      const duration = moment.duration(moment(currentTime).diff(moment(time_in)));
      const timeDiffSeconds = `${duration.hours().toString().padStart(2, '0')}:${duration.minutes().toString().padStart(2, '0')}:${duration.seconds().toString().padStart(2, '0')}`;
      const weekNumber = moment(time_in).isoWeek();
      const monthName = moment(time_in).format('MMMM');
      const yearNumber = moment(time_in).year();

      const updateQuery = `
        UPDATE weekly_data
        SET time_out = ?,
            time_difference = ?,
            week_number = ?,
            month_name = ?,
            year_number = ?
        WHERE id = ? AND time_in = ? AND time_out IS NULL
      `;
      dbConnection.query(updateQuery, [currentTime, timeDiffSeconds, weekNumber, monthName, yearNumber, id, time_in], (err, results) => {
        if (err) {
          handleConnectionError(err);
          return;
        }
        console.log(`ID ${id}: time_out updated to ${currentTime} after exceeding 12 hours`);
        // Update or insert into main_data table
        updateMainData(id, weekNumber, monthName, yearNumber);
      });
    });
  });
}

// Set an interval to run the checkAndUpdateOverdueRecords function every hour
setInterval(checkAndUpdateOverdueRecords, 60 * 60 * 1000);

// Parse the serial monitor output and process the data
function parseSerialOutput(data) {
  const lines = data.trim().split('\n');
  for (const line of lines) {
    const match = line.match(/Found ID #(\d+) with confidence of \d+/);
    if (match) {
      const id = parseInt(match[1]);
      const currentTime = moment().format('YYYY-MM-DD HH:mm:ss');
      const monthName = moment().format('MMMM'); // Get the full month name
      const weekNumber = moment().isoWeek();
      const yearNumber = moment().year();

      if (lastTimestamps.has(id)) {
        const lastTimestamp = lastTimestamps.get(id);
        const timeIn = moment(lastTimestamp, 'YYYY-MM-DD HH:mm:ss');
        const timeOut = moment(currentTime, 'YYYY-MM-DD HH:mm:ss');
        const duration = moment.duration(timeOut.diff(timeIn));
        const timeDiffSeconds = `${duration.hours().toString().padStart(2, '0')}:${duration.minutes().toString().padStart(2, '0')}:${duration.seconds().toString().padStart(2, '0')}`;

        // Check if a time_in value exists for the current ID
        const checkTimeInQuery = `
          SELECT COUNT(*) AS count
          FROM weekly_data
          WHERE id = ? AND time_in IS NOT NULL AND time_out IS NULL
        `;
        dbConnection.query(checkTimeInQuery, [id], (err, results) => {
          if (err) {
            handleConnectionError(err);
            return;
          }
          // If a time_in value exists, update the time_out value
          if (results[0].count > 0) {
            const updateQuery = `
              UPDATE weekly_data
              SET time_out = ?,
                  time_difference = ?,
                  week_number = ?,
                  month_name = ?,
                  year_number = ?
              WHERE id = ? AND time_in = ? AND time_out IS NULL
            `;
            dbConnection.query(updateQuery, [currentTime, timeDiffSeconds, weekNumber, monthName, yearNumber, id, lastTimestamp], (err, results) => {
              if (err) {
                handleConnectionError(err);
                return;
              }
              console.log(`ID ${id}: time_out updated to ${currentTime}`);

              // Update or insert into main_data table
              updateMainData(id, weekNumber, monthName, yearNumber);
            });

            lastTimestamps.delete(id);
          }
        });
      } else {
        // Check if a record with the same id has a time_in value but no time_out value
        const checkQuery = `
          SELECT COUNT(*) AS count
          FROM weekly_data
          WHERE id = ? AND time_in IS NOT NULL AND time_out IS NULL
        `;
        dbConnection.query(checkQuery, [id], (err, results) => {
          if (err) {
            handleConnectionError(err);
            return;
          }
          // If such a record exists, update its time_out value
          if (results[0].count > 0) {
            const timeInQuery = `
              SELECT time_in
              FROM weekly_data
              WHERE id = ? AND time_in IS NOT NULL AND time_out IS NULL
            `;
            dbConnection.query(timeInQuery, [id], (err, results) => {
              if (err) {
                handleConnectionError(err);
                return;
              }
              if (results.length > 0) {
                const timeIn = results[0].time_in;
                const duration = moment.duration(moment(currentTime).diff(moment(timeIn)));
                const timeDiffSeconds = `${duration.hours().toString().padStart(2, '0')}:${duration.minutes().toString().padStart(2, '0')}:${duration.seconds().toString().padStart(2, '0')}`;

                const updateQuery = `
                  UPDATE weekly_data
                  SET time_out = ?,
                      time_difference = ?,
                      week_number = ?,
                      month_name = ?,
                      year_number = ?
                  WHERE id = ? AND time_in IS NOT NULL AND time_out IS NULL
                `;
                dbConnection.query(updateQuery, [currentTime, timeDiffSeconds, weekNumber, monthName, yearNumber, id], (err, results) => {
                  if (err) {
                    handleConnectionError(err);
                    return;
                  }
                  console.log(`ID ${id}: time_out updated to ${currentTime}`);
                  lastTimestamps.delete(id);
                });
              }
            });
          } else {
            // If no such record exists, insert a new one
            const insertQuery = `
              INSERT INTO weekly_data (id, time_in, week_number, month_name, year_number) VALUES (?, ?, ?, ?, ?)
            `;
            dbConnection.query(insertQuery, [id, currentTime, weekNumber, monthName, yearNumber], (err, results) => {
              if (err) {
                handleConnectionError(err);
                return;
              }
              console.log(`ID ${id}: time_in inserted at ${currentTime}`);
              lastTimestamps.set(id, currentTime);
            });
          }
        });
      }
    }
  }
}

// Listen for data from the Arduino and pass it to the parseSerialOutput function
parser.on('data', (data) => {
  parseSerialOutput(data);
});
