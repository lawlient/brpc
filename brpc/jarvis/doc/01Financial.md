# Financial

## Background

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

Actually, all records is related to me.

There are 2 roles, payer and payee, it means amount is always positive.

## Designation

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


