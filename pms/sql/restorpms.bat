@echo "开始灰复数据库..."
E:
cd E:/MySQL/backup
set Today=%date:~0,4%%date:~5,2%%date:~8,2%
set /a PassDays=%Today%-1
mysql -upoptom -ppoptom321 poptomdb<"E:\MySQL\backup\pms_back_%PassDays%.dump"
echo "恢复数据库poptomdb完成."
pause