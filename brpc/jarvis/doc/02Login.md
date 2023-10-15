# Login


## Background

Financial is a private function, so I add a login status checking.

Login is a simple right control, only those login successfully can access to financial system.


## Designation

1. Table `jusers` stores all users' name and password.

```sql
create table `jusers` (
    `id` INT NOT NULL AUTO_INCREMENT,
    `name` VARCHAR(100) UNIQUE,
    `right` INT,
    `secret` VARCHAR(100),
    `create_time` DATETIME,
    `update_time` DATETIME,
    primary key(`id`)
)ENGINE=InnoDB DEFAULT charset=utf8;
```

- name: user name
- right: 0:viewer, 1: admin
- secret: store password. For simple store it with plain text.



2. KV in redis.

There is a string key named '**Token**' stored in redis and the value is generated from a timestamp string with sha256.

Actually, login status checking is done by whether the value's length is 64bytes or not.


------

# Reflection

Actually, login is an authentication problem, the common solutions are those:

- JWT
- Token
- Session
- OAuth

My solution is not professional.
