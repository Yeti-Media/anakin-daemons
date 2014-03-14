-- Table: "Smatcher_Patterns"

-- DROP TABLE "Smatcher_Patterns";

CREATE TABLE "Smatcher_Patterns"
(
  indexID text NOT NULL,
  patternID text NOT NULL,
  patternIDX int NOT NULL,
  CONSTRAINT smatcher_patterns_pkey PRIMARY KEY (indexID, patternID, patternIDX),
  CONSTRAINT "Smatcher_Patterns_indexid_fkey" FOREIGN KEY (indexID)
      REFERENCES "SerializedMatcher" (id) MATCH SIMPLE
      ON UPDATE CASCADE ON DELETE NO ACTION,
  CONSTRAINT "Smatcher_Patterns_patternID_fkey" FOREIGN KEY (patternID)
      REFERENCES "Pattern" (label) MATCH SIMPLE
      ON UPDATE CASCADE ON DELETE CASCADE
)
WITH (
  OIDS=FALSE
);
ALTER TABLE "Smatcher_Patterns"
  OWNER TO postgres;
