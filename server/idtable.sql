create database if not exists idserver; 
create table if not exists idserver.idtable(
id int auto_increment primary key,
biz_type int default 0,
biz_id bigint unsigned default 0,
id_step int unsigned default 1,
unique key(biz_type) 
)engine=innodb,default charset=utf8;