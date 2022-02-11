DELETE FROM `bugware0`.`kernels`;
DELETE FROM `bugware0`.`os_releases`;
DELETE FROM `bugware0`.`OSes`;
ALTER TABLE `bugware0`.`OSes` AUTO_INCREMENT = 1;
ALTER TABLE `bugware0`.`os_releases` AUTO_INCREMENT = 1;
ALTER TABLE `bugware0`.`kernels` AUTO_INCREMENT = 1;