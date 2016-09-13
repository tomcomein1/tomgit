def sock_cli():
	import socket
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	sock.connect(('localhost', 9901))
	import time
#	time.sleep(2)
	sbuff=raw_input("input world:")
#	print sbuff
	sock.send(sbuff)
	print sock.recv(1024)
	sock.close()

if __name__ == '__main__':
	sock_cli()
