package com.tom.ref;

import java.awt.*;
import java.lang.reflect.Constructor;

public class Factory {
    public static Component getComponent(String componentName) {
        try {
            Class<?> componentClass = Class.forName("com.tcpan.es.component." + componentName);
            Constructor<?> constructor = componentClass.getDeclaredConstructor(String.class);

            return (Component) constructor.newInstance("hi");
        } catch (Exception e) {
            return null;
        }
    }
}
