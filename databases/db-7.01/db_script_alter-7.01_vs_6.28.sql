SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='TRADITIONAL';

ALTER TABLE `bugware0`.`all_devices` CHANGE COLUMN `busTypeID_FK` `busTypeID_FK` TINYINT(3) UNSIGNED NOT NULL DEFAULT '1'  ;


-- -----------------------------------------------------
-- Placeholder table for view `bugware0`.`vw_pci_devices`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `bugware0`.`vw_pci_devices` (`pciSpcID` INT, `vendorCode` INT, `deviceCode` INT, `subvendor` INT, `subdevice` INT, `classCode` INT, `subClassCode` INT, `progifCode` INT);



USE `bugware0`;

-- -----------------------------------------------------
-- View `bugware0`.`vw_pci_devices`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `bugware0`.`vw_pci_devices`;
CREATE  OR REPLACE ALGORITHM=UNDEFINED DEFINER=`root`@`localhost` SQL SECURITY DEFINER VIEW `bugware0`.`vw_pci_devices` AS select `bugware0`.`pci_all`.`pciSpcID` AS `pciSpcID`,`bugware0`.`pci_vendors`.`vendorCode` AS `vendorCode`,`bugware0`.`pci_devices`.`deviceCode` AS `deviceCode`,`bugware0`.`pci_subsystems`.`subvendor` AS `subvendor`,`bugware0`.`pci_subsystems`.`subdevice` AS `subdevice`,`bugware0`.`pci_classes`.`classCode` AS `classCode`,`bugware0`.`pci_subclasses`.`subClassCode` AS `subClassCode`,`bugware0`.`pci_prog_ifs`.`progifCode` AS `progifCode` from ((((((`bugware0`.`pci_all` join `bugware0`.`pci_prog_ifs` on((`bugware0`.`pci_all`.`uProgifID_FK` = `bugware0`.`pci_prog_ifs`.`uProgifID`))) join `bugware0`.`pci_subclasses` on((`bugware0`.`pci_prog_ifs`.`subClassID_FK` = `bugware0`.`pci_subclasses`.`subClassID`))) join `bugware0`.`pci_classes` on((`bugware0`.`pci_subclasses`.`classCode_FK` = `bugware0`.`pci_classes`.`classCode`))) join `bugware0`.`pci_subsystems` on((`bugware0`.`pci_all`.`uPcisubID_FK` = `bugware0`.`pci_subsystems`.`uPcisubID`))) join `bugware0`.`pci_devices` on((`bugware0`.`pci_subsystems`.`deviceID_FK` = `bugware0`.`pci_devices`.`deviceID`))) join `bugware0`.`pci_vendors` on((`bugware0`.`pci_devices`.`vendorCode_FK` = `bugware0`.`pci_vendors`.`vendorCode`)));

DELIMITER $$

USE `bugware0`;
DROP procedure IF EXISTS `bugware0`.`sp_queryClassName`;
$$

USE `bugware0`$$
CREATE DEFINER=`root`@`localhost` PROCEDURE `sp_queryClassName`(
IN class_code CHAR(2), IN subclass_code CHAR(2), IN progif_code CHAR(2) )
BEGIN
DECLARE findings TINYINT;
-- DECLARE class_code CHAR(6);

IF progif_code='' THEN
	SET progif_code = '00';
END IF;

CREATE TEMPORARY TABLE class_names (
	className VARCHAR(100),
	subclassName VARCHAR(100),
	progifName VARCHAR(100),
	uProgifID TINYINT UNSIGNED
) ENGINE=MEMORY;

INSERT INTO class_names
SELECT className, subClassName, progifName, uProgifID FROM vw_pciClassList 
	WHERE classCode=class_code AND subClasscode=subclass_code AND progifCode=progif_code;

SELECT COUNT(*) INTO findings FROM class_names;

IF findings=0 THEN 
	INSERT INTO class_names
	SELECT className, subClassName, progifName, uProgifID FROM vw_pciClassList 
	WHERE classCode=class_code AND subClasscode=subclass_code;
END  IF;

SELECT * FROM class_names;

DROP TABLE class_names; 

END$$

USE `bugware0`;
DROP procedure IF EXISTS `bugware0`.`sp_queryDeviceName`;
$$

USE `bugware0`$$
CREATE DEFINER=`root`@`localhost` PROCEDURE `sp_queryDeviceName`(
IN vendor_code CHAR(4), IN device_code CHAR(4), IN subvendor_code CHAR(2), IN subdevice_code CHAR(2) )
BEGIN
DECLARE findings TINYINT;

CREATE TEMPORARY TABLE device_names (
	vendorName VARCHAR(100),
	deviceName VARCHAR(150),
	subsysName VARCHAR(150),
	uPcisubID INT UNSIGNED
) ENGINE=MEMORY;

INSERT INTO device_names
SELECT vendorName, deviceName, subSysName, uPcisubID FROM vw_pciDeviceList 
	WHERE vendorCode=vendor_code AND deviceCode=device_code AND subvendor=subvendor_code AND subdevice=subdevice_code;

SELECT COUNT(*) INTO findings FROM device_names;

IF findings=0 THEN 
	INSERT INTO device_names
	SELECT vendorName, deviceName, subSysName=NULL, uPcisubID FROM vw_pciDeviceList 
	  WHERE vendorCode=vendor_code AND deviceCode=device_code;

END  IF;

SELECT * FROM device_names;

DROP TABLE device_names; 


END$$


DELIMITER ;


SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;
