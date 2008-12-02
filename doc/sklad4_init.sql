SET NAMES cp1251;
DROP DATABASE IF EXISTS `sklad4`;
CREATE DATABASE `sklad4` DEFAULT CHARACTER SET utf8;
USE `sklad4`;

/*Table structure for table `bom2sklad` */

DROP TABLE IF EXISTS `bom2sklad`;

CREATE TABLE `bom2sklad` (
  `id` int(11) NOT NULL auto_increment,
  `name_bom` char(30) default NULL,
  `name_sklad` char(30) default NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;

/*Table structure for table `izdelie` */

DROP TABLE IF EXISTS `izdelie`;

CREATE TABLE `izdelie` (
  `id` int(11) NOT NULL auto_increment,
  `zakaz` int(11) default NULL,
  `name` varchar(60) default NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;

/*Table structure for table `kompl` */

DROP TABLE IF EXISTS `kompl`;

CREATE TABLE `kompl` (
  `id` int(11) NOT NULL auto_increment,
  `sostav` int(11) default NULL,
  `name` char(30) default NULL,
  `nominal` char(30) default NULL,
  `type` int(11) default NULL,
  `n` int(11) default NULL,
  `need` int(11) default NULL,
  `snato` int(11) default NULL,
  `status` int(11) default NULL,
  `position` tinytext,
  `color` int(11) default NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;

/*Table structure for table `postavshiki` */

DROP TABLE IF EXISTS `postavshiki`;

CREATE TABLE `postavshiki` (
  `id` int(11) NOT NULL auto_increment,
  `name` char(30) default NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;

/*Table structure for table `price_est` */

DROP TABLE IF EXISTS `price_est`;

CREATE TABLE `price_est` (
  `name` char(30) NOT NULL,
  `price_est` float default NULL,
  PRIMARY KEY  (`name`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;

/*Table structure for table `prihod` */

DROP TABLE IF EXISTS `prihod`;

CREATE TABLE `prihod` (
  `id` int(11) NOT NULL auto_increment,
  `zakupka` int(11) default NULL,
  `name` char(30) default NULL,
  `kod` char(30) default NULL,
  `type` int(11) default NULL,
  `proizvoditel` int(11) default NULL,
  `kolichestvo` int(11) default NULL,
  `ostatok` int(11) NOT NULL,
  `price` double default NULL,
  `mesto` char(12) default NULL,
  `naklad` varchar(60) default NULL,
  `user` int(11) default NULL,
  `date` date default NULL,
  `user2` int(11) default NULL,
  `date2` date default NULL,
  `notes` mediumtext,
  PRIMARY KEY  (`id`),
  FULLTEXT KEY `name` (`name`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;

/*Table structure for table `proekt` */

DROP TABLE IF EXISTS `proekt`;

CREATE TABLE `proekt` (
  `id` int(11) NOT NULL auto_increment,
  `name` varchar(60) default NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;

/*Table structure for table `proizvoditeli` */

DROP TABLE IF EXISTS `proizvoditeli`;

CREATE TABLE `proizvoditeli` (
  `id` int(11) NOT NULL auto_increment,
  `name` char(30) default NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;

/*Table structure for table `rashod` */

DROP TABLE IF EXISTS `rashod`;

CREATE TABLE `rashod` (
  `id` int(11) NOT NULL auto_increment,
  `zamena` int(11) default NULL,
  `prihod` int(11) default NULL,
  `n` int(11) default NULL,
  `user` int(11) default NULL,
  `date` date default NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

/*Table structure for table `sostav` */

DROP TABLE IF EXISTS `sostav`;

CREATE TABLE `sostav` (
  `id` int(11) NOT NULL auto_increment,
  `izdelie` int(11) default NULL,
  `name` varchar(60) default NULL,
  `n1` int(11) default NULL,
  `color` int(11) default NULL,
  `prioritet` int(11) default NULL,
  `date` date default NULL,
  `status2` char(20) default NULL,
  `zakupka_status` char(10) default NULL,
  `notes` varchar(60) default NULL,
  `cost_kompl` char(10) default NULL,
  `cost_plata` char(10) default NULL,
  `cost_montag` char(10) default NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

/*Table structure for table `types` */

DROP TABLE IF EXISTS `types`;

CREATE TABLE `types` (
  `id` int(11) NOT NULL auto_increment,
  `typename` varchar(80) default NULL,
  `symbol` char(40) default NULL,
  `used` int(1) default NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;

/*Table structure for table `users` */

DROP TABLE IF EXISTS `users`;

CREATE TABLE `users` (
  `id` int(11) NOT NULL auto_increment,
  `nickname` char(30) character set utf8 collate utf8_bin NOT NULL,
  `username` varchar(60) NOT NULL,
  `passw` varchar(30) character set utf8 collate utf8_bin NOT NULL,
  `permissions` int(11) default NULL,
  PRIMARY KEY  (`id`,`nickname`,`username`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;

/*Table structure for table `zakaz` */

DROP TABLE IF EXISTS `zakaz`;

CREATE TABLE `zakaz` (
  `id` int(11) NOT NULL auto_increment,
  `proekt` int(11) default NULL,
  `name` varchar(60) default NULL,
  `date` date default NULL,
  `user` int(11) default NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;

/*Table structure for table `zakupki` */

DROP TABLE IF EXISTS `zakupki`;

CREATE TABLE `zakupki` (
  `id` int(11) NOT NULL auto_increment,
  `name` char(30) default NULL,
  `type` int(11) default NULL,
  `postavshik` int(11) default NULL,
  `schet` varchar(60) default NULL,
  `n` int(11) NOT NULL default '0',
  `polucheno` int(11) NOT NULL default '0',
  `platej` varchar(60) default NULL,
  `date` date default NULL,
  `user` int(11) default NULL,
  `date2` date default NULL,
  `user2` int(11) default NULL,
  `color` int(11) default NULL,
  `notes` longtext,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;

/*Table structure for table `zamena` */

DROP TABLE IF EXISTS `zamena`;

CREATE TABLE `zamena` (
  `id` int(11) NOT NULL auto_increment,
  `kompl` int(11) default NULL,
  `name` char(30) default NULL,
  `snato` int(11) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  KEY `kompl` (`kompl`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;

insert  into `types`(`id`,`typename`,`symbol`,`used`) values (1,'������','A',1),(2,'���������������','B',0),(3,'����������������','BA',0),(4,'������������������� �������','BB',0),(5,'�������� ������������ ���������','BD',0),(6,'�������-��������','BE',0),(7,'������� (�������)','BF',0),(8,'�������-������','BC',0),(9,'�������� ������','BK',0),(10,'�����������','BL',1),(11,'��������','BM',0),(12,'������ ��������','BP',0),(13,'������������','BQ',0),(14,'�������������','BR',0),(15,'��������������','BS',0),(16,'������ ��������','BV',0),(17,'�����������','C',1),(18,'�����������','D',0),(19,'�/� ����������','DA',1),(20,'�/� ��������','DD',1),(21,'�/� ������','DS',1),(22,'���������� ��������','DT',0),(23,'�������� ������','E',1),(24,'�������������� �������','EK',0),(25,'����� �������������','EL',0),(26,'����������','ET',0),(27,'���������� ��������','F',1),(28,'�������������� �� ����','FA',0),(29,'�������������� �� ���� �����������','FP',0),(30,'��������������','FU',1),(32,'�������� �������','G',1),(33,'�������','GB',1),(34,'���������','H',1),(35,'������ �������� ������������','HA',0),(36,'��������� ����������','HG',1),(37,'�����','HL',1),(38,'����','K',1),(39,'���� �������','KA',0),(40,'���� ������������','KH',0),(41,'���� ���������������','KK',0),(42,'���������','KM',0),(43,'���� �������','KT',0),(44,'���� ����������','KV',0),(45,'��������','L',1),(46,'�������� ��������������� ���������','LL',0),(47,'���������','M',0),(48,'�������','P',0),(49,'���������','PA',0),(50,'������� ���������','PC',0),(51,'����������','PF',0),(52,'������� �������� �������','PI',0),(53,'������� ���������� �������','PK',0),(54,'������','PR',0),(55,'�������������� ������','PS',0),(56,'����','PT',0),(57,'���������','PV',0),(58,'��������','PW',0),(59,'�����������','Q',1),(60,'����������� ��������������','QF',0),(61,'�����������������','QK',0),(62,'�������������','QS',0),(63,'��������','R',1),(64,'�������������','RK',0),(65,'������������','RP',0),(66,'���� �������������','RS',1),(67,'��������','RU',1),(68,'����������','S',1),(69,'�������������','SA',1),(70,'������','SB',1),(71,'�������','SF',1),(72,'����������� �� ������','SL',0),(73,'����������� �� ��������','SP',0),(74,'����������� �� ���������','SQ',0),(75,'����������� �� ������� ��������','SQ',0),(76,'����������� �� �����������','SK',0),(77,'�������������','T',1),(78,'������������� ����','TA',1),(79,'�/� ������������','TS',0),(80,'������������� ����������','TV',1),(81,'���������� �����','U',0),(82,'���������','UB',0),(83,'�����������','UR',0),(84,'�������������','UI',0),(85,'��������������� ���������','UZ',0),(86,'������ ���������������� � �����������������','V',0),(87,'����, �����������','VD',1),(88,'������ ����������������','VL',0),(89,'����������','VT',1),(90,'��������','VS',1),(91,'������','VU',1),(92,'�������','W',0),(93,'�����������','WE',0),(94,'�����������������','WK',0),(95,'�������','WS',0),(96,'�������������','WT',0),(97,'����������','WU',0),(99,'���������� ����������','X',1),(100,'�����������','XA',0),(101,'������ (�����)','XP',1),(102,'������ (������)','XS',1),(103,'���������� ���������','XT',1),(104,'����������� �/�','XW',0),(105,'�/� ������','Y',0),(106,'�������������','YA',0),(107,'������ � �/� ��������','YB',0),(108,'����� � �/� ��������','YC',0),(109,'�/� ������ ��� �����','YH',0),(110,'���������� ���������','Z',0),(111,'������������','ZL',1),(112,'��������� ���������','ZQ',1);
