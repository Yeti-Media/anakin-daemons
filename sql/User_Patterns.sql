DROP TABLE IF EXISTS "User_Patterns" CASCADE;

CREATE TABLE "User_Patterns"(
 	user_id    TEXT references public."User" (id) ON UPDATE CASCADE ON DELETE CASCADE
	,pattern_id BIGSERIAL references public."Pattern" (id) ON UPDATE CASCADE
	,CONSTRAINT user_patterns_pkey PRIMARY KEY (user_id,pattern_id)  -- explicit pk
)WITH (
  OIDS=FALSE
);
ALTER TABLE "User_Patterns"
  OWNER TO postgres;
;
