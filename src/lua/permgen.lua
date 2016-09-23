function permgen(a, n)
	if n==0 then
--		printResult(a)
		coroutine.yield(a)
	else
		for i=1,n do
			--put i-th element as the last one
			a[n], a[i]=a[i], a[n]
			permgen(a, n-1)
			--restore i-th element
			a[n], a[i]=a[i], a[n]
		end
	end
end

function perm(a)
	local n=table.getn(a)
	local co=coroutine.create(function() permgen(a,n) end)
	return function()
		local code, res=coroutine.resume(co)
		return res
	end
end

function perm1(a)
	local n=table.getn(a)
	return coroutine.wrap(function() permgen(a, n) end)
end

function printResult(a)
	for i,v in ipairs(a) do
		io.write(v, " ")
	end
	io.write("\n")
end

--permgen({1,2,3,4}, 4)

--for p in perm{"a", "b", "c"} do
for p in perm1{"a", "b", "c"} do
	printResult(p)
end

