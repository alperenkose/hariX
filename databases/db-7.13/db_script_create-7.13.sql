SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='TRADITIONAL';


CREATE SCHEMA IF NOT EXISTS `harix_db` DEFAULT CHARACTER SET latin1 ;

-- -----------------------------------------------------
-- Table `harix_db`.`bus_type`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `harix_db`.`bus_type` ;

CREATE  TABLE IF NOT EXISTS `harix_db`.`bus_type` (
  `busTypeID` TINYINT UNSIGNED NOT NULL AUTO_INCREMENT ,
  `busName` VARCHAR(45) NOT NULL ,
  PRIMARY KEY (`busTypeID`) )
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `harix_db`.`pci_all`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `harix_db`.`pci_all` ;

CREATE  TABLE IF NOT EXISTS `harix_db`.`pci_all` (
  `pciSpcID` INT(10) UNSIGNED NOT NULL AUTO_INCREMENT ,
  `vendorCode` CHAR(4) NOT NULL ,
  `deviceCode` CHAR(4) NOT NULL ,
  `subvendorCode` CHAR(4) NULL ,
  `subdeviceCode` CHAR(4) NULL ,
  `classCode` CHAR(2) NOT NULL ,
  `subclassCode` CHAR(2) NOT NULL ,
  `progifCode` CHAR(2) NULL ,
  PRIMARY KEY (`pciSpcID`) )
ENGINE = InnoDB
DEFAULT CHARACTER SET = latin1
COLLATE = latin1_swedish_ci;


-- -----------------------------------------------------
-- Table `harix_db`.`all_devices`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `harix_db`.`all_devices` ;

CREATE  TABLE IF NOT EXISTS `harix_db`.`all_devices` (
  `uDevID` INT(10) UNSIGNED NOT NULL AUTO_INCREMENT ,
  `busTypeID_FK` TINYINT(3) UNSIGNED NOT NULL DEFAULT '1' ,
  `pciSpcID_FK` INT(10) UNSIGNED NULL DEFAULT NULL ,
  PRIMARY KEY (`uDevID`) ,
  INDEX `fk_all_devices_bus_type1` (`busTypeID_FK` ASC) ,
  INDEX `fk_all_devices_pci_all1` (`pciSpcID_FK` ASC) ,
  CONSTRAINT `fk_all_devices_bus_type1`
    FOREIGN KEY (`busTypeID_FK` )
    REFERENCES `harix_db`.`bus_type` (`busTypeID` )
    ON UPDATE CASCADE,
  CONSTRAINT `fk_all_devices_pci_all1`
    FOREIGN KEY (`pciSpcID_FK` )
    REFERENCES `harix_db`.`pci_all` (`pciSpcID` )
    ON DELETE NO ACTION
    ON UPDATE CASCADE)
ENGINE = InnoDB
AUTO_INCREMENT = 47
DEFAULT CHARACTER SET = latin1
COLLATE = latin1_swedish_ci;


-- -----------------------------------------------------
-- Table `harix_db`.`pci_vendors`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `harix_db`.`pci_vendors` ;

CREATE  TABLE IF NOT EXISTS `harix_db`.`pci_vendors` (
  `vendorCode` CHAR(4) NOT NULL ,
  `vendorName` VARCHAR(100) NULL ,
  PRIMARY KEY (`vendorCode`) )
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `harix_db`.`pci_devices`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `harix_db`.`pci_devices` ;

CREATE  TABLE IF NOT EXISTS `harix_db`.`pci_devices` (
  `deviceID` INT UNSIGNED NOT NULL AUTO_INCREMENT ,
  `deviceCode` CHAR(4) NOT NULL ,
  `deviceName` VARCHAR(150) NULL ,
  `vendorCode_FK` CHAR(4) NOT NULL ,
  PRIMARY KEY (`deviceID`) ,
  INDEX `fk_pci_devices_pci_vendors` (`vendorCode_FK` ASC) ,
  CONSTRAINT `fk_pci_devices_pci_vendors`
    FOREIGN KEY (`vendorCode_FK` )
    REFERENCES `harix_db`.`pci_vendors` (`vendorCode` )
    ON DELETE NO ACTION
    ON UPDATE CASCADE)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `harix_db`.`pci_subsystems`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `harix_db`.`pci_subsystems` ;

CREATE  TABLE IF NOT EXISTS `harix_db`.`pci_subsystems` (
  `uPcisubID` INT UNSIGNED NOT NULL AUTO_INCREMENT ,
  `subvendor` CHAR(4) NULL ,
  `subdevice` CHAR(4) NULL ,
  `subSysName` VARCHAR(150) NULL ,
  `deviceID_FK` INT UNSIGNED NOT NULL ,
  PRIMARY KEY (`uPcisubID`) ,
  INDEX `fk_pci_subsystems_pci_devices1` (`deviceID_FK` ASC) ,
  CONSTRAINT `fk_pci_subsystems_pci_devices1`
    FOREIGN KEY (`deviceID_FK` )
    REFERENCES `harix_db`.`pci_devices` (`deviceID` )
    ON DELETE NO ACTION
    ON UPDATE CASCADE)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `harix_db`.`OSes`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `harix_db`.`OSes` ;

CREATE  TABLE IF NOT EXISTS `harix_db`.`OSes` (
  `osID` SMALLINT UNSIGNED NOT NULL AUTO_INCREMENT ,
  `osName` VARCHAR(45) NOT NULL ,
  PRIMARY KEY (`osID`) )
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `harix_db`.`os_releases`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `harix_db`.`os_releases` ;

CREATE  TABLE IF NOT EXISTS `harix_db`.`os_releases` (
  `releaseID` MEDIUMINT UNSIGNED NOT NULL AUTO_INCREMENT ,
  `osID_FK` SMALLINT UNSIGNED NOT NULL ,
  `releaseName` VARCHAR(45) NULL ,
  PRIMARY KEY (`releaseID`) ,
  INDEX `fk_os_releases_OSes1` (`osID_FK` ASC) ,
  CONSTRAINT `fk_os_releases_OSes1`
    FOREIGN KEY (`osID_FK` )
    REFERENCES `harix_db`.`OSes` (`osID` )
    ON DELETE RESTRICT
    ON UPDATE CASCADE)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `harix_db`.`kernels`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `harix_db`.`kernels` ;

CREATE  TABLE IF NOT EXISTS `harix_db`.`kernels` (
  `uKernelID` INT UNSIGNED NOT NULL AUTO_INCREMENT ,
  `releaseID_FK` MEDIUMINT UNSIGNED NOT NULL ,
  `kernelVersion` VARCHAR(45) NULL ,
  `machineHardware` VARCHAR(10) NULL ,
  PRIMARY KEY (`uKernelID`) ,
  INDEX `fk_kernels_os_releases1` (`releaseID_FK` ASC) ,
  CONSTRAINT `fk_kernels_os_releases1`
    FOREIGN KEY (`releaseID_FK` )
    REFERENCES `harix_db`.`os_releases` (`releaseID` )
    ON DELETE CASCADE
    ON UPDATE CASCADE)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `harix_db`.`module_names`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `harix_db`.`module_names` ;

CREATE  TABLE IF NOT EXISTS `harix_db`.`module_names` (
  `modNameID` SMALLINT UNSIGNED NOT NULL AUTO_INCREMENT ,
  `modName` VARCHAR(45) NOT NULL ,
  PRIMARY KEY (`modNameID`) )
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `harix_db`.`modules`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `harix_db`.`modules` ;

CREATE  TABLE IF NOT EXISTS `harix_db`.`modules` (
  `uModID` INT UNSIGNED NOT NULL AUTO_INCREMENT ,
  `uKernelID_FK` INT UNSIGNED NOT NULL ,
  `modNameID_FK` SMALLINT UNSIGNED NOT NULL ,
  PRIMARY KEY (`uModID`) ,
  INDEX `fk_modules_kernels1` (`uKernelID_FK` ASC) ,
  INDEX `fk_modules_module_names1` (`modNameID_FK` ASC) ,
  CONSTRAINT `fk_modules_kernels1`
    FOREIGN KEY (`uKernelID_FK` )
    REFERENCES `harix_db`.`kernels` (`uKernelID` )
    ON DELETE CASCADE
    ON UPDATE CASCADE,
  CONSTRAINT `fk_modules_module_names1`
    FOREIGN KEY (`modNameID_FK` )
    REFERENCES `harix_db`.`module_names` (`modNameID` )
    ON DELETE RESTRICT
    ON UPDATE CASCADE)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `harix_db`.`dev_mod`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `harix_db`.`dev_mod` ;

CREATE  TABLE IF NOT EXISTS `harix_db`.`dev_mod` (
  `uDevID_FK` INT UNSIGNED NOT NULL ,
  `uModID_FK` INT UNSIGNED NOT NULL ,
  `works` TINYINT(1)  NULL ,
  PRIMARY KEY (`uDevID_FK`, `uModID_FK`) ,
  INDEX `fk_dev_mod_all_devices1` (`uDevID_FK` ASC) ,
  INDEX `fk_dev_mod_modules1` (`uModID_FK` ASC) ,
  CONSTRAINT `fk_dev_mod_all_devices1`
    FOREIGN KEY (`uDevID_FK` )
    REFERENCES `harix_db`.`all_devices` (`uDevID` )
    ON DELETE CASCADE
    ON UPDATE CASCADE,
  CONSTRAINT `fk_dev_mod_modules1`
    FOREIGN KEY (`uModID_FK` )
    REFERENCES `harix_db`.`modules` (`uModID` )
    ON DELETE CASCADE
    ON UPDATE CASCADE)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `harix_db`.`pci_classes`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `harix_db`.`pci_classes` ;

CREATE  TABLE IF NOT EXISTS `harix_db`.`pci_classes` (
  `classCode` CHAR(2) NOT NULL ,
  `className` VARCHAR(100) NOT NULL ,
  PRIMARY KEY (`classCode`) )
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `harix_db`.`pci_subclasses`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `harix_db`.`pci_subclasses` ;

CREATE  TABLE IF NOT EXISTS `harix_db`.`pci_subclasses` (
  `subClassID` TINYINT UNSIGNED NOT NULL AUTO_INCREMENT ,
  `subClassCode` CHAR(2) NOT NULL ,
  `subClassName` VARCHAR(100) NULL ,
  `classCode_FK` CHAR(2) NOT NULL ,
  PRIMARY KEY (`subClassID`) ,
  INDEX `fk_pci_subclasses_pci_classes1` (`classCode_FK` ASC) ,
  CONSTRAINT `fk_pci_subclasses_pci_classes1`
    FOREIGN KEY (`classCode_FK` )
    REFERENCES `harix_db`.`pci_classes` (`classCode` )
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `harix_db`.`pci_prog_ifs`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `harix_db`.`pci_prog_ifs` ;

CREATE  TABLE IF NOT EXISTS `harix_db`.`pci_prog_ifs` (
  `uProgifID` TINYINT UNSIGNED NOT NULL AUTO_INCREMENT ,
  `progifCode` CHAR(2) NULL ,
  `progifName` VARCHAR(100) NULL ,
  `subClassID_FK` TINYINT UNSIGNED NOT NULL ,
  PRIMARY KEY (`uProgifID`) ,
  INDEX `fk_pci_prog_ifs_pci_subclasses1` (`subClassID_FK` ASC) ,
  CONSTRAINT `fk_pci_prog_ifs_pci_subclasses1`
    FOREIGN KEY (`subClassID_FK` )
    REFERENCES `harix_db`.`pci_subclasses` (`subClassID` )
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `harix_db`.`pcimap`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `harix_db`.`pcimap` ;

CREATE  TABLE IF NOT EXISTS `harix_db`.`pcimap` (
  `uModID_FK` INT(10) UNSIGNED NOT NULL ,
  `vendor` CHAR(4) NOT NULL ,
  `device` CHAR(4) NOT NULL ,
  `subvendor` CHAR(4) NOT NULL ,
  `subdevice` CHAR(4) NOT NULL ,
  `class` CHAR(6) NOT NULL ,
  `classMask` CHAR(6) NOT NULL ,
  INDEX `fk_pcimap_modules1` (`uModID_FK` ASC) ,
  CONSTRAINT `fk_pcimap_modules1`
    FOREIGN KEY (`uModID_FK` )
    REFERENCES `harix_db`.`modules` (`uModID` )
    ON DELETE CASCADE
    ON UPDATE CASCADE)
ENGINE = InnoDB
DEFAULT CHARACTER SET = latin1
COLLATE = latin1_swedish_ci;


-- -----------------------------------------------------
-- Table `harix_db`.`board_models`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `harix_db`.`board_models` ;

CREATE  TABLE IF NOT EXISTS `harix_db`.`board_models` (
  `boardID` MEDIUMINT UNSIGNED NOT NULL AUTO_INCREMENT ,
  `boardName` VARCHAR(100) NOT NULL ,
  PRIMARY KEY (`boardID`) )
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `harix_db`.`dev_board`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `harix_db`.`dev_board` ;

CREATE  TABLE IF NOT EXISTS `harix_db`.`dev_board` (
  `boardID_FK` MEDIUMINT UNSIGNED NOT NULL ,
  `uDevID_FK` INT UNSIGNED NOT NULL ,
  PRIMARY KEY (`boardID_FK`, `uDevID_FK`) ,
  INDEX `fk_dev_board_board_models1` (`boardID_FK` ASC) ,
  INDEX `fk_dev_board_all_devices1` (`uDevID_FK` ASC) ,
  CONSTRAINT `fk_dev_board_board_models1`
    FOREIGN KEY (`boardID_FK` )
    REFERENCES `harix_db`.`board_models` (`boardID` )
    ON DELETE CASCADE
    ON UPDATE CASCADE,
  CONSTRAINT `fk_dev_board_all_devices1`
    FOREIGN KEY (`uDevID_FK` )
    REFERENCES `harix_db`.`all_devices` (`uDevID` )
    ON DELETE NO ACTION
    ON UPDATE CASCADE)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Placeholder table for view `harix_db`.`vw_pciClassList`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `harix_db`.`vw_pciClassList` (`classCode` INT, `subClassCode` INT, `progifCode` INT, `progifName` INT, `subClassName` INT, `className` INT, `subClassID` INT, `uProgifID` INT);

-- -----------------------------------------------------
-- Placeholder table for view `harix_db`.`vw_pciDeviceList`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `harix_db`.`vw_pciDeviceList` (`vendorCode` INT, `deviceCode` INT, `subvendor` INT, `subdevice` INT, `vendorName` INT, `deviceName` INT, `subSysName` INT, `deviceID` INT, `uPcisubID` INT);

-- -----------------------------------------------------
-- Placeholder table for view `harix_db`.`vw_osList`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `harix_db`.`vw_osList` (`uKernelID` INT, `releaseID` INT, `osID` INT, `osName` INT, `releaseName` INT, `kernelVersion` INT, `machineHardware` INT);

-- -----------------------------------------------------
-- Placeholder table for view `harix_db`.`vw_os_pcimap`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `harix_db`.`vw_os_pcimap` (`uKernelID_FK` INT, `uModID` INT, `vendor` INT, `device` INT, `subvendor` INT, `subdevice` INT, `class` INT, `modNameID_FK` INT);

-- -----------------------------------------------------
-- procedure sp_pciClassesAdd
-- -----------------------------------------------------
USE `harix_db`;
DROP procedure IF EXISTS `harix_db`.`sp_pciClassesAdd`;

DELIMITER $$
USE `harix_db`$$
CREATE DEFINER=`root`@`localhost` PROCEDURE `sp_pciClassesAdd`(IN class_code CHAR(2), IN class_name VARCHAR(100))
BEGIN

declare turn_back TINYINT(1) default 0;
declare CONTINUE handler for sqlexception, sqlwarning, not found BEGIN 
SET turn_back=1;
END;

START TRANSACTION;
-- some statement
INSERT INTO pci_classes VALUES (class_code, class_name);


IF turn_back=0 THEN
	COMMIT;
	select 'well done';
ELSE
	rollback;
	select 'error occured';
END IF;

END$$
DELIMITER ;

-- -----------------------------------------------------
-- procedure sp_pciProgifsAdd
-- -----------------------------------------------------
USE `harix_db`;
DROP procedure IF EXISTS `harix_db`.`sp_pciProgifsAdd`;

DELIMITER $$
USE `harix_db`$$
CREATE DEFINER=`root`@`localhost` PROCEDURE `sp_pciProgifsAdd`(
IN class_code CHAR(2), IN subclass_code CHAR(2), IN progif_code CHAR(2), IN progif_name VARCHAR(100))
BEGIN

DECLARE subclass_id TINYINT UNSIGNED;
DECLARE turn_back TINYINT(1) DEFAULT 0;
DECLARE CONTINUE HANDLER FOR SQLEXCEPTION, SQLWARNING, NOT FOUND BEGIN 
SET turn_back=1;
END;

START TRANSACTION;

SELECT subClassID INTO subclass_id FROM vw_pciClassList WHERE classCode=class_code AND subClassCode=subclass_code LIMIT 1;

INSERT INTO pci_prog_ifs (progifCode, progifName, subClassID_FK) VALUES ( progif_code, progif_name, subclass_id );

IF turn_back=0 THEN
	COMMIT;
	select 'well done', subclass_id;
-- 	select LAST_INSERT_ID();
ELSE
	rollback;
	select 'error occured';
END IF;

END$$
DELIMITER ;

-- -----------------------------------------------------
-- procedure sp_pciSubclassesAdd
-- -----------------------------------------------------
USE `harix_db`;
DROP procedure IF EXISTS `harix_db`.`sp_pciSubclassesAdd`;

DELIMITER $$
USE `harix_db`$$
CREATE DEFINER=`root`@`localhost` PROCEDURE `sp_pciSubclassesAdd`(
IN class_code CHAR(2), IN subclass_code CHAR(2), IN subclass_name VARCHAR(100))
BEGIN

DECLARE turn_back TINYINT(1) DEFAULT 0;
DECLARE CONTINUE HANDLER FOR SQLEXCEPTION, SQLWARNING, NOT FOUND BEGIN 
SET turn_back=1;
END;

START TRANSACTION;
-- some statement
INSERT INTO pci_subclasses (subClassCode, subClassName, classCode_FK) VALUES (subclass_code, subclass_name, class_code);


IF turn_back=0 THEN
	COMMIT;
	select LAST_INSERT_ID();
ELSE
	rollback;
	select 'error occured';
END IF;

END$$
DELIMITER ;

-- -----------------------------------------------------
-- procedure sp_pci_VendorsAdd
-- -----------------------------------------------------
USE `harix_db`;
DROP procedure IF EXISTS `harix_db`.`sp_pci_VendorsAdd`;

DELIMITER $$
USE `harix_db`$$
CREATE DEFINER=`root`@`localhost` PROCEDURE `harix_db`.`sp_pci_VendorsAdd`(IN vendor_code CHAR(4), IN vendor_name VARCHAR(100))
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

-- -----------------------------------------------------
-- procedure sp_pci_DevicesAdd
-- -----------------------------------------------------
USE `harix_db`;
DROP procedure IF EXISTS `harix_db`.`sp_pci_DevicesAdd`;

DELIMITER $$
USE `harix_db`$$
CREATE DEFINER=`root`@`localhost` PROCEDURE `sp_pci_DevicesAdd`(
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
	select 'well done';
ELSE
	rollback;
	select 'error occured';
END IF;

END$$
DELIMITER ;

-- -----------------------------------------------------
-- procedure sp_pci_SubsystemsAdd
-- -----------------------------------------------------
USE `harix_db`;
DROP procedure IF EXISTS `harix_db`.`sp_pci_SubsystemsAdd`;

DELIMITER $$
USE `harix_db`$$
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

-- -----------------------------------------------------
-- procedure sp_queryPcimap
-- -----------------------------------------------------
USE `harix_db`;
DROP procedure IF EXISTS `harix_db`.`sp_queryPcimap`;

DELIMITER $$
USE `harix_db`$$
CREATE DEFINER=`root`@`localhost` PROCEDURE `sp_queryPcimap`(
IN vendor_code CHAR(4), IN device_code CHAR(4), IN subvendor_code CHAR(4), IN subdevice_code CHAR(4), IN class_sub CHAR(4), IN prog_if CHAR(2) )
BEGIN
DECLARE findings TINYINT;
DECLARE class_code CHAR(6);

IF prog_if='' THEN
	SET prog_if = '00';
END IF;

SET class_code = CONCAT ( class_sub , prog_if);

CREATE TEMPORARY TABLE pcimap_devices (
	uKernelID INT,
	modNameID SMALLINT,
	vendor CHAR(6),
	device CHAR(6),
	subvendor CHAR(6),
	subdevice CHAR(6)
) ENGINE=MEMORY;

INSERT INTO pcimap_devices
SELECT uKernelID_FK, modNameID_FK, vendor, device, subvendor, subdevice FROM vw_os_pcimap 
	WHERE class=class_code AND classMask='ffffff';

SELECT COUNT(*) INTO findings FROM pcimap_devices WHERE (vendor=vendor_code OR vendor='ffff') AND (device=device_code OR device='ffff') 
								AND (subvendor=subvendor_code OR subvendor='ffff') AND (subdevice=subdevice_code OR subdevice='ffff');

IF findings=0 THEN 
	SET class_code = CONCAT ( class_sub , '00');
	INSERT INTO pcimap_devices
	SELECT uKernelID_FK, modNameID_FK, vendor, device, subvendor, subdevice FROM vw_os_pcimap 
	WHERE class=class_code AND (classMask='ffff00' OR classMask='ff0000');

	SELECT COUNT(*) INTO findings FROM pcimap_devices WHERE (vendor=vendor_code OR vendor='ffff') AND (device=device_code OR device='ffff') 
								AND (subvendor=subvendor_code OR subvendor='ffff') AND (subdevice=subdevice_code OR subdevice='ffff');

	IF findings=0 THEN 
		SET class_code = '000000';
		INSERT INTO pcimap_devices
		SELECT uKernelID_FK, modNameID_FK, vendor, device, subvendor, subdevice FROM vw_os_pcimap 
		WHERE class=class_code;
	END IF;
END  IF;

SELECT * FROM pcimap_devices WHERE (vendor=vendor_code OR vendor='ffff') AND (device=device_code OR device='ffff') 
 								AND (subvendor=subvendor_code OR subvendor='ffff') AND (subdevice=subdevice_code OR subdevice='ffff');


DROP TABLE pcimap_devices; 

END$$
DELIMITER ;

-- -----------------------------------------------------
-- procedure sp_queryClassName
-- -----------------------------------------------------
USE `harix_db`;
DROP procedure IF EXISTS `harix_db`.`sp_queryClassName`;

DELIMITER $$
USE `harix_db`$$
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
DELIMITER ;

-- -----------------------------------------------------
-- procedure sp_queryDeviceName
-- -----------------------------------------------------
USE `harix_db`;
DROP procedure IF EXISTS `harix_db`.`sp_queryDeviceName`;

DELIMITER $$
USE `harix_db`$$
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

-- -----------------------------------------------------
-- View `harix_db`.`vw_pciClassList`
-- -----------------------------------------------------
DROP VIEW IF EXISTS `harix_db`.`vw_pciClassList` ;
DROP TABLE IF EXISTS `harix_db`.`vw_pciClassList`;
CREATE  OR REPLACE ALGORITHM=UNDEFINED DEFINER=`root`@`localhost` SQL SECURITY DEFINER VIEW `harix_db`.`vw_pciClassList` AS select `cl`.`classCode` AS `classCode`,`sub`.`subClassCode` AS `subClassCode`,`ifs`.`progifCode` AS `progifCode`,`ifs`.`progifName` AS `progifName`,`sub`.`subClassName` AS `subClassName`,`cl`.`className` AS `className`,`sub`.`subClassID` AS `subClassID`,`ifs`.`uProgifID` AS `uProgifID` from (`harix_db`.`pci_classes` `cl` left join (`harix_db`.`pci_subclasses` `sub` left join `harix_db`.`pci_prog_ifs` `ifs` on((`ifs`.`subClassID_FK` = `sub`.`subClassID`))) on((`sub`.`classCode_FK` = `cl`.`classCode`)));

-- -----------------------------------------------------
-- View `harix_db`.`vw_pciDeviceList`
-- -----------------------------------------------------
DROP VIEW IF EXISTS `harix_db`.`vw_pciDeviceList` ;
DROP TABLE IF EXISTS `harix_db`.`vw_pciDeviceList`;
CREATE  OR REPLACE ALGORITHM=UNDEFINED DEFINER=`root`@`localhost` SQL SECURITY DEFINER VIEW `harix_db`.`vw_pciDeviceList` AS select `vdr`.`vendorCode` AS `vendorCode`,`dev`.`deviceCode` AS `deviceCode`,`sub`.`subvendor` AS `subvendor`,`sub`.`subdevice` AS `subdevice`,`vdr`.`vendorName` AS `vendorName`,`dev`.`deviceName` AS `deviceName`,`sub`.`subSysName` AS `subSysName`,`dev`.`deviceID` AS `deviceID`,`sub`.`uPcisubID` AS `uPcisubID` from (`harix_db`.`pci_vendors` `vdr` left join (`harix_db`.`pci_devices` `dev` left join `harix_db`.`pci_subsystems` `sub` on((`sub`.`deviceID_FK` = `dev`.`deviceID`))) on((`dev`.`vendorCode_FK` = `vdr`.`vendorCode`)));

-- -----------------------------------------------------
-- View `harix_db`.`vw_osList`
-- -----------------------------------------------------
DROP VIEW IF EXISTS `harix_db`.`vw_osList` ;
DROP TABLE IF EXISTS `harix_db`.`vw_osList`;
CREATE  OR REPLACE ALGORITHM=UNDEFINED DEFINER=`root`@`localhost` SQL SECURITY DEFINER VIEW `harix_db`.`vw_osList` AS select `ker`.`uKernelID` AS `uKernelID`,`rel`.`releaseID` AS `releaseID`,`os`.`osID` AS `osID`,`os`.`osName` AS `osName`,`rel`.`releaseName` AS `releaseName`,`ker`.`kernelVersion` AS `kernelVersion`,`ker`.`machineHardware` AS `machineHardware` from (`harix_db`.`OSes` `os` left join (`harix_db`.`os_releases` `rel` left join `harix_db`.`kernels` `ker` on((`ker`.`releaseID_FK` = `rel`.`releaseID`))) on((`rel`.`osID_FK` = `os`.`osID`)));

-- -----------------------------------------------------
-- View `harix_db`.`vw_os_pcimap`
-- -----------------------------------------------------
DROP VIEW IF EXISTS `harix_db`.`vw_os_pcimap` ;
DROP TABLE IF EXISTS `harix_db`.`vw_os_pcimap`;
CREATE  OR REPLACE ALGORITHM=UNDEFINED DEFINER=`root`@`localhost` SQL SECURITY DEFINER VIEW `harix_db`.`vw_os_pcimap` AS select `mdl`.`uKernelID_FK` AS `uKernelID_FK`,`mdl`.`uModID` AS `uModID`,`harix_db`.`pcimap`.`vendor` AS `vendor`,`harix_db`.`pcimap`.`device` AS `device`,`harix_db`.`pcimap`.`subvendor` AS `subvendor`,`harix_db`.`pcimap`.`subdevice` AS `subdevice`,`harix_db`.`pcimap`.`class` AS `class`,`mdl`.`modNameID_FK` AS `modNameID_FK` from (`harix_db`.`modules` `mdl` left join `harix_db`.`pcimap` on((`harix_db`.`pcimap`.`uModID_FK` = `mdl`.`uModID`)));

SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;
