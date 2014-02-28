DROP TABLE IF EXISTS "Landscape" CASCADE;

CREATE TABLE "Landscape" (
  	"id" BIGSERIAL PRIMARY KEY,
  	"label" TEXT NOT NULL UNIQUE,
  	"color_data" XML,
	"gray_data" XML,
	"hsv_data" XML
	)
WITH (
  OIDS=FALSE
);
ALTER TABLE "Landscape"
  OWNER TO postgres;
;
