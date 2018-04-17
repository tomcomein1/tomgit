package com.tom.test;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.util.Properties;

public class SingleProper {
	private static final SingleProper instance = new SingleProper();
	private String filename = null;

	private SingleProper() {
	}

	public static final SingleProper getInstance() {
		return SingleProper.instance;
	}

	public void setFilename(String filename) {
		this.filename = filename;
	}

	public String ReadIni(String name) {
		String value = null;
		try {
			FileInputStream reader = new FileInputStream(filename);
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

	public String ReadIni(String fileName, String name) {
		this.filename = fileName;
		return ReadIni(name);
	}

	public static void main(String[] args) {
		SingleProper sp = SingleProper.getInstance();
		sp.setFilename("E:\\test.ini");
		String item1 = sp.ReadIni("TOM");
		System.out.println(item1);
		String item2 = sp.ReadIni("MaxNumber");
		System.out.println(item2);
	}
}

