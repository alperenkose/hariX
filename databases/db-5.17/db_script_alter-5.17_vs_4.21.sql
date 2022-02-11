SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='TRADITIONAL';

CREATE  TABLE IF NOT EXISTS `bugware0`.`pcimap` (
  `uModID_FK` INT(10) UNSIGNED NOT NULL ,
  `vendor` CHAR(4) NOT NULL ,
  `device` CHAR(4) NOT NULL ,
  `subvendor` CHAR(4) NOT NULL ,
  `subdevice` CHAR(4) NOT NULL ,
  `class` CHAR(6) NOT NULL ,
  INDEX `fk_pcimap_modules1` (`uModID_FK` ASC) ,
  CONSTRAINT `fk_pcimap_modules1`
    FOREIGN KEY (`uModID_FK` )
    REFERENCES `bugware0`.`modules` (`uModID` )
    ON DELETE CASCADE
    ON UPDATE CASCADE)
ENGINE = InnoDB
DEFAULT CHARACTER SET = latin1
COLLATE = latin1_swedish_ci;

SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;
