DROP TABLE IF EXISTS "User_Histograms" CASCADE;

CREATE TABLE "User_Histograms"(
 	user_id    TEXT references public."User" (id) ON UPDATE CASCADE ON DELETE CASCADE
	,histogram_id BIGSERIAL references public."Histogram" (id) ON UPDATE CASCADE
	,CONSTRAINT user_histograms_pkey PRIMARY KEY (user_id,histogram_id)  -- explicit pk
)WITH (
  OIDS=FALSE
);
ALTER TABLE "User_Histograms"
  OWNER TO postgres;
;
