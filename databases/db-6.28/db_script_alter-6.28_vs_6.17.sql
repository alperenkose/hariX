SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='TRADITIONAL';

CREATE  TABLE IF NOT EXISTS `bugware0`.`board_models` (
  `boardID` MEDIUMINT(8) UNSIGNED NOT NULL AUTO_INCREMENT ,
  `boardName` VARCHAR(100) NOT NULL ,
  PRIMARY KEY (`boardID`) )
ENGINE = InnoDB
DEFAULT CHARACTER SET = latin1
COLLATE = latin1_swedish_ci;

CREATE  TABLE IF NOT EXISTS `bugware0`.`dev_board` (
  `boardID_FK` MEDIUMINT(8) UNSIGNED NOT NULL ,
  `uDevID_FK` INT(10) UNSIGNED NOT NULL ,
  PRIMARY KEY (`boardID_FK`, `uDevID_FK`) ,
  INDEX `fk_dev_board_board_models1` (`boardID_FK` ASC) ,
  INDEX `fk_dev_board_all_devices1` (`uDevID_FK` ASC) ,
  CONSTRAINT `fk_dev_board_board_models1`
    FOREIGN KEY (`boardID_FK` )
    REFERENCES `bugware0`.`board_models` (`boardID` )
    ON DELETE CASCADE
    ON UPDATE CASCADE,
  CONSTRAINT `fk_dev_board_all_devices1`
    FOREIGN KEY (`uDevID_FK` )
    REFERENCES `bugware0`.`all_devices` (`uDevID` )
    ON DELETE NO ACTION
    ON UPDATE CASCADE)
ENGINE = InnoDB
DEFAULT CHARACTER SET = latin1
COLLATE = latin1_swedish_ci;


SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;
