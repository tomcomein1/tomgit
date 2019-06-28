package com.tom.ref;

import java.lang.reflect.InvocationHandler;
import java.lang.reflect.Method;

public class LogInvocationHandler implements InvocationHandler {
    private Object targetObject;

    public LogInvocationHandler(Object targetObject) {
        super();
        this.targetObject = targetObject;
    }

    @Override
    //关联的这个实现类的方法被调用时将被执行
    /*InvocationHandler接口的方法，proxy表示代理，method表示原对象被调用的方法，args表示方法的参数*/
    public Object invoke(Object proxy, Method method, Object[] args)
            throws Throwable {
        Object ret = null;
        try {
            /*原对象方法调用前处理日志信息*/
            System.out.println("Method start...");
            System.out.println("Input args:");
            for (int i = 0; i < args.length; i++) {
                System.out.println(args[i]);
            }
            //调用目标方法
            ret = method.invoke(targetObject, args);
            /*原对象方法调用后处理日志信息*/
            System.out.println("Method excute end...");
        } catch (Exception e) {
            e.printStackTrace();
            System.out.println("Error-->>");
            throw e;
        }

        return ret;
    }
}
