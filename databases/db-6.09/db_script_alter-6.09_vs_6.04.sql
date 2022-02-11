SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='TRADITIONAL';

ALTER TABLE `bugware0`.`pcimap` ADD COLUMN `classMask` CHAR(6) NOT NULL  AFTER `class` ;

-- -----------------------------------------------------
-- Placeholder table for view `bugware0`.`vw_os_pcimap`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `bugware0`.`vw_os_pcimap` (`uKernelID_FK` INT, `uModID` INT, `vendor` INT, `device` INT, `subvendor` INT, `subdevice` INT, `class` INT, `modNameID_FK` INT);

USE `bugware0`;

-- -----------------------------------------------------
-- View `bugware0`.`vw_os_pcimap`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `bugware0`.`vw_os_pcimap`;
CREATE  OR REPLACE ALGORITHM=UNDEFINED DEFINER=`root`@`localhost` SQL SECURITY DEFINER VIEW `bugware0`.`vw_os_pcimap` AS select `mdl`.`uKernelID_FK` AS `uKernelID_FK`,`mdl`.`uModID` AS `uModID`,`bugware0`.`pcimap`.`vendor` AS `vendor`,`bugware0`.`pcimap`.`device` AS `device`,`bugware0`.`pcimap`.`subvendor` AS `subvendor`,`bugware0`.`pcimap`.`subdevice` AS `subdevice`,`bugware0`.`pcimap`.`class` AS `class`,`mdl`.`modNameID_FK` AS `modNameID_FK` from (`bugware0`.`modules` `mdl` left join `bugware0`.`pcimap` on((`bugware0`.`pcimap`.`uModID_FK` = `mdl`.`uModID`)));

DELIMITER $$

USE `bugware0`$$
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


SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;
