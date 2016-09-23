local BUFSIZE = 8192
local f = io.input(arg[1])  --打开输入文件
local cc, lc, wc = 0, 0, 0  --分别计数字符、行和单词    
while true do
      local lines,rest = f:read(BUFSIZE,"*line")
      if not lines then
          break
      end
      if rest then
         lines = lines .. rest .. "\n"
     end
     cc = cc + #lines
     --计算单词数量
     local _,t = string.gsub(lines."%S+","")
     wc = wc + t
     --计算行数
     _,t = string.gsub(line,"\n","\n")
     lc = lc + t
end
print(lc,wc,cc)
