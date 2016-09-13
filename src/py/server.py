#version 1.0 test. TOM create 20160912.
import sys
import socket

def usage(para):
	print para, '<port>'
	return

def sock_svr(port):

	host = '10.129.33.45'
	print 'Start:', host, port

	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	sock.bind((host, port))
	sock.listen(5)
	while True:
		connection, address = sock.accept()
		try:
			connection.settimeout(10)
			buf = connection.recv(1024)
			print address, buf
			sys.stdout.flush() 
			if buf == 'hello':
				connection.send('welcome to server!')
			elif buf == 'bye':
				connection.send('bye')
				break
			else:
				connection.send(buf)
		except socket.timeout:
			print 'time out'
		connection.close()

if __name__ == '__main__':
	argc = len(sys.argv)
	if argc != 2:
		usage(sys.argv[0])
		port = 9901	
#		sys.exit()
	else:
		port = int(sys.argv[1])

	sock_svr(port)


