@echo "��ʼ�Ҹ����ݿ�..."
E:
cd E:/MySQL/backup
set Today=%date:~0,4%%date:~5,2%%date:~8,2%
set /a PassDays=%Today%-1
mysql -upoptom -ppoptom321 poptomdb<"E:\MySQL\backup\pms_back_%PassDays%.dump"
echo "�ָ����ݿ�poptomdb���."
pause