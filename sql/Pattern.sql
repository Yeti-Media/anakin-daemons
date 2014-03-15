DROP TABLE IF EXISTS "Pattern" CASCADE;

CREATE TABLE "Pattern" (
  "id" BIGSERIAL PRIMARY KEY,
  "label" TEXT NOT NULL UNIQUE,
  "data" XML NOT NULL)
WITH (
  OIDS=FALSE
);
ALTER TABLE "Pattern"
  OWNER TO postgres;
;
