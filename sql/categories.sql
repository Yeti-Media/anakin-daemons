DROP TABLE IF EXISTS "categories" CASCADE;

CREATE TABLE "categories" (
	id BIGSERIAL PRIMARY KEY,
	title text UNIQUE NOT NULL)
WITH (
  OIDS=FALSE
);
ALTER TABLE "categories"
  OWNER TO postgres;
;
