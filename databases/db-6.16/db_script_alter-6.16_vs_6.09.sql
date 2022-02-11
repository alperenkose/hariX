SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='TRADITIONAL';

DELIMITER $$

USE `bugware0`;
DROP procedure IF EXISTS `bugware0`.`sp_pciProgifsAdd`;
$$

USE `bugware0`$$
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


SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;
