DELETE FROM `bugware0`.`pci_subsystems`;
DELETE FROM `bugware0`.`pci_devices`;
DELETE FROM `bugware0`.`pci_vendors`;
ALTER TABLE `bugware0`.`pci_vendors` AUTO_INCREMENT = 1;
ALTER TABLE `bugware0`.`pci_devices` AUTO_INCREMENT = 1;
ALTER TABLE `bugware0`.`pci_subsystems` AUTO_INCREMENT = 1;