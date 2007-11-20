class BasicSocket < IO
  def self.do_not_reverse_lookup=(setting)
    @no_reverse_lookup = setting
  end

  def self.do_not_reverse_lookup
    @no_reverse_lookup ? true : false
  end

  def initialize(domain, type, protocol)
    fd = Socket::Foreign.create_socket(domain.to_i, type.to_i, protocol.to_i)
    Errno.handle if fd < 0

    super(fd)
    
    @domain = domain
    @type = type
    @protocol = protocol
  end

  def setsockopt(level, optname, optval)
    if optval.is_a?(TrueClass)
      optval = 1
    elsif optval.is_a?(FalseClass)
      optval = 0
    end

    if optval.is_a?(Fixnum)
      MemoryPointer.new :int do |val|
        val.write_int optval
        error = Socket::Foreign.set_socket_option(@descriptor, level, optname, val, optval.size)
      end
    elsif optval.is_a?(String)
      raise NotImplementedError
    else
      raise "socket option should be a String, a Fixnum, true, or false"
    end

    if error != 0
      Errno.handle "Unable to set socket option"
    end
    nil
  end
end

class Socket < BasicSocket
    
  module Constants
    AF_UNIX =   1
    AF_LOCAL =  1
    AF_INET =   2
    
    AI_PASSIVE = 1

    SOCK_STREAM = 1
    SOCK_DGRAM =  2
    SOCK_RAW =    3
    SOCK_RDM =    4
    SOCK_SEQPACKET = 5

    SO_ACCEPTFILTER = 4096

    SOL_TCP = 6

    case RUBY_PLATFORM
    when /linux/
      SOL_SOCKET = 1
      SO_REUSEADDR = 2
    else
      SOL_SOCKET = 65535 # TODO - Different on weird platforms
      SO_REUSEADDR = 4
    end
  end
  
  module Foreign
    attach_function nil, "socket", :create_socket, [:int, :int, :int], :int
    attach_function nil, "connect", :connect_socket, [:int, :pointer, :int], :int
    attach_function nil, "bind", :bind_socket, [:int, :pointer, :int], :int
    attach_function nil, "listen", :listen_socket, [:int, :int], :int
    attach_function nil, "accept", :accept, [:int, :pointer, :pointer], :int
    attach_function nil, "setsockopt", :set_socket_option, [:int, :int, :int, :pointer, :int], :int
    attach_function nil, "ffi_pack_sockaddr_un", :pack_sa_unix, [:state, :string], :object
    attach_function nil, "ffi_pack_sockaddr_in", :pack_sa_ip,   [:state, :string, :string, :int, :int], :object
    attach_function nil, "ffi_getpeername", :getpeername, [:state, :int, :int], :object
  end
  
  include Socket::Constants
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

    name, addr = Socket::Foreign.getpeername descriptor, reverse
    if addr.nil?
      raise "Unable to get peer address"
    end
    ["AF_INET", @port, name, addr]
  end
end

class UDPSocket < IPSocket
  ivar_as_index :descriptor => 1
  def descriptor=(other)
    @descriptor = other
  end
  
  def initialize(host, port, connected = false)
    unless connected
      super(Socket::Constants::SOCK_DGRAM)
      @host = host
      @port = port


      @sockaddr, @sockaddr_size = Socket::Foreign.pack_sa_ip(host.to_s, port.to_s, @type, 0)
      sock = Socket::Foreign.connect_socket(descriptor, @sockaddr, @sockaddr_size) 
      if sock != 0
        Errno.handle "Unable to connect to #{host}:#{port}"
      end
    end
  end
  
  def inspect
    "#<#{self.class}:0x#{object_id.to_s(16)} #{@host}:#{@port}>"
  end
end

class TCPSocket < IPSocket
  ivar_as_index :descriptor => 1
  def descriptor=(other)
    @descriptor = other
  end
  
  def initialize(host, port, connected = false)
    unless connected
      super(Socket::Constants::SOCK_STREAM)
      @host = host
      @port = port


      @sockaddr, @sockaddr_size = Socket::Foreign.pack_sa_ip(host.to_s, port.to_s, @type, 0)
      sock = Socket::Foreign.connect_socket(descriptor, @sockaddr, @sockaddr_size) 
      if sock != 0
        Errno.handle "Unable to connect to #{host}:#{port}"
      end
    end
  end
  
  def inspect
    "#<#{self.class}:0x#{object_id.to_s(16)} #{@host}:#{@port}>"
  end
end

class TCPServer < TCPSocket
  ivar_as_index :descriptor => 1
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

    @descriptor = fd
    setsockopt(Socket::Constants::SOL_SOCKET, Socket::Constants::SO_REUSEADDR, true)

    @sockaddr, @sockaddr_size = Socket::Foreign.pack_sa_ip(@host.to_s, @port.to_s, @type, Socket::Constants::AI_PASSIVE)
    bind = Socket::Foreign.bind_socket(descriptor, @sockaddr, @sockaddr_size)
    if bind != 0
      Errno.handle "Unable to bind to #{@host}:#{@port}"
    end
    ret = Socket::Foreign.listen_socket(fd, 5)
    if ret != 0
      Errno.handle "Unable to listen on #{@host}:#{@port}"
    end
  end

  def accept
    return if closed?
    fd = -1
    size = 0
    MemoryPointer.new :int do |sz|
      sz.write_int @sockaddr_size # initialize to the 'expected' size
      fd = Socket::Foreign.accept @descriptor, @sockaddr, sz
      size = sz.read_int
    end
    if fd < 0
      Errno.handle "Unable to accept on socket"
    end

    socket = TCPSocket.new(@host, @port, true)
    socket.descriptor = fd

    socket
  end
end

