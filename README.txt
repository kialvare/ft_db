
Insert:

insert into (file_name) values(key,fname,lname,age)

i.e. insert into roster.db values(1,Bob,Marley,42)

Delete:

delete from (file_name) (key)

i.e. delete from roster.db 1

Select:

select * from (filename)
select (col_name) (col_name) from (filename)
select * from (filename) WHERE (col_name)=(value)

i.e. select * from roster.db WHERE key=1

Updating:

update (filename) set (col_name)=[changed value] WHERE (col_name)=[current value]

i.e. update roster.db set fname=Bob WHERE fname=Elvis
