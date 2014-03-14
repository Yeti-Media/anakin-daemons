-- Table: "Scene"

-- DROP TABLE "Scene";

CREATE TABLE "Scene"
(
  id text NOT NULL,
  descriptors text NOT NULL,
  keypts text NOT NULL,
  CONSTRAINT "Scene_pkey" PRIMARY KEY (id)
)
WITH (
  OIDS=FALSE
);
ALTER TABLE "Scene"
  OWNER TO postgres;
