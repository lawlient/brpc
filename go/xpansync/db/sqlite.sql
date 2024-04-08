create table if not exists setting (
    id integer not null primary key autoincrement,
    key TEXT not null unique,
    value TEXT,
    description TEXT
);
insert OR IGNORE into setting (key, value, description) values ("AppID", "", "BaiduNetDisk AppID");
insert OR IGNORE into setting (key, value, description) values ("AppKey", "", "BaiduNetDisk AppKey");
insert OR IGNORE into setting (key, value, description) values ("SecretKey", "", "BaiduNetDisk SecretKey");
insert OR IGNORE into setting (key, value, description) values ("SignKey", "", "BaiduNetDisk SignKey");
insert OR IGNORE into setting (key, value, description) values ("AccessToken", "", "BaiduNetDisk AccessToken");
insert OR IGNORE into setting (key, value, description) values ("Root", "", "BaiduNetDisk Root directory");
insert OR IGNORE into setting (key, value, description) values ("ChanifyAddr", "", "Chanify Notification Address");
insert OR IGNORE into setting (key, value, description) values ("Cron", "", "Upload task's cron");



create table if not exists task (
    id integer not null primary key autoincrement,
    ff TEXT not null unique,
    how TEXT not null check (`how` in ("origin", "package")) default "origin"
)


