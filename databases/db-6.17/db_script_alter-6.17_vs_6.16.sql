SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='TRADITIONAL';

DELIMITER $$

USE `bugware0`$$
CREATE DEFINER=`root`@`localhost` PROCEDURE `sp_queryClassName`(
IN class_code CHAR(2), IN subclass_code CHAR(2), IN progif_code CHAR(2) )
BEGIN
DECLARE findings TINYINT;
-- DECLARE class_code CHAR(6);

IF progif_code='' THEN
	SET progif_code = '00';
END IF;

CREATE TEMPORARY TABLE class_names (
	className VARCHAR(100),
	subclassName VARCHAR(100),
	progifName VARCHAR(100)
) ENGINE=MEMORY;

INSERT INTO class_names
SELECT className, subClassName, progifName FROM vw_pciClassList 
	WHERE classCode=class_code AND subClasscode=subclass_code AND progifCode=progif_code;

SELECT COUNT(*) INTO findings FROM class_names;

IF findings=0 THEN 
	INSERT INTO class_names
	SELECT className, subClassName, progifName FROM vw_pciClassList 
	WHERE classCode=class_code AND subClasscode=subclass_code;
END  IF;

SELECT * FROM class_names;

DROP TABLE class_names; 

END$$

USE `bugware0`$$
CREATE DEFINER=`root`@`localhost` PROCEDURE `sp_queryDeviceName`(
IN vendor_code CHAR(4), IN device_code CHAR(4), IN subvendor_code CHAR(2), IN subdevice_code CHAR(2) )
BEGIN
DECLARE findings TINYINT;

CREATE TEMPORARY TABLE device_names (
	vendorName VARCHAR(100),
	deviceName VARCHAR(150),
	subsysName VARCHAR(150)
) ENGINE=MEMORY;

INSERT INTO device_names
SELECT vendorName, deviceName, subSysName FROM vw_pciDeviceList 
	WHERE vendorCode=vendor_code AND deviceCode=device_code AND subvendor=subvendor_code AND subdevice=subdevice_code;

SELECT COUNT(*) INTO findings FROM device_names;

IF findings=0 THEN 
	INSERT INTO device_names
	SELECT vendorName, deviceName, subSysName=NULL FROM vw_pciDeviceList 
	  WHERE vendorCode=vendor_code AND deviceCode=device_code;

END  IF;

SELECT * FROM device_names;

DROP TABLE device_names; 


END$$


DELIMITER ;


SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;
