SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='TRADITIONAL';

ALTER TABLE `harix_db`.`dev_board` 
DROP PRIMARY KEY ;


-- -----------------------------------------------------
-- Placeholder table for view `harix_db`.`vw_osList`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `harix_db`.`vw_osList` (`uKernelID` INT, `releaseID` INT, `osID` INT, `osName` INT, `releaseName` INT, `kernelVersion` INT, `machineHardware` INT);

-- -----------------------------------------------------
-- Placeholder table for view `harix_db`.`vw_os_pcimap`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `harix_db`.`vw_os_pcimap` (`uKernelID_FK` INT, `uModID` INT, `vendor` INT, `device` INT, `subvendor` INT, `subdevice` INT, `class` INT, `classMask` INT, `modNameID_FK` INT);

-- -----------------------------------------------------
-- Placeholder table for view `harix_db`.`vw_pciClassList`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `harix_db`.`vw_pciClassList` (`classCode` INT, `subClassCode` INT, `progifCode` INT, `progifName` INT, `subClassName` INT, `className` INT, `subClassID` INT, `uProgifID` INT);

-- -----------------------------------------------------
-- Placeholder table for view `harix_db`.`vw_pciDeviceList`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `harix_db`.`vw_pciDeviceList` (`vendorCode` INT, `deviceCode` INT, `subvendor` INT, `subdevice` INT, `vendorName` INT, `deviceName` INT, `subSysName` INT, `deviceID` INT, `uPcisubID` INT);


USE `harix_db`;

-- -----------------------------------------------------
-- View `harix_db`.`vw_osList`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `harix_db`.`vw_osList`;
CREATE  OR REPLACE ALGORITHM=UNDEFINED DEFINER=`root`@`localhost` SQL SECURITY DEFINER VIEW `harix_db`.`vw_osList` AS select `ker`.`uKernelID` AS `uKernelID`,`rel`.`releaseID` AS `releaseID`,`os`.`osID` AS `osID`,`os`.`osName` AS `osName`,`rel`.`releaseName` AS `releaseName`,`ker`.`kernelVersion` AS `kernelVersion`,`ker`.`machineHardware` AS `machineHardware` from (`harix_db`.`OSes` `os` left join (`harix_db`.`os_releases` `rel` left join `harix_db`.`kernels` `ker` on((`ker`.`releaseID_FK` = `rel`.`releaseID`))) on((`rel`.`osID_FK` = `os`.`osID`)));


USE `harix_db`;

-- -----------------------------------------------------
-- View `harix_db`.`vw_os_pcimap`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `harix_db`.`vw_os_pcimap`;
CREATE  OR REPLACE ALGORITHM=UNDEFINED DEFINER=`root`@`localhost` SQL SECURITY DEFINER VIEW `harix_db`.`vw_os_pcimap` AS select `mdl`.`uKernelID_FK` AS `uKernelID_FK`,`mdl`.`uModID` AS `uModID`,`harix_db`.`pcimap`.`vendor` AS `vendor`,`harix_db`.`pcimap`.`device` AS `device`,`harix_db`.`pcimap`.`subvendor` AS `subvendor`,`harix_db`.`pcimap`.`subdevice` AS `subdevice`,`harix_db`.`pcimap`.`class` AS `class`,`harix_db`.`pcimap`.`classMask` AS `classMask`,`mdl`.`modNameID_FK` AS `modNameID_FK` from (`harix_db`.`modules` `mdl` left join `harix_db`.`pcimap` on((`harix_db`.`pcimap`.`uModID_FK` = `mdl`.`uModID`)));


USE `harix_db`;

-- -----------------------------------------------------
-- View `harix_db`.`vw_pciClassList`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `harix_db`.`vw_pciClassList`;
CREATE  OR REPLACE ALGORITHM=UNDEFINED DEFINER=`root`@`localhost` SQL SECURITY DEFINER VIEW `harix_db`.`vw_pciClassList` AS select `cl`.`classCode` AS `classCode`,`sub`.`subClassCode` AS `subClassCode`,`ifs`.`progifCode` AS `progifCode`,`ifs`.`progifName` AS `progifName`,`sub`.`subClassName` AS `subClassName`,`cl`.`className` AS `className`,`sub`.`subClassID` AS `subClassID`,`ifs`.`uProgifID` AS `uProgifID` from (`harix_db`.`pci_classes` `cl` left join (`harix_db`.`pci_subclasses` `sub` left join `harix_db`.`pci_prog_ifs` `ifs` on((`ifs`.`subClassID_FK` = `sub`.`subClassID`))) on((`sub`.`classCode_FK` = `cl`.`classCode`)));


USE `harix_db`;

-- -----------------------------------------------------
-- View `harix_db`.`vw_pciDeviceList`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `harix_db`.`vw_pciDeviceList`;
CREATE  OR REPLACE ALGORITHM=UNDEFINED DEFINER=`root`@`localhost` SQL SECURITY DEFINER VIEW `harix_db`.`vw_pciDeviceList` AS select `vdr`.`vendorCode` AS `vendorCode`,`dev`.`deviceCode` AS `deviceCode`,`sub`.`subvendor` AS `subvendor`,`sub`.`subdevice` AS `subdevice`,`vdr`.`vendorName` AS `vendorName`,`dev`.`deviceName` AS `deviceName`,`sub`.`subSysName` AS `subSysName`,`dev`.`deviceID` AS `deviceID`,`sub`.`uPcisubID` AS `uPcisubID` from (`harix_db`.`pci_vendors` `vdr` left join (`harix_db`.`pci_devices` `dev` left join `harix_db`.`pci_subsystems` `sub` on((`sub`.`deviceID_FK` = `dev`.`deviceID`))) on((`dev`.`vendorCode_FK` = `vdr`.`vendorCode`)));

SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;
