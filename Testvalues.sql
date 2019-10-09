-- SQLite

begin transaction;

insert into users(userID, username, password) values (1, "Kassu", "hunter2");
insert into users(userID, username, password) values (2, "Player2", "passw0rd");

insert into matches(matchID, timestamp, playerCount, duration) values (1, datetime('now'), 2, 3600.0);

insert into match_records(userID, matchID, kills, deaths, placement) values (1, 1, 25, 10, 1);
insert into match_records(userID, matchID, kills, deaths, placement) values (2, 1, 10, 25, 2);

end transaction;