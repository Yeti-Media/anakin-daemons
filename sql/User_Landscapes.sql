DROP TABLE IF EXISTS "User_Landscapes" CASCADE;

CREATE TABLE "User_Landscapes"(
 	user_id    TEXT references public."User" (id) ON UPDATE CASCADE ON DELETE CASCADE
	,landscape_id BIGSERIAL references public."Landscape" (id) ON UPDATE CASCADE
	,CONSTRAINT user_landscapes_pkey PRIMARY KEY (user_id,landscape_id)  -- explicit pk
)WITH (
  OIDS=FALSE
);
ALTER TABLE "User_Landscapes"
  OWNER TO postgres;
;
