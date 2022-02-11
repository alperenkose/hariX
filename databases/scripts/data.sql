SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;










INSERT INTO `pci_classes` (`classCode`, `className`) VALUES
('00', 'Unclassified device'),
('01', 'Mass storage controller');


INSERT INTO `pci_prog_ifs` (`uProgifID`, `progifCode`, `progifName`, `subClassID_FK`) VALUES
(1, '00', NULL, 3),
(3, '00', NULL, 2),
(4, '00', NULL, 5),
(5, '00', NULL, 4),
(6, '30', 'ADMA continuous operation', 6),
(7, '20', 'ADMA single stepping', 6),
(12, '00', NULL, 7);

INSERT INTO `pci_subclasses` (`subClassID`, `subClassCode`, `subClassName`, `classCode_FK`) VALUES
(2, '01', 'VGA compatible', '00'),
(3, '00', 'Non-VGA unclassified', '00'),
(4, '00', 'SCSI storage controller', '01'),
(5, '01', 'IDE interface', '01'),
(6, '05', 'ATA controller', '01'),
(7, '07', 'Serial Attached SCSI Controller', '01');



INSERT INTO `vw_pciClassList` (`classCode`, `subClassCode`, `progifCode`, `progifName`, `subClassName`, `className`, `subClassID`, `uProgifID`) VALUES
('00', '01', '00', NULL, 'VGA compatible', 'Unclassified device', 2, 3),
('00', '00', '00', NULL, 'Non-VGA unclassified', 'Unclassified device', 3, 1),
('01', '00', '00', NULL, 'SCSI storage controller', 'Mass storage controller', 4, 5),
('01', '01', '00', NULL, 'IDE interface', 'Mass storage controller', 5, 4),
('01', '05', '30', 'ADMA continuous operation', 'ATA controller', 'Mass storage controller', 6, 6),
('01', '05', '20', 'ADMA single stepping', 'ATA controller', 'Mass storage controller', 6, 7),
('01', '07', '00', NULL, 'Serial Attached SCSI Controller', 'Mass storage controller', 7, 12);
