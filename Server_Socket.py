import socket

####################################################
#  Server_Socket class                             #
#  based on code found in a tutorial on python.org #
#  Written by Chen-Yi Liu                          #
#  April 2017                                      #
####################################################
class Server_Socket:
  # once a class object is created, a socket will be open on port.
  # queue_size is the number of incoming connections that can be put on the wait list.
  def __init__(self, port, queue_size = 1):
    self.listening_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    self.listening_socket.bind(('localhost', port))
    self.listening_socket.listen(queue_size)
    print("Server is listening.")

  # tell the socket to start accepting new connections
  def accept(self):
    (self.io_socket, self.client_address) = self.listening_socket.accept()
    print("Server accepted a new client.")
    
  # send data in a bytearray called msg and of length msg_len
  def send(self, msg, msg_len):
    totalsent = 0
    while totalsent < msg_len:
      sent = self.io_socket.send(msg[totalsent:])
      if sent == 0:
        self.close()
        raise RuntimeError("socket connection broken!")
      totalsent = totalsent + sent
      
  # receive msg_len bytes of data packed in a bytearray
  def receive(self, msg_len):
    chunks = []
    bytes_received = 0
    while bytes_received < msg_len:
      chunk = self.io_socket.recv(min(msg_len - bytes_received, 2048))
      #print(chunk, len(chunk))
      if chunk == b'':
        self.close()
        raise RuntimeError("socket connection broken!")
      chunks.append(chunk)
      bytes_received += len(chunk)
    return b''.join(chunks)

  # remember to close the socket after use
  def close(self):
    self.io_socket.close()
    self.listening_socket.close()
    
########################################
