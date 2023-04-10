# jarvis

Jarvis is my good assistant. He helps me to manager my time elapse and financial records.

## Functions' Indroduction

1. TimeTable

Time clasify version I is very simple. There are total 5 Top Level classifications, as follow:

| Abbreviation | Full Name  | Description                                                  |
| :----------: | :--------- | :----------------------------------------------------------- |
|      I      | Investment | Do something valuable                                        |
|      S      | Sleep      | Sleep is important                                           |
|      R      | Relaxation | Meditation & Notes & so on                                   |
|      B      | Black Hole | Something meaningless, eg. games, video                      |
|      T      | Trivial    | Something less value but have to do, eg. eating, have a bath |

2. Financial

So many years, I am still so poor. Do as Rich Dad said is unavoidable.

First，I have to record my income and pay out details.

| Column   | Description                    |
| :------- | :----------------------------- |
| when     | Record concrete date and time  |
| payer    | Who pays                       |
| payee    | A person to whom money is paid |
| amount   | Too easy to explain            |
| clasify  | Clasify for the record         |
| comments | Record something special       |

Actually, all records is around of me.

There are 2 roles, payer and payee, it means amount is always positive.

## Designation

### 1. TimeTable

Creating a table as follow to record rate of time classification.

```sql
create table `time_table` (
    `id` INT NOT NULL AUTO_INCREMENT,
    `when` DATE,
    `i` DOUBLE,
    `s` DOUBLE,
    `r` DOUBLE,
    `b` DOUBLE,
    `t` DOUBLE,
    `create_time` DATETIME,
    `update_time` DATETIME,
    primary key(`id`)
)ENGINE=InnoDB DEFAULT charset=utf8;
```

### 2. Financial

1. A table to record all uses, including payer and payee.

```sql
create table `financial_users` (
    `uid` INT,
    `name` VARCHAR(100) NOT NULL,
    `create_time` DATETIME NOT NULL,
    `update_time` DATETIME NOT NULL,
    primary key(`uid`)
)ENGINE=InnoDB DEFAULT charset=utf8;
```

There are some dificult points.

- Sometimes uid is too trivial to clasify.

eg. carry out restaurant. It's meaningless to clasify each one of it.

So, I will aggregate all of it tobe restaurant.

2. Another table to record all payments.

```sql
create table `financial_records` (
    `id` INT NOT NULL AUTO_INCREMENT,
    `when` DATETIME,
    `payer` INT,
    `payee` INT,
    `amount` DOUBLE,
    `clasify` TINYINT,
    `comments` VARCHAR(1024),
    `create_time` DATETIME,
    `update_time` DATETIME,
    primary key(`id`)
)ENGINE=InnoDB DEFAULT charset=utf8;
```

- `Payer` & `payee` are `uid` in table `financial_users`.
- Classification is hard. Version I should be as simple as possible.

> Attention: you should use it append only, replace deletion with a cancelation record.

3. A table to count user's asset.


```sql
create table `financial_asset` (
    `id` INT NOT NULL AUTO_INCREMENT,
    `timestamp` DATETIME,
    `uid` INT,
    `amount` DOUBLE,
    `recordid` INT,
    `create_time` DATETIME,
    `update_time` DATETIME,
    primary key(`id`)
)ENGINE=InnoDB DEFAULT charset=utf8;
```

- `recordid` is the `id` in `financial_records`

## Fix Me

### Maintain manually

Creating image & starting contianer and so on are too complex to maintain.

There are too many details to remember it with brain.

We need config file or scripts to maintain all of it.

`docker-compose` maybe a good option.

### Bugs cause of windows + docker + wsl2 enviorment

Need a server machine run ubuntu os.

Not enmergency, I will have it in the future.

Eventually，we can access to the system no matter where and no matter when.

### Data is not persistence

Database & table exists in db container, data will lost after container deletion.

The thoroughly solution is deploy redundancy. It means more than one machine is needed.

For now the system deployment should be as simple as possible.

~~We can start a cron event to dump db data and sync to another directory.~~

Actually，mount mysql container /var/lib/mysql into host device will keep all data.

Data is still lost if the host filesystem is broken.

### MYSQL Chinese Coding Error

[fix mysql chinese coding error](https://blog.csdn.net/m0_38023584/article/details/105584623)
