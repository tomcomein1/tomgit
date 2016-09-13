mylist=[]
for i in range(1, 5):
	name = raw_input("input your name:")
	mylist.append(name)
myset = set(mylist)
for i in myset:
	print (i, mylist.count(i) )
