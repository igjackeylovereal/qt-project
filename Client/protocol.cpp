#include "protocol.h"

#include <cstdlib>
#include <cstring>

Pdu* MakePdu(uint msg_len) {
  uint total_len = msg_len + sizeof(Pdu);
  Pdu* pdu = static_cast<Pdu*>(std::malloc(total_len));
  if (pdu == nullptr) {
    std::exit(1);
  }
  std::memset(pdu, 0, total_len);
  pdu->total_len = total_len;
  pdu->msg_len = msg_len;
  return pdu;
}
