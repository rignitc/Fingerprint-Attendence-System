const xlsx = require('xlsx');
const mysql = require('mysql2/promise');

// Configuration for the MariaDB connection
const dbConfig = {
  host: 'localhost',
  user: 'root',
  password: '7square',
  database: 'mydb',
};

// Path to your Excel file
const excelFilePath = 'Rig_FingerPrint_ID_list.xlsx';

async function insertDataFromExcel() {
  try {
    // Read the Excel file
    const workbook = xlsx.readFile(excelFilePath);
    const sheetName = workbook.SheetNames[0]; // Assuming data is in the first sheet
    const worksheet = workbook.Sheets[sheetName];

    // Convert the sheet to JSON
    const data = xlsx.utils.sheet_to_json(worksheet);

    // Create a connection to the database
    const connection = await mysql.createConnection(dbConfig);

    // Loop through each row and insert into the database
    for (let row of data) {
      const { name, batch, id } = row;

      // Insert query
      const query = 'INSERT INTO enrolled_data (name, batch, id) VALUES (?, ?, ?)';
      const values = [name, batch, id];

      await connection.execute(query, values);
    }

    console.log('Data successfully inserted into the database.');
    
    // Close the connection
    await connection.end();
  } catch (error) {
    console.error('Error:', error.message);
  }
}

// Execute the function
insertDataFromExcel();
