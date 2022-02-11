SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='TRADITIONAL';


-- -----------------------------------------------------
-- Placeholder table for view `bugware0`.`vw_osList`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `bugware0`.`vw_osList` (`uKernelID` INT, `releaseID` INT, `osID` INT, `osName` INT, `releaseName` INT, `kernelVersion` INT, `machineHardware` INT);

-- -----------------------------------------------------
-- Placeholder table for view `bugware0`.`vw_os_pcimap`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `bugware0`.`vw_os_pcimap` (`uKernelID_FK` INT, `uModID` INT, `vendor` INT, `device` INT, `subvendor` INT, `subdevice` INT, `class` INT, `modNameID_FK` INT);


USE `bugware0`;

-- -----------------------------------------------------
-- View `bugware0`.`vw_osList`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `bugware0`.`vw_osList`;
CREATE  OR REPLACE ALGORITHM=UNDEFINED DEFINER=`root`@`localhost` SQL SECURITY DEFINER VIEW `bugware0`.`vw_osList` AS select `ker`.`uKernelID` AS `uKernelID`,`rel`.`releaseID` AS `releaseID`,`os`.`osID` AS `osID`,`os`.`osName` AS `osName`,`rel`.`releaseName` AS `releaseName`,`ker`.`kernelVersion` AS `kernelVersion`,`ker`.`machineHardware` AS `machineHardware` from (`bugware0`.`OSes` `os` left join (`bugware0`.`os_releases` `rel` left join `bugware0`.`kernels` `ker` on((`ker`.`releaseID_FK` = `rel`.`releaseID`))) on((`rel`.`osID_FK` = `os`.`osID`)));


USE `bugware0`;

-- -----------------------------------------------------
-- View `bugware0`.`vw_os_pcimap`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `bugware0`.`vw_os_pcimap`;
CREATE  OR REPLACE ALGORITHM=UNDEFINED DEFINER=`root`@`localhost` SQL SECURITY DEFINER VIEW `bugware0`.`vw_os_pcimap` AS select `mdl`.`uKernelID_FK` AS `uKernelID_FK`,`mdl`.`uModID` AS `uModID`,`bugware0`.`pcimap`.`vendor` AS `vendor`,`bugware0`.`pcimap`.`device` AS `device`,`bugware0`.`pcimap`.`subvendor` AS `subvendor`,`bugware0`.`pcimap`.`subdevice` AS `subdevice`,`bugware0`.`pcimap`.`class` AS `class`,`mdl`.`modNameID_FK` AS `modNameID_FK` from (`bugware0`.`modules` `mdl` left join `bugware0`.`pcimap` on((`bugware0`.`pcimap`.`uModID_FK` = `mdl`.`uModID`)));


SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;
