---------------------------------------------------------------------------------------------------------------
-- drop old tables

DROP DATABASE IF EXISTS anakin;
CREATE DATABASE anakin;
\c anakin

---------------------------------------------------------------------------------------------------------------
CREATE TABLE "users" (
  id BIGSERIAL PRIMARY KEY
)WITH (
  OIDS=FALSE
);
ALTER TABLE "users"
  OWNER TO postgres
;

---------------------------------------------------------------------------------------------------------------  
CREATE TABLE "trainers" (   
  id BIGSERIAL PRIMARY KEY,
  xml_file int NOT NULL,
  if_file int NOT NULL,
  user_id int NOT NULL,
  CONSTRAINT "trainers_user_id_fkey" FOREIGN KEY (user_id)
          REFERENCES "users" (id) MATCH SIMPLE
          ON UPDATE CASCADE ON DELETE NO ACTION
) WITH (
  OIDS=FALSE
);
ALTER TABLE "trainers"
  OWNER TO postgres
;
  
---------------------------------------------------------------------------------------------------------------
CREATE TABLE "categories" (
    id BIGSERIAL PRIMARY KEY,
    title text UNIQUE NOT NULL)
WITH (
  OIDS=FALSE
);
ALTER TABLE "categories"
  OWNER TO postgres
;

INSERT INTO "categories"(title) VALUES ('matching');
INSERT INTO "categories"(title) VALUES ('comparison');
INSERT INTO "categories"(title) VALUES ('landscape');
---------------------------------------------------------------------------------------------------------------
CREATE TABLE "patterns" (
    id BIGSERIAL PRIMARY KEY,
    user_id int NOT NULL,
    category_id int NOT NULL,
    trainer_id int,
    position int,
    CONSTRAINT "patterns_user_id_fkey" FOREIGN KEY (user_id)
            REFERENCES "users" (id) MATCH SIMPLE
            ON UPDATE CASCADE ON DELETE NO ACTION,
    CONSTRAINT "patterns_category_id_fkey" FOREIGN KEY (category_id)
            REFERENCES "categories" (id) MATCH SIMPLE
            ON UPDATE CASCADE ON DELETE NO ACTION,
    CONSTRAINT "patterns_trainer_id_fkey" FOREIGN KEY (trainer_id)
            REFERENCES "trainers" (id) MATCH SIMPLE
            ON UPDATE CASCADE ON DELETE NO ACTION
) WITH (
  OIDS=FALSE
);
ALTER TABLE "patterns"
  OWNER TO postgres
;
  
---------------------------------------------------------------------------------------------------------------  
CREATE TABLE "scenarios" (
    id BIGSERIAL PRIMARY KEY,
    descriptors_file int NOT NULL
) WITH (
  OIDS=FALSE
);
ALTER TABLE "scenarios"
  OWNER TO postgres
;

---------------------------------------------------------------------------------------------------------------
CREATE TABLE "descriptors" (
    id BIGSERIAL PRIMARY KEY,
    body_file int NOT NULL,
    pattern_id int NOT NULL,
    CONSTRAINT "descriptors_pattern_id_fkey" FOREIGN KEY (pattern_id)
            REFERENCES "patterns" (id) MATCH SIMPLE
            ON UPDATE CASCADE ON DELETE NO ACTION
) WITH (
  OIDS=FALSE
);
ALTER TABLE "descriptors"
  OWNER TO postgres
;
