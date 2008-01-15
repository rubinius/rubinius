class SocketError < StandardError
end

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
    FFI.config_hash("socket").each do |name, value|
      const_set name, value
    end

    families = FFI.config_hash('socket').select { |name,| name =~ /^AF_/ }
    families = families.map { |name, value| [value, name] }

    AF_TO_FAMILY = Hash[*families.flatten]
  end

  module Foreign
    class AddrInfo < FFI::Struct
      config("rbx.platform.addrinfo", :ai_flags, :ai_family, :ai_socktype,
             :ai_protocol, :ai_addrlen, :ai_addr, :ai_canonname, :ai_next)
    end

    attach_function "socket", :create_socket, [:int, :int, :int], :int
    attach_function "connect", :connect_socket, [:int, :string, :int], :int
    attach_function "bind", :bind_socket, [:int, :string, :int], :int
    attach_function "listen", :listen_socket, [:int, :int], :int
    attach_function "accept", :accept, [:int, :string, :pointer], :int
    attach_function "setsockopt", :set_socket_option,
                    [:int, :int, :int, :pointer, :int], :int
    attach_function "getsockopt", :get_socket_option,
                    [:int, :int, :int, :pointer, :pointer], :int

    attach_function "gai_strerror", :gai_strerror, [:int], :string

    attach_function "getaddrinfo", :getaddrinfo,
                    [:string, :string, :pointer, :pointer], :int
    attach_function "freeaddrinfo", :freeaddrinfo, [:pointer], :void

    #attach_function "ffi_pack_sockaddr_un", :pack_sa_unix,
    #                [:state, :string], :object
    attach_function "ffi_pack_sockaddr_in", :ffi_pack_sockaddr_in,
                    [:state, :string, :string, :int, :int], :object
    attach_function "ffi_decode_sockaddr", :ffi_decode_sockaddr,
                    [:state, :string, :int, :int], :object
    attach_function "ffi_getpeername", :ffi_getpeername,
                    [:state, :int, :int], :object
    attach_function "ffi_getsockname", :getsockname,
                    [:state, :int, :int], :object
    attach_function "ffi_bind", :bind_name, [:int, :string, :string, :int], :int

    def self.getpeername(socket, reverse_lookup = false)
      reverse_lookup = reverse_lookup ? 1 : 0

      result = ffi_getpeername socket, reverse_lookup

      ok, value = result.to_a

      raise SocketError, value unless ok

      value
    end

    def self.pack_sa_ip(name, port, type, flags)
      result = ffi_pack_sockaddr_in name, port, type, flags

      ok, value = result.to_a

      raise SocketError, value unless ok

      value
    end

    def self.unpack_sa_ip(sockaddr, reverse_lookup)
      reverse_lookup = reverse_lookup ? 1 : 0

      result = ffi_decode_sockaddr sockaddr, sockaddr.length, reverse_lookup

      raise SocketError, result if String === result

      result.to_a
    end
  end

  include Socket::Constants

  class SockAddr_In < FFI::Struct
    config("rbx.platform.sockaddr_in", :sin_family, :sin_port, :sin_addr, :sin_zero)

    def initialize(sockaddrin)
      @p = FFI::MemoryPointer.new sockaddrin.size
      @p.write_string(sockaddrin)
      super(@p)
    end

    def to_s
      @p.read_string(@p.size)
    end

  end

  class SockAddr_Un < FFI::Struct
    config("rbx.platform.sockaddr_un", :sun_family, :sun_path)

    def initialize(filename)
      maxfnsize = self.size - ( FFI.config("sockaddr_un.sun_family.size") + 1 )

      if(filename.length > maxfnsize )
        raise ArgumentError, "too long unix socket path (max: #{fnsize}bytes)"
      end
      @p = FFI::MemoryPointer.new self.size
      @p.write_string( [Socket::AF_UNIX].pack("s") + filename )
      super(@p)
    end

    def to_s
      @p.read_string(self.size)
    end
  end if (FFI.config("sockaddr_un.sun_family.offset") && Socket.const_defined?(:AF_UNIX))

  def self.getaddrinfo(host, service, family = nil, socktype = nil,
                       protocol = nil, flags = nil)
    service = service.to_s

    hints_p = MemoryPointer.new Socket::Foreign::AddrInfo.size
    hints = Socket::Foreign::AddrInfo.new hints_p
    hints[:ai_family] = family || 0
    hints[:ai_socktype] = socktype || 0
    hints[:ai_protocol] = protocol || 0
    hints[:ai_flags] = flags || 0

    res_p = MemoryPointer.new :pointer

    err = Socket::Foreign.getaddrinfo host, service, hints_p, res_p

    raise SocketError, Socket::Foreign.gai_strerror(err) unless err == 0

    res = Socket::Foreign::AddrInfo.new res_p.read_pointer

    addrinfos = []

    loop do
      addrinfo = []
      addrinfo << Socket::Constants::AF_TO_FAMILY[res[:ai_family]]

      ai_sockaddr = res[:ai_addr].read_string res[:ai_addrlen]

      sockaddr = Socket::Foreign::unpack_sa_ip ai_sockaddr, true

      addrinfo << sockaddr.pop # port
      addrinfo.concat sockaddr # hosts
      addrinfo << res[:ai_family]
      addrinfo << res[:ai_socktype]
      addrinfo << res[:ai_protocol]

      addrinfos << addrinfo

      break unless res[:ai_next]

      res = Socket::Foreign::AddrInfo.new res[:ai_next]
    end

    return addrinfos
  ensure
    hints_p.free if hints_p

    if res_p then
      Socket::Foreign.freeaddrinfo res_p.read_pointer
      res_p.free
    end
  end

  def self.pack_sockaddr_in(port, host, type = 0, flags = 0)
    host = "0.0.0.0" if host.empty?
    Socket::Foreign.pack_sa_ip(host.to_s, port.to_s, type, flags)
  end

  def self.unpack_sockaddr_in(sockaddr)
    host, address, port = Socket::Foreign.unpack_sa_ip sockaddr, false

    return [port, address]
  rescue SocketError => e
    if e.message =~ /ai_family not supported/ then # HACK platform specific?
      raise ArgumentError, 'not an AF_INET/AF_INET6 sockaddr'
    else
      raise
    end
  end

  class << self
    alias_method :sockaddr_in, :pack_sockaddr_in
  end

  # Only define these methods if we support unix sockets
  if self.const_defined?(:SockAddr_Un)
    def self.pack_sockaddr_un(file)
      SockAddr_Un.new(file).to_s
    end

    class << self
      alias_method :sockaddr_un, :pack_sockaddr_un
    end
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
    reverse = !BasicSocket.do_not_reverse_lookup

    name, addr, port = Socket::Foreign.getpeername descriptor, reverse

    raise SocketError, "Unable to get peer address" if addr.nil?

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

    @sockaddr = Socket.pack_sockaddr_in(@port, @host, @type)

    ret = Socket::Foreign.bind_socket(descriptor, @sockaddr, @sockaddr.size)
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

    name, addr, port = Socket::Foreign.getpeername fixnum, false

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

