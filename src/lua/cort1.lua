function foo(a)
        print("foo ������", a)
        return coroutine.yield(2*a)
end

co = coroutine.create(function(a, b)
        print("��һ��Эͬ����ִ�����", a, b)
        local r=foo(a+1)

        print("�ڶ���Эͬ����ִ�����", r)
        local r, s=coroutine.yield(a+b, a-b) --a, b��ֵΪ��һ�ε���Эͬ����ʱ����
        print("������Эͬ����ִ�����", r, s)
        return b, "����Эͬ����"
end)

print("main", coroutine.resume(co, 1, 10)) -- true, 4
print("--�ָ���----")
print("main", coroutine.resume(co, "r")) -- true 11 -9
print("---�ָ���---")
print("main", coroutine.resume(co, "x", "y")) -- true 10 end
print("---�ָ���---")
print("main", coroutine.resume(co, "x", "y")) -- cannot resume dead coroutine
print("---�ָ���---")