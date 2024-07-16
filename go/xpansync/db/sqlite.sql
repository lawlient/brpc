create table if not exists setting (
    id integer not null primary key autoincrement,
    key TEXT not null unique,
    value TEXT,
    description TEXT
);
insert OR IGNORE into setting (key, value, description) values ("AppID", "", "BaiduNetDisk AppID");
insert OR IGNORE into setting (key, value, description) values ("AppKey", "", "BaiduNetDisk AppKey");
insert OR IGNORE into setting (key, value, description) values ("SecretKey", "", "BaiduNetDisk SecretKey");
insert OR IGNORE into setting (key, value, description) values ("AccessToken", "", "BaiduNetDisk AccessToken");
insert OR IGNORE into setting (key, value, description) values ("ExpiredAt", "", "BaiduNetDisk AccessToken Expired Timestamp");
insert OR IGNORE into setting (key, value, description) values ("Root", "", "BaiduNetDisk Root directory");
insert OR IGNORE into setting (key, value, description) values ("ChanifyAddr", "", "Chanify Notification Address");
insert OR IGNORE into setting (key, value, description) values ("Cron", "", "Upload task's cron");



create table if not exists task (
    id integer not null primary key autoincrement,
    ff TEXT not null unique,
    how TEXT not null check (`how` in ("origin", "package")) default "origin"
);


-- users
--   root: manager system & authorization 
--   admin: manager system
--   user: read only
create table if not exists users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    username TEXT NOT NULL UNIQUE,
    password TEXT NOT NULL,
    avatar TEXT NOT NULL DEFAULT '',
    role TEXT NOT NULL CHECK (role IN ('root', 'admin', 'user')) DEFAULT 'user'
);