-- Table: "User_SerializedMatchers"

-- DROP TABLE "User_SerializedMatchers";

CREATE TABLE "User_SerializedMatchers"
(
  user_id text NOT NULL,
  smatcher_id text NOT NULL,
  CONSTRAINT user_serializedmatchers_pkey PRIMARY KEY (user_id, smatcher_id),
  CONSTRAINT "User_SerializedMatchers_smatcher_id_fkey" FOREIGN KEY (smatcher_id)
      REFERENCES "SerializedMatcher" (id) MATCH SIMPLE
      ON UPDATE CASCADE ON DELETE NO ACTION,
  CONSTRAINT "User_SerializedMatchers_user_id_fkey" FOREIGN KEY (user_id)
      REFERENCES "User" (id) MATCH SIMPLE
      ON UPDATE CASCADE ON DELETE CASCADE
)
WITH (
  OIDS=FALSE
);
ALTER TABLE "User_SerializedMatchers"
  OWNER TO postgres;