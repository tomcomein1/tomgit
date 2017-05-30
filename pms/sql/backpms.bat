@echo "开始备份数据库..."
E:
cd E:\
if not exist "MySQL" (md "MySQL")
cd MySQL
if not exist "backup" (md "backup") 
cd backup
set Today=%date:~0,4%%date:~5,2%%date:~8,2%
mysqldump -upoptom -ppoptom321 poptomdb>"E:\MySQL\backup\pms_back_%Today%.dump"
echo "备份数据库poptomdb完成."
exit
