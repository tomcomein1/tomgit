PRAGMA foreign_keys=OFF;
BEGIN TRANSACTION;
CREATE TABLE sample ('id' INTEGER, 'name' TEXT);
INSERT INTO "sample" VALUES(1,'Raj');
COMMIT;
