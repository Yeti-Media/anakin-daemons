-- Table: "trainers"
DROP TABLE IF EXISTS "trainers" CASCADE;

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
  OWNER TO postgres;
