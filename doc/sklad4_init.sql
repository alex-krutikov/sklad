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

insert  into `types`(`id`,`typename`,`symbol`,`used`) values (1,'Модуль','A',1),(2,'Преобразователь','B',0),(3,'Громкоговоритель','BA',0),(4,'Магнитострикционный элемент','BB',0),(5,'Детектор ионизирующих излучений','BD',0),(6,'Сельсин-приемник','BE',0),(7,'Телефон (капсюль)','BF',0),(8,'Сельсин-датчик','BC',0),(9,'Тепловой датчик','BK',0),(10,'Фотоэлемент','BL',1),(11,'Микрофон','BM',0),(12,'Датчик давления','BP',0),(13,'Пьезоэлемент','BQ',0),(14,'Тахогенератор','BR',0),(15,'Звукосниматель','BS',0),(16,'Датчик скорости','BV',0),(17,'Конденсатор','C',1),(18,'Микросборка','D',0),(19,'М/с аналоговая','DA',1),(20,'М/с цифровая','DD',1),(21,'М/с памяти','DS',1),(22,'Устройство задержки','DT',0),(23,'Элементы разные','E',1),(24,'Нагревательный элемент','EK',0),(25,'Лампа осветительная','EL',0),(26,'Пиропатрон','ET',0),(27,'Устройства защитные','F',1),(28,'Предохранитель по току','FA',0),(29,'Предохранитель по току инерционный','FP',0),(30,'Предохранитель','FU',1),(32,'Источник питания','G',1),(33,'Батарея','GB',1),(34,'Индикатор','H',1),(35,'Прибор звуковой сигнализации','HA',0),(36,'Индикатор символьный','HG',1),(37,'Лампа','HL',1),(38,'Реле','K',1),(39,'Реле токовое','KA',0),(40,'Реле указательное','KH',0),(41,'Реле электротепловое','KK',0),(42,'Пускатель','KM',0),(43,'Реле времени','KT',0),(44,'Реле напряжения','KV',0),(45,'Дроссель','L',1),(46,'Дроссель люминисцентного освещения','LL',0),(47,'Двигатели','M',0),(48,'Приборы','P',0),(49,'Амперметр','PA',0),(50,'Счетчик импульсов','PC',0),(51,'Частотомер','PF',0),(52,'Счетчик активной энергии','PI',0),(53,'Счетчик реактивной энергии','PK',0),(54,'Омметр','PR',0),(55,'Регистрирующий прибор','PS',0),(56,'Часы','PT',0),(57,'Вольтметр','PV',0),(58,'Ваттметр','PW',0),(59,'Выключатель','Q',1),(60,'Выключатель автоматический','QF',0),(61,'Короткозамыкатель','QK',0),(62,'Разъединитель','QS',0),(63,'Резистор','R',1),(64,'Терморезистор','RK',0),(65,'Потенциометр','RP',0),(66,'Шунт измерительный','RS',1),(67,'Варистор','RU',1),(68,'Коммутатор','S',1),(69,'Переключатель','SA',1),(70,'Кнопка','SB',1),(71,'Автомат','SF',1),(72,'Выключатель от уровня','SL',0),(73,'Выключатель от давления','SP',0),(74,'Выключатель от положения','SQ',0),(75,'Выключатель от частоты вращения','SQ',0),(76,'Выключатель от температуры','SK',0),(77,'Трансформатор','T',1),(78,'Трансформатор тока','TA',1),(79,'Э/м стабилизатор','TS',0),(80,'Трансформатор напряжения','TV',1),(81,'Устройства связи','U',0),(82,'Модулятор','UB',0),(83,'Демодулятор','UR',0),(84,'Дискриминатор','UI',0),(85,'Преобразователь частотный','UZ',0),(86,'Прибор электровакуумный и полупроводниковый','V',0),(87,'Диод, стабилитрон','VD',1),(88,'Прибор электровакуумный','VL',0),(89,'Транзистор','VT',1),(90,'Тиристор','VS',1),(91,'Оптрон','VU',1),(92,'Антенна','W',0),(93,'Ответвитель','WE',0),(94,'Короткозамыкатель','WK',0),(95,'Вентиль','WS',0),(96,'Трансформатор','WT',0),(97,'Аттенюатор','WU',0),(99,'Соединение контактное','X',1),(100,'Токосъемник','XA',0),(101,'Разъём (штырь)','XP',1),(102,'Разъём (гнездо)','XS',1),(103,'Соединение разборное','XT',1),(104,'Соединитель в/ч','XW',0),(105,'Э/м привод','Y',0),(106,'Электромагнит','YA',0),(107,'Тормоз с э/м приводом','YB',0),(108,'Муфта с э/м приводом','YC',0),(109,'Э/м патрон или плита','YH',0),(110,'Устройство оконечное','Z',0),(111,'Ограничитель','ZL',1),(112,'Кварцевый резонатор','ZQ',1);
