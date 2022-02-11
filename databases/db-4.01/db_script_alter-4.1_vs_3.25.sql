SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='TRADITIONAL';

DROP SCHEMA IF EXISTS `default_schema` ;

USE `bugware0`;

ALTER TABLE `bugware0`.`pci_all` DROP FOREIGN KEY `fk_pci_all_pci_subsystems1` ;

ALTER TABLE `bugware0`.`pci_all` 
  ADD CONSTRAINT `pci_all_ibfk_1`
  FOREIGN KEY (`uPcisubID_FK` )
  REFERENCES `bugware0`.`pci_subsystems` (`uPcisubID` )
  ON DELETE NO ACTION
  ON UPDATE CASCADE
, ADD INDEX `uPcisubID_FK` (`uPcisubID_FK` ASC) 
, DROP INDEX `fk_pci_all_pci_subsystems1` ;

DELIMITER $$

-- -----------------------------------------------------
-- procedure sp_pciProgifsAdd
-- -----------------------------------------------------
DROP procedure IF EXISTS `bugware0`.`sp_pciProgifsAdd` $$
CREATE DEFINER=`root`@`localhost` PROCEDURE `sp_pciProgifsAdd`(
IN class_code CHAR(2), IN subclass_code CHAR(2), IN progif_code CHAR(2), IN progif_name VARCHAR(100))
BEGIN

DECLARE subclass_id TINYINT UNSIGNED;
DECLARE turn_back TINYINT(1) DEFAULT 0;
DECLARE CONTINUE HANDLER FOR SQLEXCEPTION, SQLWARNING, NOT FOUND BEGIN 
SET turn_back=1;
END;

START TRANSACTION;

SELECT subClassID INTO subclass_id FROM vw_pciClassList WHERE classCode=class_code AND subClassCode=subclass_code;

INSERT INTO pci_prog_ifs (progifCode, progifName, subClassID_FK) VALUES ( progif_code, progif_name, subclass_id );

IF turn_back=0 THEN
	COMMIT;
	select LAST_INSERT_ID();
ELSE
	rollback;
	select 'error occured';
END IF;

END$$

-- -----------------------------------------------------
-- procedure sp_pci_SubsystemsAdd
-- -----------------------------------------------------
DROP procedure IF EXISTS `bugware0`.`sp_pci_SubsystemsAdd` $$
CREATE DEFINER=`root`@`localhost` PROCEDURE `sp_pci_SubsystemsAdd`(
IN vendor_code CHAR(4), IN device_code CHAR(4), IN subvendor_code CHAR(4), IN subdevice_code CHAR(4), IN subsys_name VARCHAR(150))
BEGIN

-- `device_id` type must match the `deviceID` in `pci_devices` table
DECLARE device_id INT UNSIGNED;
DECLARE turn_back TINYINT(1) DEFAULT 0;
DECLARE CONTINUE HANDLER FOR SQLEXCEPTION, SQLWARNING, NOT FOUND BEGIN 
SET turn_back=1;
END;

START TRANSACTION;

-- The stupid thing below costed me a week because of returning more than one row, so I had to LIMIT 1
SELECT deviceID INTO device_id FROM vw_pciDeviceList WHERE vendorCode=vendor_code AND deviceCode=device_code LIMIT 1;

INSERT INTO pci_subsystems (subvendor, subdevice, subSysName, deviceID_FK) VALUES ( subvendor_code, subdevice_code, subsys_name, device_id );

IF turn_back=0 THEN
	COMMIT;
	select 'well done', device_id;
-- 	select LAST_INSERT_ID();
ELSE
	rollback;
--	select 'error occured', device_id;
END IF;
END$$

DELIMITER ;


SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;
