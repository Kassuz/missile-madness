-- Get all users
select * from users;

-- Create new user
insert into users(username, password) values(?, ?);

-- Get user by username
select userID, username, password from users where username=?;

-- Get and set user color
select colorR, colorG, colorB from characters where userID=?;
update characters set colorR=?, colorG=?, colorB=? where userID=?;

-- Create new match
insert into matches(timestamp, playerCount, duration) values(?, ?, ?);

-- Create match record
insert into match_records(userID, matchID, kills, deaths, placement) values(?, ?, ?, ?, ?);

-- Get most recent match
select matchID, max(timestamp) from matches;

-- Get all match records for specific match ordered by placement
select placement, users.username, kills, deaths 
from match_records, users 
where users.userID=match_records.userID and matchID = ? 
order by placement;

-- Get most recent match for specified user
select matches.matchID, datetime(max(matches.timestamp), 'unixepoch', 'localtime'), matches.playerCount, matches.duration 
from matches, match_records 
where matches.matchID=match_records.matchID and match_records.userID=?;

-- Get all time stats for user (total wins, total kills, total deaths)
select sum(case when placement=1 then 1 else 0 end), sum(kills), sum(deaths) from match_records where userID=?;

-- Get x amount of newewst match stats for specified user (Match time, playerCount, duration, placement, kills, deaths)
select datetime(matches.timestamp, 'unixepoch', 'localtime'), matches.playercount, 
       matches.duration, match_records.placement, match_records.kills, match_records.deaths 
from matches, match_records 
where matches.matchID=match_records.matchID and match_records.userID=? 
order by matches.timestamp desc 
limit ?;