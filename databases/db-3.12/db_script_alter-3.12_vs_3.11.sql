SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='TRADITIONAL';

ALTER TABLE `bugware0`.`pci_devices` DROP FOREIGN KEY `fk_pci_devices_pci_vendors` ;

ALTER TABLE `bugware0`.`pci_devices` 
  ADD CONSTRAINT `fk_pci_devices_pci_vendors`
  FOREIGN KEY (`vendorCode_FK` )
  REFERENCES `bugware0`.`pci_vendors` (`vendorCode` )
  ON DELETE NO ACTION
  ON UPDATE CASCADE;

ALTER TABLE `bugware0`.`pci_subsystems` DROP FOREIGN KEY `fk_pci_subsystems_pci_devices1` ;

ALTER TABLE `bugware0`.`pci_subsystems` 
  ADD CONSTRAINT `fk_pci_subsystems_pci_devices1`
  FOREIGN KEY (`deviceID_FK` )
  REFERENCES `bugware0`.`pci_devices` (`deviceID` )
  ON DELETE NO ACTION
  ON UPDATE CASCADE;

USE `bugware0`;

DELIMITER $$

CREATE PROCEDURE `bugware0`.`sp_pci_DevicesAdd` (
IN vendor_code CHAR(4), IN device_code CHAR(4), IN device_name VARCHAR(150))
BEGIN

DECLARE turn_back TINYINT(1) DEFAULT 0;
DECLARE CONTINUE HANDLER FOR SQLEXCEPTION, SQLWARNING, NOT FOUND BEGIN 
SET turn_back=1;
END;

START TRANSACTION;
-- some statement
INSERT INTO pci_devices (deviceCode, deviceName, vendorCode_FK) VALUES (device_code, device_name, vendor_code);


IF turn_back=0 THEN
	COMMIT;
	select LAST_INSERT_ID();
ELSE
	rollback;
	select 'error occured';
END IF;

END$$

CREATE DEFINER=`root`@`localhost` PROCEDURE `bugware0`.`sp_pci_VendorsAdd`(IN vendor_code CHAR(4), IN vendor_name VARCHAR(100))
BEGIN

declare turn_back TINYINT(1) default 0;
declare CONTINUE handler for sqlexception, sqlwarning, not found BEGIN 
SET turn_back=1;
END;

START TRANSACTION;
-- some statement
INSERT INTO pci_vendors VALUES (vendor_code, vendor_name);


IF turn_back=0 THEN
	COMMIT;
	select 'well done';
ELSE
	rollback;
	select 'error occured';
END IF;

END$$

DELIMITER ;

SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;
