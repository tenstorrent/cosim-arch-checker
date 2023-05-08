// Licensed under the Apache License, Version 2.0, see LICENSE.TT for details

#include <iostream>
#include <fstream>
#include <cstring>
#include <cassert>
#include <cstdint>
#include <unistd.h>
#include <netdb.h>
#include "svdpi.h"
#include "WhisperMessage.h"


static int whisperSoc = -1;


// Connect to the whisper process running at the given host and
// listening to the given port. Return non-negative connected socket
// number on success and -1 on failure.
extern "C"
int
whisperConnectHostPort(const char* host, unsigned port)
{
  if (not host or not *host)
    {
      std::cerr << "Error: whisperConnectHostPort: Null host name\n";
      return -1;
    }

  // 1. Create socket.
  int soc = socket(AF_INET, SOCK_STREAM, 0);
  if (soc < 0)
    {
      char buffer[512];
      if (!strerror_r(errno, buffer, sizeof(buffer)))
        {
        char *p = buffer;
        std::cerr << "Failed to create socket: " << p << '\n';
        return -1;
        }
    }

  // 2. Obtain host IP address.
  struct hostent* server = gethostbyname("localhost");
  if (not server)
    {
      std::cerr << "Failed to find IP address of host " << host << '\n';
      return -1;
    }

  // 3. Setup socket address (host address + port)
  struct sockaddr_in serverAddr;
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(port);
  bcopy(server->h_addr, &serverAddr.sin_addr.s_addr, server->h_length);

  // 4. Connect socket to socket address.
  if (connect(soc, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) < 0)
    {
      std::cerr << "Socket connection failed\n";
      return -1;
    }

  return soc;
}


// Connect to the whisper process defined by the given file.  File
// should contain a single line consisting of a host name followed by
// a port number. Example:  good-host 1700
// Return non-negative socket number on success and -1 on failure.
extern "C"
int
whisperConnect(const char* filePath)
{
  if (whisperSoc >= 0)
    return whisperSoc;

  if (not filePath or not *filePath)
    {
      std::cerr << "Error: whisperConnect: Null file path\n";
      return -1;
    }

  std::ifstream ifs(filePath);
  if (not ifs)
    {
      std::cerr << "Failed to open host-info file " << filePath << "\n";
      return -1;
    }

  std::string hostName;
  unsigned port = -1;
  ifs >> hostName >> port;
  if (not ifs.good())
    {
      std::cerr << "Failed to read host-name and port-number from file "
		<< filePath << '\n';
      return -1;
    }

  whisperSoc = whisperConnectHostPort(hostName.c_str(), port);
  return whisperSoc;
}


extern "C"
void
whisperDisconnect()
{
  if (whisperSoc >= 0)
    close(whisperSoc);
  whisperSoc = -1;
}


/// Unpack socket message (received in server mode) into the given
/// WhisperMessage object.
static
void
deserializeMessage(const char buffer[], size_t bufferLen,
		   WhisperMessage& msg)

{
  assert (bufferLen >= sizeof(msg));

  const char* p = buffer;
  uint32_t x = ntohl(* reinterpret_cast<const uint32_t*> (p));
  msg.hart = x;
  p += sizeof(x);

  x = ntohl(* reinterpret_cast<const uint32_t*> (p));
  msg.type = x;
  p += sizeof(x);

  x = ntohl(* reinterpret_cast<const uint32_t*> (p));
  msg.resource = x;
  p += sizeof(x);

  x = ntohl(* reinterpret_cast<const uint32_t*> (p));
  msg.size = x;
  p += sizeof(x);

  x = ntohl(* reinterpret_cast<const uint32_t*> (p));
  msg.flags = x;
  p += sizeof(x);

  uint32_t part = ntohl(* reinterpret_cast<const uint32_t*> (p));
  msg.instrTag = uint64_t(part) << 32;
  p += sizeof(part);

  part = ntohl(* reinterpret_cast<const uint32_t*> (p));
  msg.instrTag |= part;
  p += sizeof(part);

  part = ntohl(* reinterpret_cast<const uint32_t*> (p));
  msg.time = uint64_t(part) << 32;
  p += sizeof(part);

  part = ntohl(* reinterpret_cast<const uint32_t*> (p));
  msg.time |= part;
  p += sizeof(part);

  part = ntohl(* reinterpret_cast<const uint32_t*> (p));
  msg.address = uint64_t(part) << 32;
  p += sizeof(part);

  part = ntohl(* reinterpret_cast<const uint32_t*> (p));
  msg.address |= part;
  p += sizeof(part);

  part = ntohl(* reinterpret_cast<const uint32_t*> (p));
  msg.value = uint64_t(part) << 32;
  p += sizeof(part);

  part = ntohl(* reinterpret_cast<const uint32_t*> (p));
  msg.value |= part;
  p += sizeof(part);

  memcpy(msg.buffer, p, sizeof(msg.buffer));
  p += sizeof(msg.buffer);

  memcpy(msg.tag, p, sizeof(msg.tag));
  p += sizeof(msg.tag);

  assert(size_t(p - buffer) <= bufferLen);
}


static
size_t
serializeMessage(const WhisperMessage& msg, char buffer[],
		 size_t bufferLen)
{
  assert (bufferLen >= sizeof(msg));

  char* p = buffer;
  uint32_t x = htonl(msg.hart);
  memcpy(p, &x, sizeof(x));
  p += sizeof(x);

  x = htonl(msg.type);
  memcpy(p, &x, sizeof(x));
  p += sizeof(x);

  x = htonl(msg.resource);
  memcpy(p, &x, sizeof(x));
  p += sizeof(x);

  x = htonl(msg.size);
  memcpy(p, &x, sizeof(x));
  p += sizeof(x);

  x = htonl(msg.flags);
  memcpy(p, &x, sizeof(x));
  p += sizeof(x);

  uint32_t part = static_cast<uint32_t>(msg.instrTag >> 32);
  x = htonl(part);
  memcpy(p, &x, sizeof(x));
  p += sizeof(x);

  part = (msg.instrTag) & 0xffffffff;
  x = htonl(part);
  memcpy(p, &x, sizeof(x));
  p += sizeof(x);

  part = static_cast<uint32_t>(msg.time >> 32);
  x = htonl(part);
  memcpy(p, &x, sizeof(x));
  p += sizeof(x);

  part = (msg.time) & 0xffffffff;
  x = htonl(part);
  memcpy(p, &x, sizeof(x));
  p += sizeof(x);

  part = static_cast<uint32_t>(msg.address >> 32);
  x = htonl(part);
  memcpy(p, &x, sizeof(x));
  p += sizeof(x);

  part = (msg.address) & 0xffffffff;
  x = htonl(part);
  memcpy(p, &x, sizeof(x));
  p += sizeof(x);

  part = static_cast<uint32_t>(msg.value >> 32);
  x = htonl(part);
  memcpy(p, &x, sizeof(x));
  p += sizeof(x);

  part = msg.value & 0xffffffff;
  x = htonl(part);
  memcpy(p, &x, sizeof(x));
  p += sizeof(x);

  memcpy(p, msg.buffer, sizeof(msg.buffer));
  p += sizeof(msg.buffer);

  memcpy(p, msg.tag, sizeof(msg.tag));
  p += sizeof(msg.tag);

  size_t len = p - buffer;
  assert(len <= bufferLen);
  assert(len <= sizeof(msg));
  for (size_t i = len; i < sizeof(msg); ++i)
    buffer[i] = 0;

  return sizeof(msg);
}


static bool
receiveMessage(int soc, WhisperMessage& msg)
{
  char buffer[sizeof(msg)];
  char* p = buffer;

  size_t remain = sizeof(msg);

  while (remain > 0)
    {
      ssize_t l = recv(soc, p, remain, 0);
      if (l < 0)
	{
	  if (errno == EINTR)
	    continue;
	  std::cerr << "Failed to receive socket message\n";
	  return false;
	}
      if (l == 0)
	{
	  msg.type = Quit;
	  return true;
	}
      remain -= l;
      p += l;
    }

  deserializeMessage(buffer, sizeof(buffer), msg);

  return true;
}


static bool
sendMessage(int soc, const WhisperMessage& msg)
{
  char buffer[sizeof(msg)];

  serializeMessage(msg, buffer, sizeof(buffer));

  // Send command.
  ssize_t remain = sizeof(msg);
  char* p = buffer;
  while (remain > 0)
    {
      ssize_t l = send(soc, p, remain , 0);
      if (l < 0)
	{
	  if (errno == EINTR)
	    continue;
	  std::cerr << "Failed to send socket command\n";
	  return false;
	}
      remain -= l;
      p += l;
    }

  return true;
}


static
bool
whisperCommand(const WhisperMessage& req, WhisperMessage& reply)
{
  assert(whisperSoc >= 0);

  if (not sendMessage(whisperSoc, req))
    {
      std::cerr << "Failed to send request to whisper\n";
      return false;
    }
  if (not receiveMessage(whisperSoc, reply))
    {
      std::cerr << "Failed to receive reply from whisper\n";
      return false;
    }
  return true;
}


extern "C"
bool
whisperPeek(int hart, char resource, uint64_t addr, uint64_t& value,
	    bool& valid)
{
  WhisperMessage req(hart, WhisperMessageType::Peek, resource, addr);
  WhisperMessage reply;

  if (not whisperCommand(req, reply))
    return false;

  valid = reply.type != WhisperMessageType::Invalid;
  value = reply.value;
  return true;
}


// Send a whisper poke command. Retrun true on successful comunication
// and false on failure. Set valid to false if hart/resource/addr
// are invalid.
extern "C"
bool
whisperPoke(int hart, char resource, uint64_t addr, uint64_t value,
	    bool& valid)
{
  WhisperMessage req(hart, WhisperMessageType::Poke, resource, addr, value);
  WhisperMessage reply;

  if (not whisperCommand(req, reply))
    return false;

  valid = reply.type != WhisperMessageType::Invalid;
  return true;
}


extern "C"
bool
whisperStep(int hart, uint64_t time, uint64_t instrTag, uint64_t& pc,
	    uint32_t& instruction, unsigned& changeCount,
	    char* buffer, unsigned bufferSize, uint32_t& privMode,
	    uint32_t& fpFlags, bool& hasTrap)
{
  WhisperMessage req(hart, WhisperMessageType::Step);
  req.instrTag = instrTag;
  req.time = time;

  WhisperMessage reply;
  if (not whisperCommand(req, reply))
    return false;

  pc = reply.address;
  instruction = reply.resource;
  changeCount = reply.value;

  // Recover privilege mode (2 bits), fpFlags (4 bits), and trap (1
  // bit) from flags field.
  unsigned mode = reply.flags & 3;
  unsigned flags = (reply.flags >> 2) & 0xf;
  unsigned trap = (reply.flags >> 6) & 1;

  privMode = mode;
  fpFlags = flags;
  hasTrap = trap;
  
  unsigned len = sizeof(reply.buffer);
  if (len > bufferSize)
    len = bufferSize;
  if (buffer)
    strncpy(buffer, reply.buffer, len);
  return true;
}

// Copied from chuang's LSTB Whisper Client
extern "C"
bool
whisperSimpleStep(int hart, uint64_t& pc, uint32_t& instruction, unsigned& changeCount)
{
  WhisperMessage req(hart, WhisperMessageType::Step);
  req.instrTag = 0;
  req.time = 0;

  WhisperMessage reply;
  if (not whisperCommand(req, reply))
    return false;

  pc = reply.address;
  instruction = reply.resource;
  changeCount = reply.value;

  return true;
}

extern "C"
bool
whisperChange(int hart, uint32_t& resource, uint64_t& addr, uint64_t& value,
	      bool& valid)
{
  WhisperMessage req(hart, WhisperMessageType::Change);
  WhisperMessage reply;
  if (not whisperCommand(req, reply))
    return false;

  resource = reply.resource;
  addr = reply.address;
  value = reply.value;
  valid = reply.type != WhisperMessageType::Invalid;
  return true;
}


extern "C"
bool
whisperMcmRead(int hart, uint64_t time, uint64_t instrTag, uint64_t addr,
	       unsigned size, uint64_t value, bool internal, bool& valid)
{
  WhisperMessage req(hart, WhisperMessageType::McmRead);
  req.time = time;
  req.instrTag = instrTag;
  req.address = addr;
  req.value = value;
  req.size = size;
  req.flags = not internal;

  WhisperMessage reply;
  if (not whisperCommand(req, reply))
    return false;

  valid = reply.type != WhisperMessageType::Invalid;
  return true;
}


extern "C"
bool
whisperMcmInsert(int hart, uint64_t time, uint64_t instrTag, uint64_t addr,
		 unsigned size, uint64_t value, bool& valid)
{
  WhisperMessage req(hart, WhisperMessageType::McmInsert);
  req.time = time;
  req.instrTag = instrTag;
  req.address = addr;
  req.value = value;
  req.size = size;

  WhisperMessage reply;
  if (not whisperCommand(req, reply))
    return false;

  valid = reply.type != WhisperMessageType::Invalid;
  return true;
}


extern "C"
bool
whisperMcmWrite(int hart, uint64_t time, uint64_t addr,
		unsigned size, svOpenArrayHandle handle , bool& valid)
{
  WhisperMessage req(hart, WhisperMessageType::McmWrite);
  req.time = time;
  req.address = addr;
  req.size = size;
  if (req.size > sizeof(req.buffer))
    {
      std::cerr << "whisperMcmWrite: write size too large: " << req.size << '\n';
      valid = false;
      return true;
    }

  uint8_t* data = reinterpret_cast<uint8_t*> (handle);
  for (unsigned i = 0; i < size; ++i)
    req.buffer[i] = data[i];

  WhisperMessage reply;
  if (not whisperCommand(req, reply))
    return false;

  valid = reply.type != WhisperMessageType::Invalid;
  return true;
}

extern "C"
bool
whisperReset(int hart, bool& valid)
{
  WhisperMessage req(hart, WhisperMessageType::Reset);
  WhisperMessage reply;

  if (not whisperCommand(req, reply))
    return false;

  valid = reply.type != WhisperMessageType::Invalid;
  return true;
}

extern "C"
bool
whisperQuit()
{
  assert(whisperSoc >= 0);

  WhisperMessage req(0 /*hart*/, WhisperMessageType::Quit);  // Any hart will do
  if (not sendMessage(whisperSoc, req))
    {
      std::cerr << "Failed to send request to whisper\n";
      return false;
    }

  whisperDisconnect();

  // There is no reply for quit command.
  return true;
}


#if 0

int
main(int argc, char* argv[])
{
  if (argc == 2)
    {
      std::string path = argv[1];
      int soc = whisperConnect(path);
      if (soc >= 0)
	{
	  std::cerr << "Connected to socket: " << soc << '\n';
	  uint64_t value = 0;
	  unsigned hart = 0;
	  bool valid = false;
	  whisperPeek(soc, hart, 'r', 1, value, valid);
	  whisperPeek(soc, hart, 'r', 2, value, valid);
	  whisperPoke(soc, hart, 'p', 0, 0x1234, valid);
	  whisperPeek(soc, hart, 'p', 0, value, valid);

	  unsigned count = 0;
	  whisperStep(soc, hart, count);
	  std::cerr << "Change count: " << std::hex << count << '\n';
	  for (unsigned i = 0; i < count; ++i)
	    {
	      unsigned resource = 0;
	      uint64_t addr = 0;
	      if (whisperChange(soc, hart, resource, addr, value, valid) and valid)
		std::cerr << "  " << char(resource) << "  " << std::hex << addr
			  << ' ' << std::hex << value << '\n';
	    }
	  whisperQuit(soc);
	}
    }
  return 0;
}
    
#endif
