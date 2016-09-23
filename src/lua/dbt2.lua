sqlite3 = require "luasql.sqlite3"
local env  = sqlite3.sqlite3()
local conn = env:connect('mydb.sqlite')
print(env,conn)
status,errorString = conn:execute([[CREATE TABLE sample ('id' INTEGER, 'name' TEXT)]])
print(status,errorString )
status,errorString = conn:execute([[INSERT INTO sample values('1','Raj')]])
print(status,errorString )
cursor,errorString = conn:execute([[select * from sample]])
print(cursor,errorString)
row = cursor:fetch ({}, "a")
while row do
  print(string.format("Id: %s, Name: %s", row.id, row.name))
  row = cursor:fetch (row, "a")
end
-- close everything
cursor:close()
conn:close()
env:close()