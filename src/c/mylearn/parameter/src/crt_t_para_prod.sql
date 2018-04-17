drop table t_para_prod;
create table t_para_prod (
  prod_no char(4) not null,
  prod_name varchar(100) not null,
  primary key (prod_no)
) in pbdatadbs05 
EXTENT SIZE 64 NEXT SIZE 64 LOCK MODE ROW;

create unique index idx_para_prod on t_para_prod( prod_name ) in pbidxdbs03 ;


