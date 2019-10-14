-- Command line:
-- sqlite3 < CreateDatabase.sql

.open game.db

pragma foreign_keys = ON;

drop trigger create_default_character;

drop table match_records;
drop table matches;
drop table characters;
drop table users;


-- Tables

create table users (
    userID      integer primary key,
    username    varchar(255) not null,
    password    char(255) not NULL
);

create table characters (
    characterID     integer primary key,
    userID          integer,
    colorR          float,
    colorG          float,
    colorB          float,
    foreign key (userID) references users(userID)         
);

create table matches (
    matchID     integer primary key,
    timestamp   datetime,
    playerCount integer,
    duration    float
);

create table match_records (
    recordID    integer primary key,
    userID    integer,
    matchID     integer,
    kills       integer,
    deaths      integer,
    placement   integer,
    foreign key(userID) references users(userID),
    foreign key(matchID) references matches(matchID)
);

-- Triggers

create trigger create_default_character 
after insert on users
begin 
    insert into characters(userID, colorR, colorG, colorB) 
    values (new.userID, 1.0, 1.0, 1.0);
end; 




.quit