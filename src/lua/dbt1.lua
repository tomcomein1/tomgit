sqlite3 = require "luasql.sqlite3"
local env  = sqlite3.sqlite3()
local conn = env:connect('mydb.sqlite')
print(env,conn)
status,errorString = conn:execute([[CREATE TABLE sample ('id' INTEGER, 'name' TEXT)]])
print(status,errorString )