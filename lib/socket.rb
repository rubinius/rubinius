require 'fcntl'

class SocketError < StandardError
end

# @todo   Socket#accept[_nonblock]
# @todo   UNIXServer#accept[_nonblock]
# @todo   UDPSocket#recvfrom

class BasicSocket < IO

  class << self
    def from_descriptor(fixnum)
      sock = allocate()
      sock.from_descriptor(fixnum)
      return sock
    end

    alias :for_fd :from_descriptor
  end

  def from_descriptor(fixnum)
    IO.setup self, fixnum, nil, true
    return self
  end

  def self.do_not_reverse_lookup=(setting)
    @no_reverse_lookup = setting
  end

  def self.do_not_reverse_lookup
    @no_reverse_lookup ? true : false
  end

  def getsockopt(level, optname)
    Socket::Foreign.getsockopt descriptor, level, optname
  end

  def setsockopt(level, optname, optval)
    optval = 1 if optval == true
    optval = 0 if optval == false

    error = 0

    case optval
    when Fixnum then
      FFI::MemoryPointer.new :socklen_t do |val|
        val.write_int optval
        error = Socket::Foreign.setsockopt(descriptor, level,
                                           optname, val,
                                           val.total)
      end
    when String then
      FFI::MemoryPointer.new optval.size do |val|
        val.write_string optval
        error = Socket::Foreign.setsockopt(descriptor, level,
                                           optname, val,
                                           optval.size)
      end
    else
      raise "socket option should be a String, a Fixnum, true, or false"
    end

    Errno.handle "Unable to set socket option" unless error == 0

    return 0
  end

  def getsockname()
    return Socket::Foreign.getsockname(descriptor)
  end

  #
  # Obtain peername information for this socket.
  #
  # @see  Socket.getpeername
  #
  def getpeername()
    Socket::Foreign.getpeername @descriptor
  end

  #
  #
  #
  def send(message, flags, to = nil)
    connect to if to

    bytes = message.length
    bytes_sent = 0

    FFI::MemoryPointer.new :char, bytes + 1 do |buffer|
      buffer.write_string message
      bytes_sent = Socket::Foreign.send(descriptor, buffer, bytes, flags)
      Errno.handle 'send(2)' if bytes_sent < 0
    end

    bytes_sent
  end

  def recvfrom(bytes_to_read, flags = 0)
    # FIXME 0 is knowledge from io.cpp
    return socket_recv(bytes_to_read, flags, 0)
  end

  def recv(bytes_to_read, flags = 0)
    # FIXME 0 is knowledge from io.cpp
    return socket_recv(bytes_to_read, flags, 0)
  end

  def close_read
    ensure_open_and_readable

    # If we were only in readonly mode, close it all together
    if @mode & ACCMODE == RDONLY
      return close
    end

    # MRI doesn't check if shutdown worked, so we don't.
    Socket::Foreign.shutdown @descriptor, 0

    @mode = @mode & ~RDONLY

    nil
  end

  def close_write
    ensure_open_and_writable

    # If we were only in writeonly mode, close it all together
    if @mode & ACCMODE == WRONLY
      return close
    end

    Socket::Foreign.shutdown @descriptor, 1

    # Remove write from the mode bits
    @mode = @mode & ~WRONLY

    nil
  end

  #
  # Sets socket nonblocking and reads up to given number of bytes.
  #
  # @todo   Should EWOULDBLOCK be passed unchanged? --rue
  #
  def recv_nonblock(bytes_to_read, flags = 0)
    fcntl Fcntl::F_SETFL, Fcntl::O_NONBLOCK
    recvfrom bytes_to_read, flags

  rescue Errno::EWOULDBLOCK
    raise Errno::EAGAIN
  end


#

end

class Socket < BasicSocket

  # @todo   Is omitting empty-value constants reasonable? --rue
  module Constants
    all_valid = FFI.config_hash("socket").reject {|name, value| value.empty? }

    all_valid.each {|name, value| const_set name, Integer(value) }

    # MRI compat. socket is a pretty screwed up API. All the constants in Constants
    # must also be directly accessible on Socket itself. This means it's not enough
    # to include Constants into Socket, because Socket#const_defined? must be able
    # to see constants like AF_INET6 directly on Socket, but #const_defined? doesn't
    # check inherited constants. O_o
    #
    all_valid.each {|name, value| Socket.const_set name, Integer(value) }


    afamilies = all_valid.select { |name,| name =~ /^AF_/ }
    afamilies.map! {|name, value| [value.to_i, name] }

    pfamilies = all_valid.select { |name,| name =~ /^PF_/ }
    pfamilies.map! {|name, value| [value.to_i, name] }

    AF_TO_FAMILY = Hash[*afamilies.flatten]
    PF_TO_FAMILY = Hash[*pfamilies.flatten]
  end

  module Foreign
    extend FFI::Library

    class AddrInfo < FFI::Struct
      config("rbx.platform.addrinfo", :ai_flags, :ai_family, :ai_socktype,
             :ai_protocol, :ai_addrlen, :ai_addr, :ai_canonname, :ai_next)
    end

    attach_function :_bind,    "bind", [:int, :pointer, :socklen_t], :int
    attach_function :_connect, "connect", [:int, :pointer, :socklen_t], :int

    attach_function :accept,   [:int, :pointer, :pointer], :int
    attach_function :close,    [:int], :int
    attach_function :shutdown, [:int, :int], :int
    attach_function :listen,   [:int, :int], :int
    attach_function :socket,   [:int, :int, :int], :int
    attach_function :send,     [:int, :pointer, :int, :int], :int
    attach_function :recv,     [:int, :pointer, :int, :int], :int
    attach_function :recvfrom, [:int, :pointer, :size_t, :int,
                                :pointer, :pointer], :int

    attach_function :_getsockopt,
                    "getsockopt", [:int, :int, :int, :pointer, :pointer], :int
    attach_function :_getaddrinfo,
                    "getaddrinfo", [:string, :string, :pointer, :pointer], :int

    attach_function :gai_strerror,  [:int], :string
    attach_function :setsockopt,    [:int, :int, :int, :pointer, :socklen_t], :int
    attach_function :freeaddrinfo,  [:pointer], :void
    attach_function :_getpeername,  "getpeername", [:int, :pointer, :pointer], :int
    attach_function :_getsockname,  "getsockname", [:int, :pointer, :pointer], :int

    attach_function :socketpair,    [:int, :int, :int, :pointer], :int

    attach_function :gethostname,   [:pointer, :size_t], :int
    attach_function :getservbyname, [:pointer, :pointer], :pointer

    attach_function :htons,         [:u_int16_t], :u_int16_t
    attach_function :ntohs,         [:u_int16_t], :u_int16_t

    attach_function :_getnameinfo,
                    "getnameinfo", [:pointer, :socklen_t, :pointer, :socklen_t,
                                    :pointer, :socklen_t, :int], :int

    def self.bind(descriptor, sockaddr)
      FFI::MemoryPointer.new :char, sockaddr.length do |sockaddr_p|
        sockaddr_p.write_string sockaddr, sockaddr.length

        _bind descriptor, sockaddr_p, sockaddr.length
      end
    end

    def self.connect(descriptor, sockaddr)
      err = 0
      FFI::MemoryPointer.new :char, sockaddr.length do |sockaddr_p|
        sockaddr_p.write_string sockaddr, sockaddr.length

        err = _connect descriptor, sockaddr_p, sockaddr.length
      end

      getsockopt(descriptor, Socket::SOL_SOCKET, Socket::SO_ERROR) if err < 0
      err
    end

    def self.getsockopt(descriptor, level, optname)
      FFI::MemoryPointer.new 256 do |val| # HACK magic number
        FFI::MemoryPointer.new :socklen_t do |length|
          length.write_int 256 # HACK magic number

          err = _getsockopt descriptor, level, optname, val, length

          Errno.handle "Unable to get socket option" unless err == 0

          return val.read_string(length.read_int)
        end
      end
    end

    def self.getaddrinfo(host, service = nil, family = 0, socktype = 0,  protocol = 0, flags = 0)
      hints = AddrInfo.new
      hints[:ai_family] = family
      hints[:ai_socktype] = socktype
      hints[:ai_protocol] = protocol
      hints[:ai_flags] = flags
      host = "" if host.nil?

      if host.empty?
        if (flags & Socket::AI_PASSIVE == 1) # Passive socket
          host = (family == Socket::AF_INET6 ? "::"  : "0.0.0.0") # IPv6 or IPv4
        else
          host = (family == Socket::AF_INET6 ? "::1" : "127.0.0.1")
        end
      end

      res_p = FFI::MemoryPointer.new :pointer

      err = _getaddrinfo host, service, hints.pointer, res_p

      raise SocketError, gai_strerror(err) unless err == 0

      ptr = res_p.read_pointer

      return [] unless ptr

      res = AddrInfo.new ptr

      addrinfos = []

      while true
        addrinfo = []
        addrinfo << res[:ai_flags]
        addrinfo << res[:ai_family]
        addrinfo << res[:ai_socktype]
        addrinfo << res[:ai_protocol]
        addrinfo << res[:ai_addr].read_string(res[:ai_addrlen])
        addrinfo << res[:ai_canonname]

        addrinfos << addrinfo

        break unless res[:ai_next]

        res = AddrInfo.new res[:ai_next]
      end

      return addrinfos
    ensure
      hints.free if hints

      if res_p
        ptr = res_p.read_pointer

        # Be sure to feed a legit pointer to freeaddrinfo
        if ptr and !ptr.null?
          freeaddrinfo ptr
        end
        res_p.free
      end
    end

    def self.getaddress(host)
      addrinfos = getaddrinfo(host)
      unpack_sockaddr_in(addrinfos.first[4], false).first
    end

    def self.getnameinfo(sockaddr,
                         reverse_lookup = !BasicSocket.do_not_reverse_lookup)
      name_info = []
      value = nil

      FFI::MemoryPointer.new :char, sockaddr.length do |sockaddr_p|
        FFI::MemoryPointer.new :char, Socket::Constants::NI_MAXHOST do |node|
          FFI::MemoryPointer.new :char, Socket::Constants::NI_MAXSERV do |service|
            sockaddr_p.write_string sockaddr, sockaddr.length

            if reverse_lookup then
              err = _getnameinfo(sockaddr_p, sockaddr.length,
                                 node, Socket::Constants::NI_MAXHOST, nil, 0, 0)

              unless err == 0 then
                raise SocketError, gai_strerror(err)
              end

              name_info[2] = node.read_string
            end

            err = _getnameinfo(sockaddr_p, sockaddr.length,
                               node, Socket::Constants::NI_MAXHOST,
                               service, Socket::Constants::NI_MAXSERV,
                               Socket::Constants::NI_NUMERICHOST |
                                 Socket::Constants::NI_NUMERICSERV)

            unless err == 0 then
              raise SocketError, gai_strerror(err)
            end

            sa_family = SockAddr_In.new(sockaddr)[:sin_family]

            name_info[0] = Socket::Constants::AF_TO_FAMILY[sa_family]
            name_info[1] = Integer service.read_string
            name_info[3] = node.read_string
          end
        end
      end

      name_info[2] = name_info[3] if name_info[2].nil?
      name_info
    end

    def self.getpeername(descriptor)
      FFI::MemoryPointer.new :char, 128 do |sockaddr_storage_p|
        FFI::MemoryPointer.new :socklen_t do |len_p|
          len_p.write_int 128

          err = _getpeername descriptor, sockaddr_storage_p, len_p

          Errno.handle 'getpeername(2)' unless err == 0

          sockaddr_storage_p.read_string len_p.read_int
        end
      end
    end

    def self.getsockname(descriptor)
      FFI::MemoryPointer.new :char, 128 do |sockaddr_storage_p|
        FFI::MemoryPointer.new :socklen_t do |len_p|
          len_p.write_int 128

          err = _getsockname descriptor, sockaddr_storage_p, len_p

          Errno.handle 'getsockname(2)' unless err == 0

          sockaddr_storage_p.read_string len_p.read_int
        end
      end
    end

    def self.pack_sockaddr_in(name, port, type, flags)
      hints = AddrInfo.new
      hints[:ai_family] = Socket::AF_INET
      hints[:ai_socktype] = type
      hints[:ai_flags] = flags

      res_p = FFI::MemoryPointer.new :pointer

      err = _getaddrinfo name, port, hints.pointer, res_p

      raise SocketError, gai_strerror(err) unless err == 0

      return [] if res_p.read_pointer.null?

      res = AddrInfo.new res_p.read_pointer

      return res[:ai_addr].read_string(res[:ai_addrlen])

    ensure
      hints.free if hints

      if res_p then
        ptr = res_p.read_pointer

        freeaddrinfo ptr if ptr and not ptr.null?

        res_p.free
      end
    end

    def self.unpack_sockaddr_in(sockaddr, reverse_lookup)
      family, port, host, ip = getnameinfo sockaddr, reverse_lookup
      # On some systems this doesn't fail for families other than AF_INET(6)
      # so we raise manually here.
      raise ArgumentError, 'not an AF_INET/AF_INET6 sockaddr' unless family =~ /AF_INET/
      return host, ip, port
    end
  end

  module ListenAndAccept
    include IO::Socketable

    def listen(backlog)
      backlog = Type.coerce_to backlog, Fixnum, :to_int

      err = Socket::Foreign.listen descriptor, backlog

      Errno.handle 'listen(2)' unless err == 0

      err
    end

    def accept
      return if closed?

      fd = super

      socket = self.class.superclass.allocate
      IO.setup socket, fd, nil, true
      socket
    end

    #
    # Set nonblocking and accept.
    #
    def accept_nonblock
      return if closed?

      fcntl(Fcntl::F_SETFL, Fcntl::O_NONBLOCK)

      fd = nil
      sockaddr = nil

      FFI::MemoryPointer.new 1024 do |sockaddr_p| # HACK from MRI
        FFI::MemoryPointer.new :int do |size_p|
          fd = Socket::Foreign.accept descriptor, sockaddr_p, size_p
        end
      end

      Errno.handle 'accept(2)' if fd < 0

      # TCPServer -> TCPSocket etc. *sigh*
      socket = self.class.superclass.allocate
      IO.setup socket, fd, nil, true
      socket
    end

  end

  include Socket::ListenAndAccept

  class SockAddr_In < FFI::Struct
    config("rbx.platform.sockaddr_in", :sin_family, :sin_port, :sin_addr, :sin_zero)

    def initialize(sockaddrin)
      @p = FFI::MemoryPointer.new sockaddrin.size
      @p.write_string(sockaddrin)
      super(@p)
    end

    def to_s
      @p.read_string(@p.total)
    end

  end

  # If we have the details to support unix sockets, do so.
  if FFI.config("sockaddr_un.sun_family.offset") and Socket::Constants.const_defined?(:AF_UNIX)
    class SockAddr_Un < FFI::Struct
      config("rbx.platform.sockaddr_un", :sun_family, :sun_path)

      def initialize(filename = nil)
        maxfnsize = self.size - (FFI.config("sockaddr_un.sun_family.size") + 1)

        if filename and filename.length > maxfnsize
          raise ArgumentError, "too long unix socket path (max: #{maxfnsize}bytes)"
        end
        @p = FFI::MemoryPointer.new self.size
        if filename
          @p.write_string( [Socket::AF_UNIX].pack("s") + filename )
        end
        super @p
      end

      def to_s
        @p.read_string self.size
      end
    end
  end

  def self.getaddrinfo(host, service, family = nil, socktype = nil,
                       protocol = nil, flags = nil)
    if service
      if service.kind_of? Fixnum
        service = service.to_s
      else
        service = StringValue(service)
      end
    end

    family ||= 0
    socktype ||= 0
    protocol ||= 0
    flags ||= 0

    addrinfos = Socket::Foreign.getaddrinfo(host, service, family, socktype,
                                            protocol, flags)

    addrinfos.map do |ai|
      addrinfo = []
      addrinfo << Socket::Constants::AF_TO_FAMILY[ai[1]]

      sockaddr = Foreign.unpack_sockaddr_in ai[4], true

      addrinfo << sockaddr.pop # port
      addrinfo.concat sockaddr # hosts
      addrinfo << ai[1]
      addrinfo << ai[2]
      addrinfo << ai[3]
      addrinfo
    end
  end

  def self.gethostname
    FFI::MemoryPointer.new :char, 1024 do |mp|  #magic number 1024 comes from MRI
      Socket::Foreign.gethostname(mp, 1024) # same here
      return mp.read_string
    end
  end

  def self.gethostbyname(hostname)
    addrinfos = Socket.getaddrinfo(hostname, nil)

    hostname     = addrinfos.first[2]
    family       = addrinfos.first[4]
    addresses    = []
    alternatives = []
    addrinfos.each do |a|
      alternatives << a[2] unless a[2] == hostname
      addresses    << a[3] if a[4] == family
    end

    [hostname, alternatives.uniq, family] + addresses.uniq
  end


  class Servent < FFI::Struct
    config("rbx.platform.servent", :s_name, :s_aliases, :s_port, :s_proto)

    def initialize(data)
      @p = FFI::MemoryPointer.new data.size
      @p.write_string(data)
      super(@p)
    end

    def to_s
      @p.read_string(size)
    end

  end

  def self.getservbyname(service, proto='tcp')
    FFI::MemoryPointer.new :char, service.length + 1 do |svc|
      FFI::MemoryPointer.new :char, proto.length + 1 do |prot|
        svc.write_string(service + "\0")
        prot.write_string(proto + "\0")
        fn = Socket::Foreign.getservbyname(svc, prot)

        raise SocketError, "no such service #{service}/#{proto}" if fn.nil?

        s = Servent.new(fn.read_string(Servent.size))
        return Socket::Foreign.ntohs(s[:s_port])
      end
    end
  end

  def self.pack_sockaddr_in(port, host, type = Socket::SOCK_DGRAM, flags = 0)
    if host.nil?
      host = "127.0.0.1"
    elsif host.empty?
      host = "0.0.0.0"
    end
    Socket::Foreign.pack_sockaddr_in host.to_s, port.to_s, type, flags
  end

  def self.unpack_sockaddr_in(sockaddr)
    host, address, port = Socket::Foreign.unpack_sockaddr_in sockaddr, false

    return [port, address]
  rescue SocketError => e
    if e.message =~ /ai_family not supported/ then # HACK platform specific?
      raise ArgumentError, 'not an AF_INET/AF_INET6 sockaddr'
    else
      raise e
    end
  end

  def self.socketpair(domain, type, protocol, klass=self)
    if domain.kind_of? String
      if domain.prefix? "AF_" or domain.prefix? "PF_"
        begin
          domain = Socket::Constants.const_get(domain)
        rescue NameError
          raise SocketError, "unknown socket domain #{domani}"
        end
      else
        raise SocketError, "unknown socket domain #{domani}"
      end
    end

    if type.kind_of? String
      if type.prefix? "SOCK_"
        begin
          type = Socket::Constants.const_get(type)
        rescue NameError
          raise SocketError, "unknown socket type #{type}"
        end
      else
        raise SocketError, "unknown socket type #{type}"
      end
    end

    FFI::MemoryPointer.new :int, 2 do |mp|
      Socket::Foreign.socketpair(domain, type, protocol, mp)
      fd0, fd1 = mp.read_array_of_int(2)

      [ klass.from_descriptor(fd0), klass.from_descriptor(fd1) ]
    end
  end

  class << self
    alias_method :sockaddr_in, :pack_sockaddr_in
    alias_method :pair, :socketpair
  end

  # Only define these methods if we support unix sockets
  if self.const_defined?(:SockAddr_Un)
    def self.pack_sockaddr_un(file)
      SockAddr_Un.new(file).to_s
    end

    def self.unpack_sockaddr_un(addr)

      if addr.length > FFI.config("sockaddr_un.sizeof")
        raise TypeError, "too long sockaddr_un - #{addr.length} longer than #{FFI.config("sockaddr_un.sizeof")}"
      end

      struct = SockAddr_Un.new
      struct.pointer.write_string(addr)

      struct[:sun_path]
    end

    class << self
      alias_method :sockaddr_un, :pack_sockaddr_un
    end
  end

  def initialize(family, socket_type, protocol)
    descriptor = Socket::Foreign.socket family, socket_type, protocol

    Errno.handle 'socket(2)' if descriptor < 0

    IO.setup self, descriptor, nil, true
  end

  def bind(server_sockaddr)
    err = Socket::Foreign.bind(descriptor, server_sockaddr)
    Errno.handle 'bind(2)' unless err == 0
  end

  # @todo  Should this be closing the descriptor? --rue
  def connect(*args)
    sockaddr  = if args.size == 1
                  args.first
                else
                  Socket.pack_sockaddr_in args.first, args.last
                end

    syscall = 'connect(2)'
    status = Socket::Foreign.connect descriptor, sockaddr

    if status < 0 then
      Socket::Foreign.close descriptor
      Errno.handle syscall
    end

    return 0
  end
end

class UNIXSocket < BasicSocket
  attr_accessor :path

  # Coding to the lowest standard here.
  def recvfrom(bytes_read, flags = 0)
    # FIXME 2 is hardcoded knowledge from io.cpp
    socket_recv(bytes_read, flags, 2)
  end

  def initialize(path)
    @path = path
    unix_setup
    @path = ""  # Client
  end
  private :initialize

  def from_descriptor(fixnum)
    super
    @path = ""
  end

  def unix_setup(server = false)
    status = nil
    phase = 'socket(2)'
    sock = Socket::Foreign.socket Socket::Constants::AF_UNIX, Socket::Constants::SOCK_STREAM, 0

    IO.setup self, sock, 'r+', true

    Errno.handle phase if descriptor < 0

    sockaddr = Socket.pack_sockaddr_un(@path)

    if server then
      phase = 'bind(2)'
      status = Socket::Foreign.bind descriptor, sockaddr
    else
      phase = 'connect(2)'
      status = Socket::Foreign.connect descriptor, sockaddr
    end

    if status < 0 then
      Socket::Foreign.close descriptor
      Errno.handle phase
    end

    if server then
      phase = 'listen(2)'
      status = Socket::Foreign.listen descriptor, 5
      Errno.handle phase if status < 0
    end

    return sock
  end
  private :unix_setup

  def addr
    sockaddr = Socket::Foreign.getsockname descriptor
    _, sock_path = sockaddr.unpack('SZ*')
    ["AF_UNIX", sock_path]
  end

  def peeraddr
    sockaddr = Socket::Foreign.getpeername descriptor
    _, sock_path = sockaddr.unpack('SZ*')
    ["AF_UNIX", sock_path]
  end

  class << self
    def socketpair(type=Socket::SOCK_STREAM, protocol=0)
      Socket.socketpair(Socket::PF_UNIX, type, protocol, self)
    end

    alias_method :pair, :socketpair
  end

end

class UNIXServer < UNIXSocket

  include Socket::ListenAndAccept

  def initialize(path)
    @path = path
    unix_setup(true)
  end
  private :initialize
end

class IPSocket < BasicSocket

  def self.getaddress(host)
    Socket::Foreign.getaddress host
  end

  def addr
    sockaddr = Socket::Foreign.getsockname descriptor

    Socket::Foreign.getnameinfo sockaddr
  end

  def peeraddr
    sockaddr = Socket::Foreign.getpeername descriptor

    Socket::Foreign.getnameinfo sockaddr
  end

  def recvfrom(maxlen, flags = 0)
    # FIXME 1 is hardcoded knowledge from io.cpp
    socket_recv maxlen, flags, 1
  end

  def recvfrom_nonblock(maxlen, flags = 0)
    # Set socket to non-blocking, if we can
    # Todo: Ensure this works in Windows!  If not, I claim that's Fcntl's fault.
    fcntl(Fcntl::F_SETFL, Fcntl::O_NONBLOCK)

    # Wait until we have something to read
    # @todo  Why? ^^ --rue
    IO.select([self])
    return recvfrom(maxlen, flags)
  end
end

class UDPSocket < IPSocket

  def initialize(socktype = Socket::AF_INET)
    status = Socket::Foreign.socket socktype,
                                    Socket::SOCK_DGRAM,
                                    Socket::IPPROTO_UDP
    Errno.handle 'socket(2)' if status < 0

    IO.setup self, status, nil, true
  end

  def bind(host, port)
    @host = host.to_s if host
    @port = port.to_s if port

    addrinfos = Socket::Foreign.getaddrinfo(@host,
                                           @port,
                                           Socket::AF_UNSPEC,
                                           Socket::SOCK_DGRAM, 0,
                                           Socket::AI_PASSIVE)

    status = -1

    addrinfos.each do |addrinfo|
      flags, family, socket_type, protocol, sockaddr, canonname = addrinfo

      status = Socket::Foreign.bind descriptor, sockaddr

      break if status >= 0
    end

    if status < 0
      Errno.handle 'bind(2)'
      Socket::Foreign.close descriptor
    end

    status
  end

  def connect(host, port)
    sockaddr = Socket.pack_sockaddr_in(port, host)
    syscall = 'connect(2)'
    status = Socket::Foreign.connect descriptor, sockaddr

    if status < 0
      Socket::Foreign.close descriptor
      Errno.handle syscall
    end

    0
  end

  def send(message, flags, *to)
    connect *to unless to.empty?

    bytes = message.length
    bytes_sent = 0

    FFI::MemoryPointer.new :char, bytes + 1 do |buffer|
      buffer.write_string message
      bytes_sent = Socket::Foreign.send(descriptor, buffer, bytes, flags)
      Errno.handle 'send(2)' if bytes_sent < 0
    end

    bytes_sent
  end

  def inspect
    "#<#{self.class}:0x#{object_id.to_s(16)} #{@host}:#{@port}>"
  end

end

class TCPSocket < IPSocket

  def self.gethostbyname(hostname)
    addrinfos = Socket.getaddrinfo(hostname, nil)

    hostname     = addrinfos.first[2]
    family       = addrinfos.first[4]
    addresses    = []
    alternatives = []
    addrinfos.each do |a|
      alternatives << a[2] unless a[2] == hostname
      addresses    << a[3] if a[4] == family
    end

    [hostname, alternatives.uniq, family] + addresses.uniq
  end

  #
  # @todo   Is it correct to ignore the to? If not, does
  #         the socket need to be reconnected? --rue
  #
  def send(bytes_to_read, flags, to = nil)
    super(bytes_to_read, flags)
  end


  def initialize(host, port)
    @host = host
    @port = port

    tcp_setup @host, @port
  end
  private :initialize

  def tcp_setup(remote_host, remote_service, local_host = nil,
                local_service = nil, server = false)
    status = nil
    syscall = nil
    remote_host    = remote_host.to_s    if remote_host
    remote_service = remote_service.to_s if remote_service

    flags = server ? Socket::AI_PASSIVE : 0
    @remote_addrinfo = Socket::Foreign.getaddrinfo(remote_host,
                                                   remote_service,
                                                   Socket::AF_UNSPEC,
                                                   Socket::SOCK_STREAM, 0,
                                                   flags)

    if server == false and (local_host or local_service) then
      @local_addrinfo = Socket::Foreign.getaddrinfo(local_host,
                                                    local_service,
                                                    Socket::AF_UNSPEC,
                                                    Socket::SOCK_STREAM, 0, 0)
    end

    @remote_addrinfo.each do |addrinfo|
      flags, family, socket_type, protocol, sockaddr, canonname = addrinfo

      status = Socket::Foreign.socket family, socket_type, protocol
      syscall = 'socket(2)'
      IO.setup self, status, nil, true

      next if descriptor < 0

      if server then
        status = 1

        begin
          setsockopt(Socket::Constants::SOL_SOCKET,
                     Socket::Constants::SO_REUSEADDR, true)
        rescue SystemCallError
        end

        status = Socket::Foreign.bind descriptor, sockaddr
        syscall = 'bind(2)'
      else
        if @local_addrinfo then
          status = bind descriptor, @local_addrinfo.first[4]
          syscall = 'bind(2)'
        end

        if status >= 0 then
          status = Socket::Foreign.connect descriptor, sockaddr
          syscall = 'connect(2)'
        end
      end

      break if status >= 0
    end

    if status < 0
      Errno.handle syscall
      Socket::Foreign.close descriptor
    end

    if server then
      err = Socket::Foreign.listen descriptor, 5
      Errno.handle syscall unless err == 0
    end

    IO.setup self, descriptor, nil, true
  end
  private :tcp_setup

  def from_descriptor(descriptor)
    IO.setup self, descriptor, nil, true

    self
  end
  private :from_descriptor

end

class TCPServer < TCPSocket

  include Socket::ListenAndAccept

  def initialize(host, port = nil)
    if Fixnum === host and port.nil? then
      port = host
      host = nil
    end

    port = StringValue port unless port.kind_of? Fixnum

    @host = host
    @port = port

    tcp_setup @host, @port, nil, nil, true
  end

end
