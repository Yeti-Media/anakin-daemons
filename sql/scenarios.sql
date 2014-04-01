DROP TABLE IF EXISTS "scenarios" CASCADE;

CREATE TABLE "scenarios" (
	id BIGSERIAL PRIMARY KEY,
 	descriptors xml NOT NULL
) WITH (
  OIDS=FALSE
);
ALTER TABLE "scenarios"
  OWNER TO postgres;
;
