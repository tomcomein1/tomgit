#coding=utf-8
import urllib

def getHtml(url):
	page = urllib.urlopen(url)
	html = page.read()
	return html

if __name__ == '__main__':
	url=raw_input("input url:")
	print url, 'GO>>>'
	html=getHtml(url)
	print html
