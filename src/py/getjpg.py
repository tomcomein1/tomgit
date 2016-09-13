#coding=utf-8
import re
import urllib

def getHtml(url):
	page = urllib.urlopen(url)
	html = page.read()
	return html

def getImg(html, page):
	reg = r'src="(.+?\.jpg)" pic_ext'
	imgre = re.compile(reg)
	imglist = re.findall(imgre, html)
	x = 0
	for imgurl in imglist:
		urllib.urlretrieve(imgurl, 'jpg\\%s_%s.jpg' % (page, x))
		x += 1
	print 'download:', x
#	return imglist

if __name__ == '__main__':
#	url=raw_input("input url:")
#	print url, 'GO>>>'

	for i in range(1, 17):
		url='http://tieba.baidu.com/p/2460150866?pn=' + str(i)
		print url
		html=getHtml(url)
		print getImg(html, str(i) )

