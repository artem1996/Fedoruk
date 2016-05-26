DROP TABLE IF EXISTS elements;
CREATE TABLE elements (
  id smallint(6) NOT NULL default '0',
  n1 smallint(6) NOT NULL default '0',
  n2 smallint(6) NOT NULL default '0',
  n3 smallint(6) NOT NULL default '0',
  props char(12) NOT NULL default 'steel',
  PRIMARY KEY  (id)
);

LOCK TABLES elements WRITE;
INSERT INTO elements VALUES (1,2,3,5,'steel'),(2,1,2,4,'steel'),(3,2,5,4,'steel'),
	(4,5,6,4,'steel'),(5,5,7,6,'steel'),(6,5,8,7,'steel'),(7,8,9,7,'steel'),(8,8,10,9,'steel'),
	(9,10,11,9,'steel'),(10,10,12,11,'steel'),(11,12,13,11,'steel'),(12,12,14,13,'steel'),
	(13,12,15,14,'steel'),(14,14,18,13,'steel'),(15,15,16,14,'steel'),(16,16,17,14,'steel'),
	(17,14,17,18,'steel'),(18,16,20,17,'steel'),(19,17,19,18,'steel'),(20,20,19,17,'steel'),
	(21,20,21,19,'steel'),(22,19,21,23,'steel'),(23,20,22,21,'steel'),(24,22,24,21,'steel'),
	(25,21,24,23,'steel'),(26,28,27,22,'steel'),(27,27,29,26,'steel'),(28,27,26,22,'steel'),
	(29,24,26,25,'steel'),(30,24,25,23,'steel');
UNLOCK TABLES;

DROP TABLE IF EXISTS loadings;
CREATE TABLE loadings (
  type char(1) NOT NULL default '',
  direction char(1) default NULL,
  node smallint(6) NOT NULL default '0',
  value float default NULL,
  KEY key_node (node)
);

LOCK TABLES loadings WRITE;
INSERT INTO loadings VALUES ('r','x',1,NULL),('r','x',2,NULL),('r','x',3,NULL),
	('h',NULL,14,NULL),('f','x',27,-10),('f','y',27,-50);
UNLOCK TABLES;

DROP TABLE IF EXISTS materials;
CREATE TABLE materials (
  name char(12) NOT NULL default '',
  density float NOT NULL default '0',
  elastics float NOT NULL default '0',
  poisson float NOT NULL default '0',
  strength float NOT NULL default '0',
  PRIMARY KEY  (name)
);

LOCK TABLES materials WRITE;
INSERT INTO materials VALUES ('steel',7.8,200,0.25,1000),
	('aluminium',2.7,65,0.34,600),('concrete',5.6,25,0.12,300),
	('duraluminium',2.8,70,0.31,700),('titanium',4.5,116,0.32,950),
	('brass',8.5,93,0.37,300);
UNLOCK TABLES;

DROP TABLE IF EXISTS nodes;
CREATE TABLE nodes (
  id smallint(6) NOT NULL default '0',
  x float NOT NULL default '0',
  y float NOT NULL default '0',
  PRIMARY KEY  (id)
);

LOCK TABLES nodes WRITE;
INSERT INTO nodes VALUES (1,-95,20),(2,-87.5,20),(3,-80,20),(4,-95,10),
	(5,-80,15),(6,-85,-1),(7,-75,-3),(8,-65,15),(9,-55,-6),(10,-40,15),(11,-35,-10),
	(12,-15,15),(13,-15,-14),(14,0,0),(15,5,20),(16,20,8),(17,20,-10),(18,10,-20),
	(19,30,-27),(20,40,-3),(21,50,-25),(22,60,-15),(23,60,-39),(24,65,-25),(25,75,-35),
	(26,80,-20),(27,75,-7),(28,65,-5),(29,83,-9);
UNLOCK TABLES;