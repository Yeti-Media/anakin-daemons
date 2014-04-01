DROP TABLE IF EXISTS "descriptors" CASCADE;

CREATE TABLE "descriptors" (
	id BIGSERIAL PRIMARY KEY,
 	body xml NOT NULL,
	pattern_id int NOT NULL,
	CONSTRAINT "descriptors_pattern_id_fkey" FOREIGN KEY (pattern_id)
      		REFERENCES "patterns" (id) MATCH SIMPLE
      		ON UPDATE CASCADE ON DELETE NO ACTION
) WITH (
  OIDS=FALSE
);
ALTER TABLE "descriptors"
  OWNER TO postgres;
;
