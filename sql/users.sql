﻿DROP TABLE IF EXISTS "users" CASCADE;

CREATE TABLE "users" (
  id BIGSERIAL PRIMARY KEY
)WITH (
  OIDS=FALSE
);
ALTER TABLE "users"
  OWNER TO postgres;
;
