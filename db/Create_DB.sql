CREATE DATABASE cBotDB CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci;
USE cBotDB;
CREATE TABLE usersTable
(
userId bigint,
chatId bigint
);
USE cBotDB;
CREATE TABLE blockTable
(
userId BIGINT,
blockNumber SMALLINT,
blockName VARCHAR(255),
blockInfo MEDIUMTEXT,
PRIMARY KEY (userId)
);