class BasicSocket < IO
  def self.do_not_reverse_lookup=(setting)
    @no_reverse_lookup = setting
  end

  def self.do_not_reverse_lookup
    @no_reverse_lookup ? true : false
  end

  def initialize(domain, type, protocol)
    # TCPSocket hits here already setup (the class inheritance
    # for socket is busted, thats why we have this.)
    unless descriptor
      fd = Socket::Foreign.create_socket(domain.to_i, type.to_i, protocol.to_i)
      Errno.handle if fd < 0
    
      super(fd)
    end
    
    @domain = domain
    @type = type
    @protocol = protocol
  end

  def getsockopt(level, optname)

    MemoryPointer.new 256 do |val|
      MemoryPointer.new :int do |length|
        length.write_int 256
        error = Socket::Foreign.get_socket_option(descriptor, level, optname, val, length)

        if error != 0
          Errno.handle "Unable to get socket option"
          return nil
        end

        return val.read_string(length.read_int)
      end
    end
    
  end

  def setsockopt(level, optname, optval)
    if optval.is_a?(TrueClass)
      optval = 1
    elsif optval.is_a?(FalseClass)
      optval = 0
    end

    error = 0

    if optval.is_a?(Fixnum)
      MemoryPointer.new :int do |val|
        val.write_int optval
        error = Socket::Foreign.set_socket_option(descriptor, level, optname, val, val.size)
      end
    elsif optval.is_a?(String)
      MemoryPointer.new optval.size do |val|
        val.write_string optval
        error = Socket::Foreign.set_socket_option(descriptor, level, optname, val, optval.size)
      end
    else
      raise "socket option should be a String, a Fixnum, true, or false"
    end

    if error != 0
      Errno.handle "Unable to set socket option"
    end
    
    return 0
  end
end

class Socket < BasicSocket
    
  module Constants
    FFI.config_hash("socket").each do |key, value|
      const_set(key, value)
    end

    # Eventually needs to go into the Rakefile check:
    AI_PASSIVE = 1
  end
  
  module Foreign
    attach_function "socket", :create_socket, [:int, :int, :int], :int
    attach_function "connect", :connect_socket, [:int, :string, :int], :int
    attach_function "bind", :bind_socket, [:int, :string, :int], :int
    attach_function "listen", :listen_socket, [:int, :int], :int
    attach_function "accept", :accept, [:int, :string, :pointer], :int
    attach_function "setsockopt", :set_socket_option, [:int, :int, :int, :pointer, :int], :int
    attach_function "getsockopt", :get_socket_option, [:int, :int, :int, :pointer, :pointer], :int
    attach_function "ffi_pack_sockaddr_un", :pack_sa_unix, [:state, :string], :object
    attach_function "ffi_pack_sockaddr_in", :pack_sa_ip,   [:state, :string, :string, :int, :int], :object
    attach_function "ffi_getpeername", :getpeername, [:state, :int, :int], :object
    attach_function "ffi_getsockname", :getsockname, [:state, :int, :int], :object
    attach_function "ffi_bind", :bind_name, [:int, :string, :string, :int], :int
  end
  
  include Socket::Constants

  def self.pack_sockaddr_in(port, host, type = 0, flags = 0)
    Socket::Foreign.pack_sa_ip(host.to_s, port.to_s, type, flags)
  end

  class << self
    alias_method :sockaddr_in, :pack_sockaddr_in
  end
  
end

class UNIXSocket < BasicSocket
    
  def initialize(path)
    super(Socket::Constants::AF_UNIX, Socket::Constants::SOCK_STREAM, 0)
    @path = path
  end
end

class UNIXServer < UNIXSocket
  
end

class IPSocket < BasicSocket
  def initialize(kind, protocol=0)
    super(Socket::Constants::AF_INET, kind, protocol)
  end

  def peeraddr
    reverse = BasicSocket.do_not_reverse_lookup ? 0 : 1

    name, addr, port = Socket::Foreign.getpeername descriptor, reverse
    if addr.nil?
      raise "Unable to get peer address"
    end
    ["AF_INET", port.to_i, name, addr]
  end
end

class UDPSocket < IPSocket
  def initialize
    super(Socket::Constants::SOCK_DGRAM)
  end
  
  def bind(host, port)
    @port = port
    @host = host

    ret = Socket::Foreign.bind_name(descriptor, @host.to_s, @port.to_s, @type.to_i)

    Errno.handle if ret != 0

    return

    p :bind => [host, port]
    @sockaddr = Socket.pack_sockaddr_in(@port, @host, @type)

    p :sockaddr => [descriptor, @sockaddr]
    ret = Socket::Foreign.bind_socket(descriptor, @sockaddr, @sockaddr.size)
    p :ret => ret
    Errno.handle if ret != 0
  end
  
  def inspect
    "#<#{self.class}:0x#{object_id.to_s(16)} #{@host}:#{@port}>"
  end

end

class TCPSocket < IPSocket

  def self.from_descriptor(fixnum)
    sock = allocate()
    sock.from_descriptor(fixnum)
    return sock
  end

  def from_descriptor(fixnum)
    setup(fixnum)

    @connected = true
    
    name, addr, port = Socket::Foreign.getpeername fixnum, 0

    initialize(addr, port)

    return self
  end

  def initialize(host, port)
    super(Socket::Constants::SOCK_STREAM)

    @host = host
    @port = port

    @connected ||= false

    unless @connected
      @sockaddr = Socket.pack_sockaddr_in(@port, @host, @type)

      ret = Socket::Foreign.connect_socket(descriptor, @sockaddr, @sockaddr.size)
      Errno.handle if ret != 0
      @connected = true
    end
  end
  
  def inspect
    "#<#{self.class}:0x#{object_id.to_s(16)} #{@host}:#{@port}>"
  end
end

class TCPServer < TCPSocket
  def initialize(host, port = nil)
    if host.kind_of?(Fixnum) then
      port = host
      host = '0.0.0.0' # TODO - Do this in a portable way
    end
    @host = host
    @port = port

    @domain = Socket::Constants::AF_INET
    @type = Socket::Constants::SOCK_STREAM
    @protocol = 0
    fd = Socket::Foreign.create_socket(@domain, @type, @protocol)
    if fd < 0
      Errno.handle "Unable to create socket"
    end

    setup(fd)

    begin
      setsockopt(Socket::Constants::SOL_SOCKET, Socket::Constants::SO_REUSEADDR, true)
    rescue SystemCallError
      # MRI's socket.c tries this but never checks the return value, so
      # just eat any error.
    end

    @sockaddr = Socket.pack_sockaddr_in(@port, @host, @type,
                                        Socket::Constants::AI_PASSIVE)

    ret = Socket::Foreign.bind_socket(descriptor, @sockaddr, @sockaddr.size)
    Errno.handle if ret != 0

    ret = Socket::Foreign.listen_socket(fd, 5)
    if ret != 0
      Errno.handle "Unable to listen on #{@host}:#{@port}"
    end
  end

  def accept
    return if closed?
    wait_til_readable()

    fd = -1
    size = 0
    MemoryPointer.new :int do |sz|
      sz.write_int @sockaddr.size # initialize to the 'expected' size
      fd = Socket::Foreign.accept descriptor, @sockaddr, sz
      size = sz.read_int
    end

    if fd < 0
      Errno.handle "Unable to accept on socket"
    end

    TCPSocket.from_descriptor(fd)
  end
  
  def listen(log)
    log = Type.coerce_to(log, Fixnum, :to_int)

    ret = Socket::Foreign.listen_socket(descriptor, log)
    if ret != 0
      Errno.handle
    end
    ret
  end
end

