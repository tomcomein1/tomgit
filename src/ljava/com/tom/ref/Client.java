package com.tom.ref;

public class Client {
    public static void main(String[] args) {
//        UserManager um = (UserManager)(Class.forName("com.tom.ref.UserManagerImpl").newInstance());
        UserManager um = new UserManagerImpl();
        UserManager userManager = (UserManager) ProxyHandler.getDynamicProxy(um, new LogInvocationHandler1(um));
        //UserManager userManager=new UserManagerImpl();
        userManager.addUser("1111", "张三");
        userManager.findUser("1111");
        userManager.delUser("1111");
    }
}
