# jarvis

Jarvis is my good assistant. He helps me to manager my time elapse and financial records.

## Functions' Indroduction

1. TimeTable

Time clasify version I is very simple. There are total 5 Top Level classifications, as follow:

| Abbreviation  |   Full Name   |       Description                                                 |
| :------------:|:--------------|:------------------------------------------------------------------|
|  I            | Investment    | Do something valuable                                             |
|  S            | Sleep         | Sleep is important                                                |
|  R            | Relaxation    | Meditation & Notes & so on                                        |
|  B            | Black Hole    | Something meaningless, eg. games, video                           |
|  T            | Trivial       | Something less value but have to do, eg. eating, have a bath      |


2. Financial

So many years, I am still so poor. Do as Rich Dad said is unavoidable.

First，I have to record my income and pay out details.

| Column    |   Description                                                 |
| :-------  |:--------------                                                |
| when      | Record concrete date and time                                 |
| payer     | Who pays                                                      |
| payee     | A person to whom money is paid                                |
| amount    | Too easy to explain                                           |
| clasify   | Clasify for the record                                        |
| comments  | Record something special                                      |


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
    `id` INT NOT NULL AUTO_INCREMENT,
    `uid` TINYINT,
    `name` VARCHAR(100) NOT NULL,
    `create_time` DATETIME,
    `update_time` DATETIME,
    primary key(`id`)
)ENGINE=InnoDB DEFAULT charset=utf8;
```

There are some dificult points.

- Sometimes uid is too trivial to clasify.

eg. carry out restaurant. It's meaningless to clasify each one of it.

So, I will aggregate all of it tobe restaurant.



2. Another table to record all payments.

```sql
create table `financial_turnovers` (
    `id` INT NOT NULL AUTO_INCREMENT,
    `when` DATETIME,
    `payer` TINYINT,
    `payee` TINYINT,
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

