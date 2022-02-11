
CALL sp_queryPcimap('8086','29b0','1734','10fc','0600',''); 	-- intel-agp CORRECT
CALL sp_queryPcimap('8086','29b1','','','0604','');			-- shpchp CORRECT	
CALL sp_queryPcimap('8086','29b4','1734','10fc','0780','');	-- YOK CORRECT
CALL sp_queryPcimap('8086','29b6','1734','10fc','0101','85');	-- matches 3 CORRECT
CALL sp_queryPcimap('8086','29b7','1734','10fc','0700','02');	-- serial olarak kodda yapman gerek 0700'u!!
CALL sp_queryPcimap('8086','10bd','1734','10fd','0200','');	-- e1000e CORRECT
CALL sp_queryPcimap('8086','2937','1734','10fd','0c03','');	-- uhci-hcd CORRECT
CALL sp_queryPcimap('8086','2938','1734','10fd','0c03','');	-- uhci-hcd CORRECT
CALL sp_queryPcimap('8086','2939','1734','10fd','0c03','');	-- uhci-hcd CORRECT
CALL sp_queryPcimap('8086','293c','1734','10fd','0c03','20');	-- ehci-hcd CORRECT
CALL sp_queryPcimap('8086','293e','1734','10fd','0403','');	-- snd-hda-intel CORRECT
CALL sp_queryPcimap('8086','2940','','','0604','');			-- shpchp CORRECT
CALL sp_queryPcimap('8086','2948','','','0604','');			-- shpchp CORRECT
CALL sp_queryPcimap('8086','2934','1734','10fd','0c03','');	-- uhci-hcd CORRECT
CALL sp_queryPcimap('8086','2935','1734','10fd','0c03','');	-- uhci-hcd CORRECT
CALL sp_queryPcimap('8086','2936','1734','10fd','0c03','');	-- uhci-hcd CORRECT
CALL sp_queryPcimap('8086','293a','1734','10fd','0c03','20');	-- ehci-hcd CORRECT
CALL sp_queryPcimap('8086','244e','','','0604','01');			-- lspci'dada YOK CORRECT
CALL sp_queryPcimap('8086','2914','1734','10fd','0601','');	-- iTCO_wdt CORRECT
CALL sp_queryPcimap('8086','2922','1734','10fd','0106','01'); -- ahci CORRECT
CALL sp_queryPcimap('8086','2930','1734','10fd','0c05','');	-- i2c-i801 CORRECT
CALL sp_queryPcimap('10de','029e','10de','032c','0300','');	-- nvidia CORRECT
