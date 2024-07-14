INSTALL AND SETUP MYSQL 8.0
USE MYSQL-CONNECTOR-PYTHON TO CATCH SHA2_PASSWORD
OPEN MYSQL CLI AND COPY PASTE THESE IN ORDER
CHANGE THE ARGUMENT OF MYSQL CONNECTION OBJECT
-- -----------------------------------------------------
-- Database `ATTENDENCE_DATA`.
-- -----------------------------------------------------
CREATE DATABASE ATTENDENCE_DATA;
-- -----------------------------------------------------
-- Table `ATTENDENCE_DATA`.`enrolled_data`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `ATTENDENCE_DATA`.`enrolled_data` (
  `ID` INT NOT NULL,
  `Name` VARCHAR(45) NOT NULL,
  `Batch` VARCHAR(45) NOT NULL,
  PRIMARY KEY (`ID`, `Name`, `Batch`),
  UNIQUE INDEX `ID_UNIQUE` (`ID` ASC) VISIBLE,
  UNIQUE INDEX `Name_UNIQUE` (`Name` ASC) VISIBLE,
  INDEX `Batch_idx` (`Batch` ASC) VISIBLE)
  ENGINE = InnoDB;

  -- -----------------------------------------------------
-- Table `ATTENDENCE_DATA`.`weekly_data`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `ATTENDENCE_DATA`.`weekly_data` (
  `ID` INT NOT NULL,
  `time_in` INT NOT NULL,
  `time_out` INT NOT NULL,
  `time_difference` INT NOT NULL,
  `week_number` INT NOT NULL,
  `month_name` VARCHAR(45) NOT NULL,
  `year_number` INT NOT NULL,
  PRIMARY KEY (`ID`,`time_difference`,`week_number`,`month_name`,`year_number`),
  CONSTRAINT `ID_FK`
  FOREIGN KEY (`ID`)
    REFERENCES `ATTENDENCE_DATA`.`ENROLLED_DATA` (`ID`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `ATTENDENCE_DATA`.`main_data`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `ATTENDENCE_DATA`.`main_data` (
  `Work_hours_satisfied` VARCHAR(5) NOT NULL DEFAULT 'TRUE',
  `ID` INT NOT NULL,
  `Name` VARCHAR(45) NOT NULL,
  `Batch` VARCHAR(45) NOT NULL,
  `Week No` VARCHAR(45) NOT NULL,
  `Month` VARCHAR(45) NOT NULL,
  `Hours Spent` INT NOT NULL DEFAULT '0',
  `Year` INT NOT NULL DEFAULT '2024',
  INDEX `Name_idx` (`Name` ASC) VISIBLE,
  INDEX `Batch_idx` (`Batch` ASC) VISIBLE,
  CONSTRAINT `ID_FK1`
    FOREIGN KEY (`ID`)
    REFERENCES `ATTENDENCE_DATA`.`ENROLLED_DATA` (`ID`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `Name_FK`
    FOREIGN KEY (`Name`)
    REFERENCES `ATTENDENCE_DATA`.`ENROLLED_DATA` (`Name`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `Batch_FK`
    FOREIGN KEY (`Batch`)
    REFERENCES `ATTENDENCE_DATA`.`ENROLLED_DATA` (`Batch`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;

-- -----------------------------------------------------
-- Dummy Data for Table `ATTENDENCE_DATA`.`main_data`
-- -----------------------------------------------------
INSERT INTO `ATTENDENCE_DATA`.`main_data` (`Work_hours_satisfied`, `ID`, `Name`, `Batch`, `Week No`, `Month`, `Hours Spent`, `Year`)
VALUES 
('TRUE', 1, 'RASHITH', 'B22', 'Week2', 'February', 26, 2024),
('FALSE', 1, 'RASHITH', 'B22', 'Week3', 'March', 24, 2024),
('TRUE', 2, 'KRISHNACHANDRAN', 'B22', 'Week2', 'February', 27, 2024),
('FALSE', 2, 'KRISHNACHANDRAN', 'B22', 'Week3', 'March', 24, 2024),
('TRUE', 3, 'SHIBIN', 'B21', 'Week2', 'February', 28, 2024),
('FALSE', 3, 'SHIBIN', 'B21', 'Week3', 'March', 24, 2024),
('TRUE', 4, 'JERIN', 'B22', 'Week2', 'February', 29, 2024),
('FALSE', 4, 'JERIN', 'B22', 'Week3', 'March', 24, 2024),
('TRUE', 5, 'ARJUN', 'B20', 'Week2', 'February', 30, 2024),
('FALSE', 5, 'ARJUN', 'B20', 'Week3', 'March', 24, 2024);