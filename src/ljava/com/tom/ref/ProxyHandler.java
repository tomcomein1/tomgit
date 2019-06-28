package com.tom.ref;

import java.lang.reflect.InvocationHandler;
import java.lang.reflect.Method;
import java.lang.reflect.Proxy;

public class ProxyHandler {
    public static Object getDynamicProxy(Object object, InvocationHandler handler) {
        Object proxy = Proxy.newProxyInstance(object.getClass().getClassLoader(),
                object.getClass().getInterfaces(),
                handler);
        return proxy;
    }
}
