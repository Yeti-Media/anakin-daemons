DROP TABLE IF EXISTS "patterns" CASCADE;

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
  OWNER TO postgres;
;

