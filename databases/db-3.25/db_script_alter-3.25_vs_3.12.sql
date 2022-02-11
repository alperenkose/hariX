SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='TRADITIONAL';

DROP SCHEMA IF EXISTS `default_schema` ;


-- -----------------------------------------------------
-- Placeholder table for view `bugware0`.`vw_pciDeviceList`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `bugware0`.`vw_pciDeviceList` (`vendorCode` INT, `deviceCode` INT, `subvendor` INT, `subdevice` INT, `vendorName` INT, `deviceName` INT, `subSysName` INT, `deviceID` INT, `uPcisubID` INT);

USE `bugware0`;

-- -----------------------------------------------------
-- View `bugware0`.`vw_pciDeviceList`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `bugware0`.`vw_pciDeviceList`;
CREATE  OR REPLACE ALGORITHM=UNDEFINED DEFINER=`root`@`localhost` SQL SECURITY DEFINER VIEW `bugware0`.`vw_pciDeviceList` AS select `vdr`.`vendorCode` AS `vendorCode`,`dev`.`deviceCode` AS `deviceCode`,`sub`.`subvendor` AS `subvendor`,`sub`.`subdevice` AS `subdevice`,`vdr`.`vendorName` AS `vendorName`,`dev`.`deviceName` AS `deviceName`,`sub`.`subSysName` AS `subSysName`,`dev`.`deviceID` AS `deviceID`,`sub`.`uPcisubID` AS `uPcisubID` from (`bugware0`.`pci_vendors` `vdr` left join (`bugware0`.`pci_devices` `dev` left join `bugware0`.`pci_subsystems` `sub` on((`sub`.`deviceID_FK` = `dev`.`deviceID`))) on((`dev`.`vendorCode_FK` = `vdr`.`vendorCode`)));


-- -----------------------------------------------------
-- Stored Procedure `bugware0`.`sp_pci_SubsystemsAdd`
-- -----------------------------------------------------
DELIMITER $$

CREATE DEFINER=`root`@`localhost` PROCEDURE `sp_pci_SubsystemsAdd`(
IN vendor_code CHAR(4), IN device_code CHAR(4), IN subvendor_code CHAR(4), IN subdevice_code CHAR(4), IN subsys_name VARCHAR(150))
BEGIN

DECLARE device_id TINYINT;
DECLARE turn_back TINYINT(1) DEFAULT 0;
DECLARE CONTINUE HANDLER FOR SQLEXCEPTION, SQLWARNING, NOT FOUND BEGIN 
SET turn_back=1;
END;

START TRANSACTION;

SELECT deviceID INTO device_id FROM vw_pciDeviceList WHERE vendorCode=vendor_code AND deviceCode=device_code;

INSERT INTO pci_subsystems (subvendor, subdevice, subSysName, deviceID_FK) VALUES ( subvendor_code, subdevice_code, subsys_name, device_id );

IF turn_back=0 THEN
	COMMIT;
	select LAST_INSERT_ID();
ELSE
	rollback;
	select 'error occured';
END IF;
END$$


DELIMITER ;


SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;