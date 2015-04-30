DROP TABLE tourism;

CREATE TABLE tourism (
	id integer CONSTRAINT pkTourism PRIMARY KEY,
	typ varchar(32) not null,
	typ_detail varchar(1024) not null,
	nom varchar(1024) not null,
	adresse varchar(1024) not null,
	codepostal varchar(16) not null,
	commune varchar(32) not null,
	ouverture varchar(1024) not null,
	longitude double precision,
	latitude double precision
);

