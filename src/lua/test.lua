for count=1, math.huge do
	local line=io.read("*line") 
	if line == nil then
		break
	end
	io.write(string.format("%6d ", count), line, "\n")
end
