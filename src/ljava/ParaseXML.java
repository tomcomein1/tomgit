package com.tom.xml;

import java.io.IOException;
import java.io.StringWriter;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.logging.Level;
import java.util.logging.Logger;

import org.dom4j.Attribute;
import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;
import org.dom4j.io.OutputFormat;
import org.dom4j.io.XMLWriter;

public class ParaseXML {
	private String xmlstr = null;

	ParaseXML(){
	}
	
	ParaseXML(String xmlstr){
		this.xmlstr=xmlstr;
	}
	
	public void setXmlstr(String xmlstr) {
		this.xmlstr = xmlstr;
	}

	public String getXmlstr() {
		return this.xmlstr;
	}
	
	public void readXML(String str) {
		if (str != null && !"".equals(str)) {
			this.xmlstr = str;
			readXML();
		}
	}

	public void readXML() {
		try {
			// 将xml格式字符串转化为DOM对象
			Document document = DocumentHelper.parseText(xmlstr);
			// 获取根结点对象
			Element rootElement = document.getRootElement();
			// 循环根节点，获取其子节点
			for (Iterator iter = rootElement.elementIterator(); iter.hasNext();) {
				Element element = (Element) iter.next(); // 获取标签对象
				// 获取该标签对象的属性
				Attribute attr = element.attribute("id");
				if (null != attr) {
					String attrVal = attr.getValue();
					String attrName = attr.getName();
					System.out.println(attrName + ": " + attrVal);
				}
				// 循环第一层节点，获取其子节点
				for (Iterator iterInner = element.elementIterator(); iterInner
						.hasNext();) {
					// 获取标签对象
					Element elementOption = (Element) iterInner.next();
					// 获取该标签对象的名称
					String tagName = elementOption.getName();
					// 获取该标签对象的内容
					String tagContent = elementOption.getTextTrim();
					// 输出内容
					System.out.print(tagName + ": " + tagContent + "  ");
				}
				System.out.println();
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public String listMap2xml(List<Map<String,Object>> list,String listRoot,String mapRoot){
        Document doc = DocumentHelper.createDocument();
        
        Element rootEle = doc.addElement("result");
        Element noEle = rootEle.addElement("no");
        Element msgEle = rootEle.addElement("msg");
        
        if(null!=list && !list.isEmpty()){
            noEle.setText("1");
            msgEle.setText("成功获取相关信息");
            
            Element listRootEle = rootEle.addElement(listRoot);
            
            for(Map<String,Object> map:list){
                
                Element mapRootELe = listRootEle.addElement(mapRoot);
                
                Set<Map.Entry<String,Object>> set = map.entrySet();
                Iterator<Map.Entry<String,Object>> iter = set.iterator();
                while(iter.hasNext()){
                    Map.Entry<String,Object> entry = (Map.Entry<String,Object>)iter.next();
                    
                    Element ele = mapRootELe.addElement(entry.getKey());
                    ele.setText(String.valueOf(entry.getValue()));
                }
            }
        }else{
            noEle.setText("0");
            msgEle.setText("没有获取到相关信息");
        }
        
        StringWriter sw = new StringWriter();
        OutputFormat format = OutputFormat.createPrettyPrint();
        format.setEncoding("utf-8");
        
        try {
            
            XMLWriter xmlWriter = new XMLWriter(sw, format);
            
            xmlWriter.write(doc);
        } catch (IOException ex) {
            Logger.getLogger(ParaseXML.class.getName()).log(Level.SEVERE, null, ex);
        }finally{
            try {
                sw.close();
            } catch (IOException ex) {
                Logger.getLogger(ParaseXML.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
        
        return (xmlstr=sw.toString());
    }

	public static void main(String[] args) {
		String xmlResult = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><users><user id=\"10001\"><name>张三</name><role>群主</role><sex>男</sex><content>今天天气真不错！</content><time>2016-04-25 16:43:28</time></user><user id=\"10002\"> <name>李四</name><role>管理员</role><sex>男</sex><content>http://192.168.0.190:9999/beike/data/b3217f668.png</content><time>2016-04-25 16:45:08</time></user></users>";
		ParaseXML pxml=new ParaseXML(xmlResult);
		pxml.readXML();
		
		Map<String, Object> map = new HashMap<String, Object>();
        map.put("name", "李四");
        map.put("age", 25);
        
        //System.out.println(map2xml(map));
        
        
        List<Object> list = new ArrayList<Object>();
        list.add("测试1");
        list.add("测试2");
        list.add("测试3");
        
        //System.out.println(list2xml(list,"items"));
        
        List<Map<String,Object>> listMap = new ArrayList<Map<String,Object>>();
        Map<String,Object> map1 = new HashMap<String,Object>();
        map1.put("name", "张三");
        map1.put("age", 23);
        listMap.add(map1);
        map1 = new HashMap<String,Object>();
        map1.put("name", "李四");
        map1.put("age", 24);
        listMap.add(map1);
        map1 = new HashMap<String,Object>();
        map1.put("name", "王五");
        map1.put("age", 25);
        listMap.add(map1);
        
        System.out.println(pxml.listMap2xml(listMap,"users","user"));
	}
}
