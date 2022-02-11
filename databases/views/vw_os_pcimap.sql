CREATE VIEW `bugware0`.`vw_os_pcimap` AS
  SELECT mdl.uKernelID_FK, mdl.uModID, vendor, device, subvendor, subdevice, class, classMask, mdl.modNameID_FK
	FROM pcimap RIGHT JOIN modules mdl
		ON pcimap.uModID_FK = mdl.uModID; 
