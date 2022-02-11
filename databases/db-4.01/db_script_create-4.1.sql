SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='TRADITIONAL';

CREATE SCHEMA IF NOT EXISTS `mydb` DEFAULT CHARACTER SET latin1 COLLATE latin1_swedish_ci ;
CREATE SCHEMA IF NOT EXISTS `bugware0` DEFAULT CHARACTER SET latin1 ;

-- -----------------------------------------------------
-- Table `bugware0`.`bus_type`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `bugware0`.`bus_type` ;

CREATE  TABLE IF NOT EXISTS `bugware0`.`bus_type` (
  `busTypeID` TINYINT UNSIGNED NOT NULL AUTO_INCREMENT ,
  `busName` VARCHAR(45) NOT NULL ,
  PRIMARY KEY (`busTypeID`) )
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `bugware0`.`pci_vendors`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `bugware0`.`pci_vendors` ;

CREATE  TABLE IF NOT EXISTS `bugware0`.`pci_vendors` (
  `vendorCode` CHAR(4) NOT NULL ,
  `vendorName` VARCHAR(100) NULL ,
  PRIMARY KEY (`vendorCode`) )
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `bugware0`.`pci_devices`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `bugware0`.`pci_devices` ;

CREATE  TABLE IF NOT EXISTS `bugware0`.`pci_devices` (
  `deviceID` INT UNSIGNED NOT NULL AUTO_INCREMENT ,
  `deviceCode` CHAR(4) NOT NULL ,
  `deviceName` VARCHAR(150) NULL ,
  `vendorCode_FK` CHAR(4) NOT NULL ,
  PRIMARY KEY (`deviceID`) ,
  INDEX `fk_pci_devices_pci_vendors` (`vendorCode_FK` ASC) ,
  CONSTRAINT `fk_pci_devices_pci_vendors`
    FOREIGN KEY (`vendorCode_FK` )
    REFERENCES `bugware0`.`pci_vendors` (`vendorCode` )
    ON DELETE NO ACTION
    ON UPDATE CASCADE)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `bugware0`.`pci_subsystems`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `bugware0`.`pci_subsystems` ;

CREATE  TABLE IF NOT EXISTS `bugware0`.`pci_subsystems` (
  `uPcisubID` INT UNSIGNED NOT NULL AUTO_INCREMENT ,
  `subvendor` CHAR(4) NULL ,
  `subdevice` CHAR(4) NULL ,
  `subSysName` VARCHAR(150) NULL ,
  `deviceID_FK` INT UNSIGNED NOT NULL ,
  PRIMARY KEY (`uPcisubID`) ,
  INDEX `fk_pci_subsystems_pci_devices1` (`deviceID_FK` ASC) ,
  CONSTRAINT `fk_pci_subsystems_pci_devices1`
    FOREIGN KEY (`deviceID_FK` )
    REFERENCES `bugware0`.`pci_devices` (`deviceID` )
    ON DELETE NO ACTION
    ON UPDATE CASCADE)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `bugware0`.`pci_classes`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `bugware0`.`pci_classes` ;

CREATE  TABLE IF NOT EXISTS `bugware0`.`pci_classes` (
  `classCode` CHAR(2) NOT NULL ,
  `className` VARCHAR(100) NOT NULL ,
  PRIMARY KEY (`classCode`) )
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `bugware0`.`pci_subclasses`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `bugware0`.`pci_subclasses` ;

CREATE  TABLE IF NOT EXISTS `bugware0`.`pci_subclasses` (
  `subClassID` TINYINT UNSIGNED NOT NULL AUTO_INCREMENT ,
  `subClassCode` CHAR(2) NOT NULL ,
  `subClassName` VARCHAR(100) NULL ,
  `classCode_FK` CHAR(2) NOT NULL ,
  PRIMARY KEY (`subClassID`) ,
  INDEX `fk_pci_subclasses_pci_classes1` (`classCode_FK` ASC) ,
  CONSTRAINT `fk_pci_subclasses_pci_classes1`
    FOREIGN KEY (`classCode_FK` )
    REFERENCES `bugware0`.`pci_classes` (`classCode` )
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `bugware0`.`pci_prog_ifs`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `bugware0`.`pci_prog_ifs` ;

CREATE  TABLE IF NOT EXISTS `bugware0`.`pci_prog_ifs` (
  `uProgifID` TINYINT UNSIGNED NOT NULL AUTO_INCREMENT ,
  `progifCode` CHAR(2) NULL ,
  `progifName` VARCHAR(100) NULL ,
  `subClassID_FK` TINYINT UNSIGNED NOT NULL ,
  PRIMARY KEY (`uProgifID`) ,
  INDEX `fk_pci_prog_ifs_pci_subclasses1` (`subClassID_FK` ASC) ,
  CONSTRAINT `fk_pci_prog_ifs_pci_subclasses1`
    FOREIGN KEY (`subClassID_FK` )
    REFERENCES `bugware0`.`pci_subclasses` (`subClassID` )
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `bugware0`.`pci_all`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `bugware0`.`pci_all` ;

CREATE  TABLE IF NOT EXISTS `bugware0`.`pci_all` (
  `pciSpcID` INT(10) UNSIGNED NOT NULL AUTO_INCREMENT ,
  `uProgifID_FK` TINYINT(3) UNSIGNED NOT NULL ,
  `uPcisubID_FK` INT(10) UNSIGNED NOT NULL ,
  PRIMARY KEY (`pciSpcID`) ,
  INDEX `fk_pci_all_pci_prog_ifs1` (`uProgifID_FK` ASC) ,
  INDEX `uPcisubID_FK` (`uPcisubID_FK` ASC) ,
  CONSTRAINT `pci_all_ibfk_1`
    FOREIGN KEY (`uPcisubID_FK` )
    REFERENCES `bugware0`.`pci_subsystems` (`uPcisubID` )
    ON DELETE NO ACTION
    ON UPDATE CASCADE,
  CONSTRAINT `fk_pci_all_pci_prog_ifs1`
    FOREIGN KEY (`uProgifID_FK` )
    REFERENCES `bugware0`.`pci_prog_ifs` (`uProgifID` )
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB
DEFAULT CHARACTER SET = latin1
COLLATE = latin1_swedish_ci;


-- -----------------------------------------------------
-- Table `bugware0`.`all_devices`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `bugware0`.`all_devices` ;

CREATE  TABLE IF NOT EXISTS `bugware0`.`all_devices` (
  `uDevID` INT UNSIGNED NOT NULL AUTO_INCREMENT ,
  `busTypeID_FK` TINYINT UNSIGNED NOT NULL ,
  `pciSpcID_FK` INT UNSIGNED NULL ,
  PRIMARY KEY (`uDevID`) ,
  INDEX `fk_all_devices_bus_type1` (`busTypeID_FK` ASC) ,
  INDEX `fk_all_devices_pci_all1` (`pciSpcID_FK` ASC) ,
  CONSTRAINT `fk_all_devices_bus_type1`
    FOREIGN KEY (`busTypeID_FK` )
    REFERENCES `bugware0`.`bus_type` (`busTypeID` )
    ON DELETE RESTRICT
    ON UPDATE CASCADE,
  CONSTRAINT `fk_all_devices_pci_all1`
    FOREIGN KEY (`pciSpcID_FK` )
    REFERENCES `bugware0`.`pci_all` (`pciSpcID` )
    ON DELETE CASCADE
    ON UPDATE CASCADE)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `bugware0`.`OSes`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `bugware0`.`OSes` ;

CREATE  TABLE IF NOT EXISTS `bugware0`.`OSes` (
  `osID` SMALLINT UNSIGNED NOT NULL AUTO_INCREMENT ,
  `osName` VARCHAR(45) NOT NULL ,
  PRIMARY KEY (`osID`) )
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `bugware0`.`os_revisions`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `bugware0`.`os_revisions` ;

CREATE  TABLE IF NOT EXISTS `bugware0`.`os_revisions` (
  `revisionID` MEDIUMINT UNSIGNED NOT NULL AUTO_INCREMENT ,
  `osID_FK` SMALLINT UNSIGNED NOT NULL ,
  `revisionName` VARCHAR(45) NULL ,
  PRIMARY KEY (`revisionID`) ,
  INDEX `fk_os_revisions_OSes1` (`osID_FK` ASC) ,
  CONSTRAINT `fk_os_revisions_OSes1`
    FOREIGN KEY (`osID_FK` )
    REFERENCES `bugware0`.`OSes` (`osID` )
    ON DELETE RESTRICT
    ON UPDATE CASCADE)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `bugware0`.`kernels`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `bugware0`.`kernels` ;

CREATE  TABLE IF NOT EXISTS `bugware0`.`kernels` (
  `uKernelID` INT UNSIGNED NOT NULL AUTO_INCREMENT ,
  `revisionID_FK` MEDIUMINT UNSIGNED NOT NULL ,
  `kernelVersion` VARCHAR(45) NULL ,
  PRIMARY KEY (`uKernelID`) ,
  INDEX `fk_kernels_os_revisions1` (`revisionID_FK` ASC) ,
  CONSTRAINT `fk_kernels_os_revisions1`
    FOREIGN KEY (`revisionID_FK` )
    REFERENCES `bugware0`.`os_revisions` (`revisionID` )
    ON DELETE CASCADE
    ON UPDATE CASCADE)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `bugware0`.`module_names`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `bugware0`.`module_names` ;

CREATE  TABLE IF NOT EXISTS `bugware0`.`module_names` (
  `modNameID` SMALLINT UNSIGNED NOT NULL AUTO_INCREMENT ,
  `modName` VARCHAR(45) NOT NULL ,
  PRIMARY KEY (`modNameID`) )
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `bugware0`.`modules`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `bugware0`.`modules` ;

CREATE  TABLE IF NOT EXISTS `bugware0`.`modules` (
  `uModID` INT UNSIGNED NOT NULL AUTO_INCREMENT ,
  `uKernelID_FK` INT UNSIGNED NOT NULL ,
  `modNameID_FK` SMALLINT UNSIGNED NOT NULL ,
  PRIMARY KEY (`uModID`) ,
  INDEX `fk_modules_kernels1` (`uKernelID_FK` ASC) ,
  INDEX `fk_modules_module_names1` (`modNameID_FK` ASC) ,
  CONSTRAINT `fk_modules_kernels1`
    FOREIGN KEY (`uKernelID_FK` )
    REFERENCES `bugware0`.`kernels` (`uKernelID` )
    ON DELETE CASCADE
    ON UPDATE CASCADE,
  CONSTRAINT `fk_modules_module_names1`
    FOREIGN KEY (`modNameID_FK` )
    REFERENCES `bugware0`.`module_names` (`modNameID` )
    ON DELETE RESTRICT
    ON UPDATE CASCADE)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `bugware0`.`dev_mod`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `bugware0`.`dev_mod` ;

CREATE  TABLE IF NOT EXISTS `bugware0`.`dev_mod` (
  `uDevID_FK` INT UNSIGNED NOT NULL ,
  `uModID_FK` INT UNSIGNED NOT NULL ,
  `works` TINYINT(1)  NULL ,
  PRIMARY KEY (`uDevID_FK`, `uModID_FK`) ,
  INDEX `fk_dev_mod_all_devices1` (`uDevID_FK` ASC) ,
  INDEX `fk_dev_mod_modules1` (`uModID_FK` ASC) ,
  CONSTRAINT `fk_dev_mod_all_devices1`
    FOREIGN KEY (`uDevID_FK` )
    REFERENCES `bugware0`.`all_devices` (`uDevID` )
    ON DELETE CASCADE
    ON UPDATE CASCADE,
  CONSTRAINT `fk_dev_mod_modules1`
    FOREIGN KEY (`uModID_FK` )
    REFERENCES `bugware0`.`modules` (`uModID` )
    ON DELETE CASCADE
    ON UPDATE CASCADE)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Placeholder table for view `bugware0`.`vw_pciClassList`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `bugware0`.`vw_pciClassList` (`classCode` INT, `subClassCode` INT, `progifCode` INT, `progifName` INT, `subClassName` INT, `className` INT, `subClassID` INT, `uProgifID` INT);

-- -----------------------------------------------------
-- Placeholder table for view `bugware0`.`vw_pciDeviceList`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `bugware0`.`vw_pciDeviceList` (`vendorCode` INT, `deviceCode` INT, `subvendor` INT, `subdevice` INT, `vendorName` INT, `deviceName` INT, `subSysName` INT, `deviceID` INT, `uPcisubID` INT);

-- -----------------------------------------------------
-- procedure sp_pciClassesAdd
-- -----------------------------------------------------

DELIMITER $$
DROP procedure IF EXISTS `bugware0`.`sp_pciClassesAdd` $$
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
-- procedure sp_pciSubclassesAdd
-- -----------------------------------------------------
DROP procedure IF EXISTS `bugware0`.`sp_pciSubclassesAdd` $$
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

-- -----------------------------------------------------
-- procedure sp_pci_VendorsAdd
-- -----------------------------------------------------
DROP procedure IF EXISTS `bugware0`.`sp_pci_VendorsAdd` $$
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

-- -----------------------------------------------------
-- procedure sp_pci_DevicesAdd
-- -----------------------------------------------------
DROP procedure IF EXISTS `bugware0`.`sp_pci_DevicesAdd` $$
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

-- -----------------------------------------------------
-- View `bugware0`.`vw_pciClassList`
-- -----------------------------------------------------
DROP VIEW IF EXISTS `bugware0`.`vw_pciClassList` ;
DROP TABLE IF EXISTS `bugware0`.`vw_pciClassList`;
CREATE  OR REPLACE ALGORITHM=UNDEFINED DEFINER=`root`@`localhost` SQL SECURITY DEFINER VIEW `bugware0`.`vw_pciClassList` AS select `cl`.`classCode` AS `classCode`,`sub`.`subClassCode` AS `subClassCode`,`ifs`.`progifCode` AS `progifCode`,`ifs`.`progifName` AS `progifName`,`sub`.`subClassName` AS `subClassName`,`cl`.`className` AS `className`,`sub`.`subClassID` AS `subClassID`,`ifs`.`uProgifID` AS `uProgifID` from (`bugware0`.`pci_classes` `cl` left join (`bugware0`.`pci_subclasses` `sub` left join `bugware0`.`pci_prog_ifs` `ifs` on((`ifs`.`subClassID_FK` = `sub`.`subClassID`))) on((`sub`.`classCode_FK` = `cl`.`classCode`)));

-- -----------------------------------------------------
-- View `bugware0`.`vw_pciDeviceList`
-- -----------------------------------------------------
DROP VIEW IF EXISTS `bugware0`.`vw_pciDeviceList` ;
DROP TABLE IF EXISTS `bugware0`.`vw_pciDeviceList`;
CREATE  OR REPLACE ALGORITHM=UNDEFINED DEFINER=`root`@`localhost` SQL SECURITY DEFINER VIEW `bugware0`.`vw_pciDeviceList` AS select `vdr`.`vendorCode` AS `vendorCode`,`dev`.`deviceCode` AS `deviceCode`,`sub`.`subvendor` AS `subvendor`,`sub`.`subdevice` AS `subdevice`,`vdr`.`vendorName` AS `vendorName`,`dev`.`deviceName` AS `deviceName`,`sub`.`subSysName` AS `subSysName`,`dev`.`deviceID` AS `deviceID`,`sub`.`uPcisubID` AS `uPcisubID` from (`bugware0`.`pci_vendors` `vdr` left join (`bugware0`.`pci_devices` `dev` left join `bugware0`.`pci_subsystems` `sub` on((`sub`.`deviceID_FK` = `dev`.`deviceID`))) on((`dev`.`vendorCode_FK` = `vdr`.`vendorCode`)));


SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;
