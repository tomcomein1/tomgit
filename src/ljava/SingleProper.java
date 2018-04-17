package com.tom.test;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.util.Properties;

public class SingleProper {
	private static final SingleProper instance = new SingleProper();

	private SingleProper() {
	}

	public static final SingleProper getInstance() {
		return SingleProper.instance;
	}

	public String ReadIni(String fileName, String name) {
		String value = null;
		try {
			FileInputStream reader = new FileInputStream(fileName);
			Properties pro = new Properties();
			pro.load(reader);
			value = pro.getProperty(name);
		} catch (FileNotFoundException e) {
			System.out.print("¶ÁÈ¡ÎÄ¼þÊ§°Ü");
			e.printStackTrace();
		} catch (Exception e) {
			e.printStackTrace();
		}

		return value;
	}

	public static void main(String[] args) {
		SingleProper sp = SingleProper.getInstance();
		String item1 = sp.ReadIni("E:\\test.ini", "TOM");
		System.out.println(item1);
		String item2 = sp.ReadIni("E:\\test.ini", "MaxNumber");
		System.out.println(item2);
	}
}
