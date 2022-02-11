
INSERT INTO `pci_vendors` (`vendorCode`, `vendorName`) VALUES
('0000', 'Gammagraphx, Inc. (or missing ID)'),
('0010', 'Allied Telesis, Inc'),
('0070', 'Hauppauge computer works Inc.'),
('10de', 'nVidia Corporation');


INSERT INTO `pci_devices` (`deviceID`, `deviceCode`, `deviceName`, `vendorCode_FK`) VALUES
(1, '8139', 'AT-2500TX V3 Ethernet', '0010'),
(2, '0003', 'WinTV PVR-250', '0070'),
(3, '0807', 'WinTV PVR-150', '0070'),
(4, '4001', 'WinTV PVR-250 (v1)', '0070'),
(5, '4009', 'WinTV PVR-250', '0070'),
(6, '0008', 'NV1 [EDGE 3D]', '10de'),
(7, '0020', 'NV4 [RIVA TNT]', '10de');

INSERT INTO `pci_subsystems` (`uPcisubID`, `subvendor`, `subdevice`, `subSysName`, `deviceID_FK`) VALUES
(1, '1043', '0200', 'V3400 TNT', 7),
(2, '1048', '0c18', 'Erazor II SGRAM', 7),
(3, '1048', '0c19', 'Erazor II', 7),
(4, '1048', '0c1b', 'Erazor II', 7),
(5, '1048', '0c1c', 'Erazor II', 7),
(6, '1092', '0550', 'Viper V550', 7);
