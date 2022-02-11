-- phpMyAdmin SQL Dump
-- version 3.2.5
-- http://www.phpmyadmin.net
--
-- Host: myhost
-- Generation Time: Feb 05, 2010 at 07:05 PM
-- Server version: 5.1.38
-- PHP Version: 5.3.1

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Database: `bugware0`
--
CREATE DATABASE `bugware0` DEFAULT CHARACTER SET latin1 COLLATE latin1_swedish_ci;
USE `bugware0`;

-- --------------------------------------------------------

--
-- Table structure for table `OSes`
--

CREATE TABLE IF NOT EXISTS `OSes` (
  `osID` smallint(5) unsigned NOT NULL AUTO_INCREMENT,
  `osName` varchar(45) NOT NULL,
  PRIMARY KEY (`osID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Table structure for table `all_devices`
--

CREATE TABLE IF NOT EXISTS `all_devices` (
  `uDevID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `busTypeID_FK` tinyint(3) unsigned NOT NULL,
  `pciSpcID_FK` int(10) unsigned DEFAULT NULL,
  PRIMARY KEY (`uDevID`),
  KEY `fk_all_devices_pci_subsystems1` (`pciSpcID_FK`),
  KEY `fk_all_devices_bus_type1` (`busTypeID_FK`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Table structure for table `bus_type`
--

CREATE TABLE IF NOT EXISTS `bus_type` (
  `busTypeID` tinyint(3) unsigned NOT NULL AUTO_INCREMENT,
  `busName` varchar(45) NOT NULL,
  PRIMARY KEY (`busTypeID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Table structure for table `dev_mod`
--

CREATE TABLE IF NOT EXISTS `dev_mod` (
  `uDevID_FK` int(10) unsigned NOT NULL,
  `uModID_FK` int(10) unsigned NOT NULL,
  `works` tinyint(1) DEFAULT NULL,
  PRIMARY KEY (`uDevID_FK`,`uModID_FK`),
  KEY `fk_dev_mod_all_devices1` (`uDevID_FK`),
  KEY `fk_dev_mod_modules1` (`uModID_FK`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `kernels`
--

CREATE TABLE IF NOT EXISTS `kernels` (
  `uKernelID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `revisionID_FK` mediumint(8) unsigned NOT NULL,
  `kernelVersion` varchar(45) DEFAULT NULL,
  PRIMARY KEY (`uKernelID`),
  KEY `fk_kernels_os_revisions1` (`revisionID_FK`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Table structure for table `module_names`
--

CREATE TABLE IF NOT EXISTS `module_names` (
  `modNameID` smallint(5) unsigned NOT NULL AUTO_INCREMENT,
  `modName` varchar(45) NOT NULL,
  PRIMARY KEY (`modNameID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Table structure for table `modules`
--

CREATE TABLE IF NOT EXISTS `modules` (
  `uModID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `uKernelID_FK` int(10) unsigned NOT NULL,
  `modNameID_FK` smallint(5) unsigned NOT NULL,
  PRIMARY KEY (`uModID`),
  KEY `fk_modules_kernels1` (`uKernelID_FK`),
  KEY `fk_modules_module_names1` (`modNameID_FK`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Table structure for table `os_revisions`
--

CREATE TABLE IF NOT EXISTS `os_revisions` (
  `revisionID` mediumint(8) unsigned NOT NULL AUTO_INCREMENT,
  `osID_FK` smallint(5) unsigned NOT NULL,
  `revisionName` varchar(45) DEFAULT NULL,
  PRIMARY KEY (`revisionID`),
  KEY `fk_os_revisions_OSes1` (`osID_FK`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Table structure for table `pci_classes`
--

CREATE TABLE IF NOT EXISTS `pci_classes` (
  `classCode` char(2) NOT NULL,
  `className` varchar(100) NOT NULL,
  PRIMARY KEY (`classCode`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `pci_devices`
--

CREATE TABLE IF NOT EXISTS `pci_devices` (
  `deviceID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `deviceCode` char(4) NOT NULL,
  `deviceName` varchar(150) DEFAULT NULL,
  `vendorCode_FK` char(4) NOT NULL,
  PRIMARY KEY (`deviceID`),
  KEY `fk_pci_devices_pci_vendors` (`vendorCode_FK`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Table structure for table `pci_prog_ifs`
--

CREATE TABLE IF NOT EXISTS `pci_prog_ifs` (
  `uProgifID` tinyint(3) unsigned NOT NULL AUTO_INCREMENT,
  `progifCode` char(2) DEFAULT NULL,
  `progifName` varchar(100) DEFAULT NULL,
  `subClassID_FK` tinyint(3) unsigned NOT NULL,
  PRIMARY KEY (`uProgifID`),
  KEY `fk_pci_prog_ifs_pci_subclasses1` (`subClassID_FK`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=13 ;

-- --------------------------------------------------------

--
-- Table structure for table `pci_subclasses`
--

CREATE TABLE IF NOT EXISTS `pci_subclasses` (
  `subClassID` tinyint(3) unsigned NOT NULL AUTO_INCREMENT,
  `subClassCode` char(2) NOT NULL,
  `subClassName` varchar(100) DEFAULT NULL,
  `classCode_FK` char(2) NOT NULL,
  PRIMARY KEY (`subClassID`),
  KEY `fk_pci_subclasses_pci_classes1` (`classCode_FK`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=8 ;

-- --------------------------------------------------------

--
-- Table structure for table `pci_subsystems`
--

CREATE TABLE IF NOT EXISTS `pci_subsystems` (
  `pciSpcID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `subvendor` char(4) DEFAULT NULL,
  `subdevice` char(4) DEFAULT NULL,
  `subSysName` varchar(150) DEFAULT NULL,
  `deviceID_FK` int(10) unsigned NOT NULL,
  `uProgifID_FK` tinyint(3) unsigned DEFAULT NULL,
  PRIMARY KEY (`pciSpcID`),
  KEY `fk_pci_subsystems_pci_devices1` (`deviceID_FK`),
  KEY `fk_pci_subsystems_pci_prog_ifs1` (`uProgifID_FK`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Table structure for table `pci_vendors`
--

CREATE TABLE IF NOT EXISTS `pci_vendors` (
  `vendorCode` char(4) NOT NULL,
  `vendorName` varchar(100) DEFAULT NULL,
  PRIMARY KEY (`vendorCode`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `vw_pciClassList`
--

CREATE ALGORITHM=UNDEFINED DEFINER=`root`@`localhost` SQL SECURITY DEFINER VIEW `bugware0`.`vw_pciClassList` AS select `cl`.`classCode` AS `classCode`,`sub`.`subClassCode` AS `subClassCode`,`ifs`.`progifCode` AS `progifCode`,`ifs`.`progifName` AS `progifName`,`sub`.`subClassName` AS `subClassName`,`cl`.`className` AS `className`,`sub`.`subClassID` AS `subClassID`,`ifs`.`uProgifID` AS `uProgifID` from (`bugware0`.`pci_classes` `cl` left join (`bugware0`.`pci_subclasses` `sub` left join `bugware0`.`pci_prog_ifs` `ifs` on((`ifs`.`subClassID_FK` = `sub`.`subClassID`))) on((`sub`.`classCode_FK` = `cl`.`classCode`)));

--
-- Constraints for dumped tables
--

--
-- Constraints for table `all_devices`
--
ALTER TABLE `all_devices`
  ADD CONSTRAINT `fk_all_devices_bus_type1` FOREIGN KEY (`busTypeID_FK`) REFERENCES `bus_type` (`busTypeID`) ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_all_devices_pci_subsystems1` FOREIGN KEY (`pciSpcID_FK`) REFERENCES `pci_subsystems` (`pciSpcID`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `dev_mod`
--
ALTER TABLE `dev_mod`
  ADD CONSTRAINT `fk_dev_mod_all_devices1` FOREIGN KEY (`uDevID_FK`) REFERENCES `all_devices` (`uDevID`) ON DELETE CASCADE ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_dev_mod_modules1` FOREIGN KEY (`uModID_FK`) REFERENCES `modules` (`uModID`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `kernels`
--
ALTER TABLE `kernels`
  ADD CONSTRAINT `fk_kernels_os_revisions1` FOREIGN KEY (`revisionID_FK`) REFERENCES `os_revisions` (`revisionID`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `modules`
--
ALTER TABLE `modules`
  ADD CONSTRAINT `fk_modules_kernels1` FOREIGN KEY (`uKernelID_FK`) REFERENCES `kernels` (`uKernelID`) ON DELETE CASCADE ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_modules_module_names1` FOREIGN KEY (`modNameID_FK`) REFERENCES `module_names` (`modNameID`) ON UPDATE CASCADE;

--
-- Constraints for table `os_revisions`
--
ALTER TABLE `os_revisions`
  ADD CONSTRAINT `fk_os_revisions_OSes1` FOREIGN KEY (`osID_FK`) REFERENCES `OSes` (`osID`) ON UPDATE CASCADE;

--
-- Constraints for table `pci_devices`
--
ALTER TABLE `pci_devices`
  ADD CONSTRAINT `fk_pci_devices_pci_vendors` FOREIGN KEY (`vendorCode_FK`) REFERENCES `pci_vendors` (`vendorCode`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `pci_prog_ifs`
--
ALTER TABLE `pci_prog_ifs`
  ADD CONSTRAINT `fk_pci_prog_ifs_pci_subclasses1` FOREIGN KEY (`subClassID_FK`) REFERENCES `pci_subclasses` (`subClassID`) ON DELETE NO ACTION ON UPDATE CASCADE;

--
-- Constraints for table `pci_subclasses`
--
ALTER TABLE `pci_subclasses`
  ADD CONSTRAINT `pci_subclasses_ibfk_1` FOREIGN KEY (`classCode_FK`) REFERENCES `pci_classes` (`classCode`) ON UPDATE CASCADE;

--
-- Constraints for table `pci_subsystems`
--
ALTER TABLE `pci_subsystems`
  ADD CONSTRAINT `fk_pci_subsystems_pci_devices1` FOREIGN KEY (`deviceID_FK`) REFERENCES `pci_devices` (`deviceID`) ON DELETE CASCADE ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_pci_subsystems_pci_prog_ifs1` FOREIGN KEY (`uProgifID_FK`) REFERENCES `pci_prog_ifs` (`uProgifID`) ON DELETE SET NULL ON UPDATE CASCADE;

DELIMITER $$
--
-- Procedures
--
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

CREATE DEFINER=`root`@`localhost` PROCEDURE `sp_pciProgifsAdd`(
IN class_code CHAR(2), IN subclass_code CHAR(2), IN progif_code CHAR(2), IN progif_name VARCHAR(100))
BEGIN

DECLARE subclass_id TINYINT;
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
