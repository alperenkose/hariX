SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='TRADITIONAL';

DROP TABLE IF EXISTS `bugware0`.`os_revisions` ;

CREATE  TABLE IF NOT EXISTS `bugware0`.`os_releases` (
  `releaseID` MEDIUMINT(8) UNSIGNED NOT NULL AUTO_INCREMENT ,
  `osID_FK` SMALLINT(5) UNSIGNED NOT NULL ,
  `releaseName` VARCHAR(45) NULL DEFAULT NULL ,
  PRIMARY KEY (`releaseID`) ,
  INDEX `fk_os_releases_OSes1` (`osID_FK` ASC) ,
  CONSTRAINT `fk_os_releases_OSes1`
    FOREIGN KEY (`osID_FK` )
    REFERENCES `bugware0`.`OSes` (`osID` )
    ON DELETE RESTRICT
    ON UPDATE CASCADE)
ENGINE = InnoDB
DEFAULT CHARACTER SET = latin1
COLLATE = latin1_swedish_ci;

ALTER TABLE `bugware0`.`all_devices` DROP FOREIGN KEY `fk_all_devices_pci_all1` ;

ALTER TABLE `bugware0`.`all_devices` 
  ADD CONSTRAINT `fk_all_devices_pci_all1`
  FOREIGN KEY (`pciSpcID_FK` )
  REFERENCES `bugware0`.`pci_all` (`pciSpcID` )
  ON DELETE NO ACTION
  ON UPDATE CASCADE;

ALTER TABLE `bugware0`.`kernels` DROP COLUMN `revisionID_FK` , ADD COLUMN `machineHardware` VARCHAR(10) NULL DEFAULT NULL  AFTER `kernelVersion` , ADD COLUMN `releaseID_FK` MEDIUMINT(8) UNSIGNED NOT NULL  AFTER `uKernelID` , DROP FOREIGN KEY `fk_kernels_os_revisions1` ;

ALTER TABLE `bugware0`.`kernels` 
  ADD CONSTRAINT `fk_kernels_os_releases1`
  FOREIGN KEY (`releaseID_FK` )
  REFERENCES `bugware0`.`os_releases` (`releaseID` )
  ON DELETE CASCADE
  ON UPDATE CASCADE
, DROP INDEX `fk_kernels_os_revisions1`
, ADD INDEX `fk_kernels_os_releases1` (`releaseID_FK` ASC) ;


SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;
