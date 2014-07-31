import sys
from struct import *

try:
  for extdir in ['../build/swig']:
    if extdir not in sys.path:
      sys.path.append(extdir)
  import npgsm_swig as npgsm
except:
    import npgsm

from twisted.internet.protocol import DatagramProtocol
from twisted.internet import reactor
from twisted.application.internet import MulticastServer

class RawGsmListener(DatagramProtocol):
  def startProtocol(self):
    self.transport.joinGroup("239.13.37.1",interface="127.0.0.1")
    print 'Started Listening'

  def datagramReceived(self, datagram, address):
    print 'datagram recieved!'
    timeslot, frame_number, arfcn = unpack('<BIH', datagram[:7])
    data = datagram[7:]
    print timeslot,frame_number,arfcn,repr(data)

reactor.listenMulticast(29653, RawGsmListener(), listenMultiple=True)
reactor.run()

