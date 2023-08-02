# OKR

## 背景

现有计划管理通过文件进行组织，并通过git仓库进行管理。

存在如下问题：

    1. 隐私问题。git仓库使用的是github，意味着数据存在泄露的风险；
    2. 结构化差。展示不够直观，格式不固定每月的执行情况难于统一；
    3. 统计分析困难。由于文件存储，且缺少固定格式，数据收集、统计、分析困难；
    4. 依赖终端。移动端支持程度差，难以查看和修改。

继Task管理之后，计划将月度计划管理也进行升级。

## 设计


OKR是一种目标管理方法，用于团队协作共同完成更大目标的手段。本文通过OKR的思想对每月的核心目标进行组织管理。

### 核心思想

> 天下大事必做于细，天下难事必作于易。

将年度目标进行拆解，以月为单元，将每一个大的目标差分成一个个小目标并列入每月的事项中进行执行与实现。

### 数据结构


#### 数据表示例

| id    |   event              | start       | end          | est   | act  | parent  |   score   | classify  |  level  | comments  |
| :-    | :-                   | :-          | :-           | :-:   | :-   | :-      | :-        | :-        | :-      | :-        |
| 1     |  tooth fix           | 2023-08-01  | 2023-08-20   |   12  |  15  |   0     |     90    |   3       |    1    |  NULL     |
| 2     |  treatment confirm   | 2023-08-05  | 2023-08-05   |   4   |   4  |   1     |     100   |   3       |    2    |  NULL     |


#### 字段说明

|  FIELD     |  TYPE           |      DESCRIPTION                                               |
|  :-        |  :-             |  :-                                                            |
|  id        |  INT            | primary key, not null, auto increment                          |
|  event     |  VARCHAR(2048)  | content of the objectives or key results                       |
|  start     |  DATE           | the day start the event                                        |
|  end       |  DATE           | the day end the event                                          |
|  est       |  FLOAT          | estimate hours for the event                                   |
|  act       |  FLOAT          | actual hours for the event                                     |
|  parent    |  INT            | index `id` of parent objectives; 0 means top level             |
|  score     |  TINYINT        | self assessment from 0 ot 100                                  |
|  classify  |  TINYINT        | 0: Work, 1: Skill, 2: Read, 3: Life                            |
|  level     |  TINYINT        | 0: year obj, 1: monthly obj, 2: kr                             |
|  comments  |  VARCHAR(10240) | the specifics of the event                                     |
|  ctime     |  DATETIME       | create time of the record                                      |
|  mtime     |  DATETIME       | modify time of the record                                      |


#### 建表语句

```sql
create table `okr` (
    `id` INT NOT NULL AUTO_INCREMENT,
    `event` VARCHAR(2048),
    `start` DATE NOT NULL,
    `end` DATE NOT NULL,
    `est` FLOAT,
    `act` FLOAT,
    `parent` INT,
    `score` TINYINT,
    `classify` TINYINT,
    `level` TINYINT,
    `comments` VARCHAR(10240),
    `ctime` DATETIME,
    `mtime` DATETIME,
    primary key(`id`)
)ENGINE=InnoDB DEFAULT charset=utf8;
```