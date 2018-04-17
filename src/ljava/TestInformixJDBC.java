package com.tom.test;

import java.sql.*;

public class TestInformixJDBC {
	private static String URL_STRING = "jdbc:informix-sqli://160.161.12.181:8009/snswdb:INFORMIXSERVER=snhx";
	private static String USER = "snsw";
	private static String PASSWORD = "snsw";

	public static void main(String[] args) throws SQLException,
			ClassNotFoundException {
		Connection conn;
		Class.forName("com.informix.jdbc.IfxDriver");
//		Class.forName("com.ibm.db2.jcc.DB2Driver");
		for (int i = 0; i < 1; i++) {
			try {
				System.out.println("Testing JDBC URL: " + URL_STRING);
				conn = DriverManager.getConnection(URL_STRING, USER,
						PASSWORD);
				System.out.println("connect successfully");
				conn.close();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		return;
	}
}
