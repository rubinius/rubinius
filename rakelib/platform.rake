require 'lib/ffi/struct_generator'

file 'runtime/platform.conf' => %w[Rakefile rakelib/platform.rake lib/ffi/struct_generator.rb] do |task|
  addrinfo = FFI::StructGenerator.new do |s|
    s.include 'sys/socket.h'
    s.include 'netdb.h'
    s.name 'struct addrinfo'
    s.field :ai_flags, :int
    s.field :ai_family, :int
    s.field :ai_socktype, :int
    s.field :ai_protocol, :int
    s.field :ai_addrlen, :int
    s.field :ai_addr, :pointer
    s.field :ai_canonname, :string
    s.field :ai_next, :pointer
  end

  dirent = FFI::StructGenerator.new do |s|
    s.include "sys/types.h"
    s.include "dirent.h"
    s.name 'struct dirent'
    s.field :d_ino, :ino_t
    s.field :d_reclen, :ushort
    s.field :d_name, :char_array
  end

  timeval = FFI::StructGenerator.new do |s|
    s.include "sys/time.h"
    s.name 'struct timeval'
    s.field :tv_sec, :time_t
    s.field :tv_usec, :suseconds_t
  end

  sockaddr_in = FFI::StructGenerator.new do |s|
    s.include "netinet/in.h"
    s.include "fcntl.h"
    s.include "sys/socket.h"
    s.include "sys/stat.h"
    s.name 'struct sockaddr_in'
    s.field :sin_family, :sa_family_t
    s.field :sin_port, :ushort
    s.field :sin_addr
    s.field :sin_zero, :char_array
  end

  sockaddr_un = FFI::StructGenerator.new do |s|
    s.include "sys/un.h"
    s.name 'struct sockaddr_un'
    s.field :sun_family, :sa_family_t
    s.field :sun_path, :char_array
  end

  servent = FFI::StructGenerator.new do |s|
    s.include "netdb.h"
    s.name 'struct servent'
    s.field :s_name, :pointer
    s.field :s_aliases, :pointer
    s.field :s_port, :int
    s.field :s_proto, :pointer
  end
  
  stat = FFI::StructGenerator.new do |s|
    s.include "sys/types.h"
    s.include "sys/stat.h"
    s.name 'struct stat'
    s.field :st_dev, :dev_t
    s.field :st_ino, :ino_t
    s.field :st_mode, :mode_t
    s.field :st_nlink, :nlink_t
    s.field :st_uid, :uid_t
    s.field :st_gid, :gid_t
    s.field :st_rdev, :dev_t
    s.field :st_size, :off_t
    s.field :st_blksize
    s.field :st_blocks
    s.field :st_atime, :time_t
    s.field :st_mtime, :time_t
    s.field :st_ctime, :time_t
  end
  
  rlimit = FFI::StructGenerator.new do |s|
    s.include "sys/types.h"
    s.include "sys/time.h"
    s.include "sys/resource.h"
    s.name 'struct rlimit'
    s.field :rlim_cur, :rlim_t
    s.field :rlim_max, :rlim_t
  end

  evp_md = FFI::StructGenerator.new do |s|
    s.include "openssl/ossl_typ.h"
    s.include "openssl/evp.h"
    s.name 'struct env_md_st'
    s.field  :type, :int
    s.field  :pkey_type, :int
    s.field  :md_size, :int
    s.field  :flags, :ulong
    s.field  :init, :pointer
    s.field  :update, :pointer
    s.field  :final, :pointer
    s.field  :copy, :pointer
    s.field  :cleanup, :pointer
    s.field  :sign, :pointer
    s.field  :verify, :pointer
    # int required_pkey[5] goes here but we don't care
    s.field  :block_size, :int
    s.field  :ctx_size, :int
  end

  evp_md_ctx = FFI::StructGenerator.new do |s|
    s.include "openssl/ossl_typ.h"
    s.include "openssl/evp.h"
    s.name 'struct env_md_ctx_st'
    s.field :digest, :pointer
    s.field :engine, :pointer
    s.field :flags, :ulong
    s.field :md_data, :pointer
  end
  
  # FIXME these constants don't have standard names.
  # LOCK_SH == Linux, O_SHLOCK on Bsd/Darwin, etc.
  # Binary doesn't exist at all in many non-Unix variants.
  # This should come out of something like config.h
  fixme_constants = %w{
    LOCK_SH
    LOCK_EX
    LOCK_NB
    LOCK_UN
    BINARY
  }
  
  syslog_constants = %w{
    LOG_PID
    LOG_EMERG
    LOG_ALERT
    LOG_ERR
    LOG_CRIT
    LOG_WARNING
    LOG_NOTICE
    LOG_INFO
    LOG_DEBUG
    LOG_CONS
    LOG_ODELAY
    LOG_NODELAY
    LOG_NOWAIT
    LOG_PERROR
    LOG_AUTH
    LOG_AUTHPRIV
    LOG_CONSOLE
    LOG_CRON
    LOG_DAEMON
    LOG_FTP
    LOG_KERN
    LOG_LPR
    LOG_MAIL
    LOG_NEWS
    LOG_NTP
    LOG_SECURITY
    LOG_SYSLOG
    LOG_USER
    LOG_UUCP
    LOG_LOCAL0
    LOG_LOCAL1
    LOG_LOCAL2
    LOG_LOCAL3
    LOG_LOCAL4
    LOG_LOCAL5
    LOG_LOCAL6
    LOG_LOCAL7
  }

  file_constants = %w{
    O_RDONLY
    O_WRONLY
    O_RDWR
    O_CREAT
    O_EXCL
    O_NOCTTY
    O_TRUNC
    O_APPEND
    O_NONBLOCK
    O_SYNC
    S_IRUSR
    S_IWUSR
    S_IXUSR
    S_IRGRP
    S_IWGRP
    S_IXGRP
    S_IROTH
    S_IWOTH
    S_IXOTH
    S_IFMT
    S_IFIFO
    S_IFCHR
    S_IFDIR
    S_IFBLK
    S_IFREG
    S_IFLNK
    S_IFSOCK
    S_IFWHT
    S_ISUID
    S_ISGID
  }

  io_constants = %w{
    SEEK_SET
    SEEK_CUR
    SEEK_END
  }

  fcntl_constants = %w{
    F_DUPFD
    F_GETFD
    F_GETLK
    F_SETFD
    F_GETFL
    F_SETFL
    F_SETLK
    F_SETLKW
    FD_CLOEXEC
    F_RDLCK
    F_UNLCK
    F_WRLCK
    O_CREAT
    O_EXCL
    O_NOCTTY
    O_TRUNC
    O_APPEND
    O_NONBLOCK
    O_NDELAY
    O_RDONLY
    O_RDWR
    O_WRONLY
    O_ACCMODE
  }

  socket_constants = %w[
    AF_APPLETALK
    AF_ATM
    AF_AX25
    AF_CCITT
    AF_CHAOS
    AF_CNT
    AF_COIP
    AF_DATAKIT
    AF_DEC
    AF_DLI
    AF_E164
    AF_ECMA
    AF_HYLINK
    AF_IMPLINK
    AF_INET
    AF_INET6
    AF_IPX
    AF_ISDN
    AF_ISO
    AF_LAT
    AF_LINK
    AF_LOCAL
    AF_MAX
    AF_NATM
    AF_NDRV
    AF_NETBIOS
    AF_NETGRAPH
    AF_NS
    AF_OSI
    AF_PPP
    AF_PUP
    AF_ROUTE
    AF_SIP
    AF_SNA
    AF_SYSTEM
    AF_UNIX
    AF_UNSPEC

    AI_ADDRCONFIG
    AI_ALL
    AI_CANONNAME
    AI_DEFAULT
    AI_MASK
    AI_NUMERICHOST
    AI_PASSIVE
    AI_V4MAPPED
    AI_V4MAPPED_CFG

    EAI_ADDRFAMILY
    EAI_AGAIN
    EAI_BADFLAGS
    EAI_BADHINTS
    EAI_FAIL
    EAI_FAMILY
    EAI_MAX
    EAI_MEMORY
    EAI_NODATA
    EAI_NONAME
    EAI_PROTOCOL
    EAI_SERVICE
    EAI_SOCKTYPE
    EAI_SYSTEM

    INADDR_ALLHOSTS_GROUP
    INADDR_ANY
    INADDR_BROADCAST
    INADDR_LOOPBACK
    INADDR_MAX_LOCAL_GROUP
    INADDR_NONE
    INADDR_UNSPEC_GROUP

    IPPORT_RESERVED
    IPPORT_USERRESERVED

    IPPROTO_BIP
    IPPROTO_EGP
    IPPROTO_EON
    IPPROTO_GGP
    IPPROTO_HELLO
    IPPROTO_ICMP
    IPPROTO_IDP
    IPPROTO_IGMP
    IPPROTO_IP
    IPPROTO_MAX
    IPPROTO_ND
    IPPROTO_PUP
    IPPROTO_RAW
    IPPROTO_TCP
    IPPROTO_TP
    IPPROTO_UDP
    IPPROTO_XTP

    IPX_TYPE

    IP_ADD_MEMBERSHIP
    IP_DEFAULT_MULTICAST_LOOP
    IP_DEFAULT_MULTICAST_TTL
    IP_DROP_MEMBERSHIP
    IP_HDRINCL
    IP_MAX_MEMBERSHIPS
    IP_MULTICAST_IF
    IP_MULTICAST_LOOP
    IP_MULTICAST_TTL
    IP_OPTIONS
    IP_RECVDSTADDR
    IP_RECVOPTS
    IP_RECVRETOPTS
    IP_RETOPTS
    IP_TOS
    IP_TTL

    MSG_COMPAT
    MSG_CTRUNC
    MSG_DONTROUTE
    MSG_DONTWAIT
    MSG_EOF
    MSG_EOR
    MSG_FLUSH
    MSG_HAVEMORE
    MSG_HOLD
    MSG_OOB
    MSG_PEEK
    MSG_RCVMORE
    MSG_SEND
    MSG_TRUNC
    MSG_WAITALL

    NI_DGRAM
    NI_MAXHOST
    NI_MAXSERV
    NI_NAMEREQD
    NI_NOFQDN
    NI_NUMERICHOST
    NI_NUMERICSERV

    PF_APPLETALK
    PF_AX25
    PF_CCITT
    PF_CHAOS
    PF_CNT
    PF_COIP
    PF_DATAKIT
    PF_DLI
    PF_ECMA
    PF_HYLINK
    PF_IMPLINK
    PF_INET
    PF_INET6
    PF_IPX
    PF_ISDN
    PF_ISO
    PF_KEY
    PF_LAT
    PF_LINK
    PF_LOCAL
    PF_MAX
    PF_NATM
    PF_NDRV
    PF_NETBIOS
    PF_NETGRAPH
    PF_NS
    PF_OSI
    PF_PIP
    PF_PPP
    PF_PUP
    PF_ROUTE
    PF_RTIP
    PF_SIP
    PF_SNA
    PF_SYSTEM
    PF_UNIX
    PF_UNSPEC
    PF_XTP

    SHUT_RD
    SHUT_RDWR
    SHUT_WR

    SOCK_DGRAM
    SOCK_PACKET
    SOCK_RAW
    SOCK_RDM
    SOCK_SEQPACKET
    SOCK_STREAM

    SOL_ATALK
    SOL_AX25
    SOL_IP
    SOL_IPX
    SOL_SOCKET
    SOL_TCP
    SOL_UDP

    SOPRI_BACKGROUND
    SOPRI_INTERACTIVE
    SOPRI_NORMAL

    SO_ACCEPTCONN
    SO_ACCEPTFILTER
    SO_ATTACH_FILTER
    SO_BINDTODEVICE
    SO_BROADCAST
    SO_DEBUG
    SO_DETACH_FILTER
    SO_DONTROUTE
    SO_DONTTRUNC
    SO_ERROR
    SO_KEEPALIVE
    SO_LINGER
    SO_NKE
    SO_NOSIGPIPE
    SO_NO_CHECK
    SO_NREAD
    SO_OOBINLINE
    SO_PASSCRED
    SO_PEERCRED
    SO_PEERNAME
    SO_PRIORITY
    SO_RCVBUF
    SO_RCVLOWAT
    SO_RCVTIMEO
    SO_REUSEADDR
    SO_REUSEPORT
    SO_SECURITY_AUTHENTICATION
    SO_SECURITY_ENCRYPTION_NETWORK
    SO_SECURITY_ENCRYPTION_TRANSPORT
    SO_SNDBUF
    SO_SNDLOWAT
    SO_SNDTIMEO
    SO_TIMESTAMP
    SO_TYPE
    SO_USELOOPBACK
    SO_WANTMORE
    SO_WANTOOBFLAG

    TCP_MAXSEG
    TCP_NODELAY
  ]

  process_constants = %w{
    WNOHANG
    WUNTRACED
    PRIO_PROCESS
    PRIO_PGRP
    PRIO_USER
    RLIMIT_CPU
    RLIMIT_FSIZE
    RLIMIT_DATA
    RLIMIT_STACK
    RLIMIT_CORE
    RLIMIT_RSS
    RLIMIT_NPROC
    RLIMIT_NOFILE
    RLIMIT_MEMLOCK
    RLIMIT_AS
    RLIMIT_SBSIZE
  }

  long_process_constants = %w{
    RLIM_INFINITY
    RLIM_SAVED_MAX
    RLIM_SAVED_CUR
  }

  # The constants come from MRI's signal.c. This means that some of them might
  # be missing.
  signal_constants = %w{
    SIGHUP
    SIGINT
    SIGQUIT
    SIGILL
    SIGTRAP
    SIGIOT
    SIGABRT
    SIGEMT
    SIGFPE
    SIGKILL
    SIGBUS
    SIGSEGV
    SIGSYS
    SIGPIPE
    SIGALRM
    SIGTERM
    SIGURG
    SIGSTOP
    SIGTSTP
    SIGCONT
    SIGCHLD
    SIGCLD
    SIGCHLD
    SIGTTIN
    SIGTTOU
    SIGIO
    SIGXCPU
    SIGXFSZ
    SIGVTALRM
    SIGPROF
    SIGWINCH
    SIGUSR1
    SIGUSR2
    SIGLOST
    SIGMSG
    SIGPWR
    SIGPOLL
    SIGDANGER
    SIGMIGRATE
    SIGPRE
    SIGGRANT
    SIGRETRACT
    SIGSOUND
    SIGINFO
  }

  zlib_constants = %w[ZLIB_VERSION]

  cg = ConstGenerator.new
  cg.include "stdio.h"
  cg.include "fcntl.h"
  cg.include "sys/types.h"
  cg.include "sys/socket.h"
  cg.include "netdb.h"
  cg.include "sys/stat.h"
  cg.include "sys/resource.h"
  cg.include "netinet/tcp.h"
  cg.include "signal.h"
  cg.include "netinet/in.h"
  cg.include "syslog.h"
  cg.include "zlib.h"

  file_constants.each { |c| cg.const c }
  io_constants.each { |c| cg.const c }
  fcntl_constants.each {|c| cg.const c }
  socket_constants.each { |c| cg.const c }
  process_constants.each { |c| cg.const c }
  long_process_constants.each { |c|
    cg.const c, "%llu", "(unsigned long long)"
  }
  signal_constants.each { |c| cg.const c }
  fcntl_constants.each { |c| cg.const c }
  syslog_constants.each { |c| cg.const c }
  zlib_constants.each { |c| cg.const c, "%s", "(char *)" }

  cg.calculate

  puts "Generating #{task.name}..."

  File.open task.name, "w" do |f|
    f.puts addrinfo.generate_config('addrinfo')
    f.puts dirent.generate_config('dirent')
    f.puts timeval.generate_config('timeval')
    f.puts sockaddr_in.generate_config('sockaddr_in')
    f.puts sockaddr_un.generate_config('sockaddr_un') if sockaddr_un.found?
    f.puts servent.generate_config('servent')
    f.puts stat.generate_config('stat')
    f.puts rlimit.generate_config('rlimit')
    f.puts evp_md.generate_config('evp_md')
    f.puts evp_md_ctx.generate_config('evp_md_ctx')

    file_constants.each do | name |
      const = cg.constants[name]
      f.puts "rbx.platform.file.#{name} = #{const.converted_value}"
    end

    io_constants.each do |name|
      const = cg.constants[name]
      f.puts "rbx.platform.io.#{name} = #{const.converted_value}"
    end

    fcntl_constants.each do |name|
      const = cg.constants[name]
      next if const.converted_value.nil?
      f.puts "rbx.platform.fcntl.#{name} = #{const.converted_value}"
    end

    socket_constants.each do |name|
      const = cg.constants[name]
      next if const.converted_value.nil?
      f.puts "rbx.platform.socket.#{name} = #{const.converted_value}"
    end

    (process_constants + long_process_constants).each do |name|
      const = cg.constants[name]
      next if const.converted_value.nil?
      f.puts "rbx.platform.process.#{name} = #{const.converted_value}"
    end

    signal_constants.each do |name|
      const = cg.constants[name]
      next if const.converted_value.nil?
      f.puts "rbx.platform.signal.#{name} = #{const.converted_value}"
    end

    syslog_constants.each do |name|
      const = cg.constants[name]
      f.puts "rbx.platform.syslog.#{name} = #{const.converted_value}"
    end

    zlib_constants.each do |name|
      const = cg.constants[name]
      f.puts "rbx.platform.zlib.#{name} = #{const.converted_value}"
    end

    f.puts TypesGenerator.generate
  end
end
