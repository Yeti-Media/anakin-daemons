DROP TABLE IF EXISTS "Histogram" CASCADE;

CREATE TABLE "Histogram" (
  	"id" BIGSERIAL PRIMARY KEY,
  	"label" TEXT NOT NULL UNIQUE,
  	"color_data" XML,
	"gray_data" XML,
	"hsv_data" XML
	)
WITH (
  OIDS=FALSE
);
ALTER TABLE "Histogram"
  OWNER TO postgres;
;

