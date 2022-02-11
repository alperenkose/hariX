-- --------------------------------------------------------------------------------
-- Routine DDL
-- --------------------------------------------------------------------------------
DELIMITER $$

CREATE PROCEDURE `bugware0`.`sp_queryPcimap`(
IN vendor_code CHAR(4), IN device_code CHAR(4), IN subvendor_code CHAR(4), IN subdevice_code CHAR(4), IN class_sub CHAR(4), IN prog_if CHAR(2)  )
BEGIN

DECLARE class_code CHAR(6);
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

IF (SELECT COUNT(*) FROM pcimap_devices)>0 THEN 
	SELECT * FROM pcimap_devices;
	-- DO SOMETHING!!
ELSE
	SET class_code = CONCAT ( class_sub , '00');
	INSERT INTO pcimap_devices
	SELECT uKernelID_FK, modNameID_FK, vendor, device, subvendor, subdevice FROM vw_os_pcimap 
	WHERE class=class_code AND (classMask='ffff00' OR classMask='ff0000');

	IF (SELECT COUNT(*) FROM pcimap_devices)>0 THEN 
		SELECT * FROM pcimap_devices;
		-- DO SOMETHING!!
	ELSE
		SET class_code = '000000';
		INSERT INTO pcimap_devices
		SELECT uKernelID_FK, modNameID_FK, vendor, device, subvendor, subdevice FROM vw_os_pcimap 
		WHERE class=class_code;
		SELECT * FROM pcimap_devices;
	END IF;
END  IF;

DROP TABLE pcimap_devices; 

END$$