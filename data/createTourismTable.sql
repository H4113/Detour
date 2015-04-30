DROP TABLE tourism;

CREATE TABLE tourism (
	id integer CONSTRAINT pkTourism PRIMARY KEY,
	typ varchar(32) not null,
	typ_detail varchar(64) not null,
	nom varchar(64) not null,
	adresse varchar(64) not null,
	codepostal varchar(16) not null,
	commune varchar(32) not null,
	ouverture varchar(64),
	longitude double precision,
	latitude double precision
);

