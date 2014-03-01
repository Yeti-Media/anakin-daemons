-- Table: "SerializedMatcher"

-- DROP TABLE "SerializedMatcher";

CREATE TABLE "SerializedMatcher"
(
  id text NOT NULL,
  fbm_id int NOT NULL,
  index_id int NOT NULL,
  CONSTRAINT "SerializedMatcher_pkey" PRIMARY KEY (id)
)
WITH (
  OIDS=FALSE
);
ALTER TABLE "SerializedMatcher"
  OWNER TO postgres;