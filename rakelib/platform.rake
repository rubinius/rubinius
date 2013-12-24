require File.expand_path('../../library/ffi/generators', __FILE__)

directory "runtime"

deps = %w[Rakefile rakelib/platform.rake runtime] + Dir['lib/ffi/*rb']

file 'runtime/platform.conf' => deps do |task|
  puts "GEN runtime/platform.conf"

  File.open task.name, "wb" do |f|
    Rubinius::FFI::Generators::Structures.new 'addrinfo' do |s|
      if BUILD_CONFIG[:windows]
        s.include "ws2tcpip.h"
      else
        s.include 'sys/socket.h'
        s.include 'netdb.h'
      end
      s.name 'struct addrinfo'
      s.field :ai_flags, :int
      s.field :ai_family, :int
      s.field :ai_socktype, :int
      s.field :ai_protocol, :int
      s.field :ai_addrlen, :int
      s.field :ai_addr, :pointer
      s.field :ai_canonname, :string
      s.field :ai_next, :pointer
    end.write_config(f)

    Rubinius::FFI::Generators::Structures.new 'dirent' do |s|
      s.include "sys/types.h"
      s.include "dirent.h"
      s.name 'struct dirent'
      s.field :d_ino, :ino_t
      s.field :d_reclen, :ushort
      s.field :d_name, :char_array
    end.write_config(f)

    Rubinius::FFI::Generators::Structures.new 'timeval' do |s|
      s.include "sys/time.h"
      s.name 'struct timeval'
      s.field :tv_sec, :time_t
      s.field :tv_usec, :suseconds_t
    end.write_config(f)

    Rubinius::FFI::Generators::Structures.new 'sockaddr_in' do |s|
      if BUILD_CONFIG[:windows]
        s.include "ws2tcpip.h"
      else
        s.include "netinet/in.h"
        s.include "sys/socket.h"
      end
      s.include "fcntl.h"
      s.include "sys/stat.h"
      s.name 'struct sockaddr_in'
      s.field :sin_family, :sa_family_t
      s.field :sin_port, :ushort
      s.field :sin_addr
      s.field :sin_zero, :char_array
    end.write_config(f)

    Rubinius::FFI::Generators::Structures.new 'sockaddr_in6' do |s|
      if BUILD_CONFIG[:windows]
        s.include "ws2tcpip.h"
      else
        s.include "netinet/in.h"
        s.include "sys/socket.h"
      end
      s.include "fcntl.h"
      s.include "sys/stat.h"
      s.name 'struct sockaddr_in6'
      s.field :sin6_family, :sa_family_t
      s.field :sin6_port, :ushort
      s.field :sin6_flowinfo
      s.field :sin6_addr, :char_array
      s.field :sin6_scope_id
    end.write_config(f)

    unless BUILD_CONFIG[:windows]
      sockaddr_un = Rubinius::FFI::Generators::Structures.new 'sockaddr_un' do |s|
        s.include "sys/un.h"
        s.name 'struct sockaddr_un'
        s.field :sun_family, :sa_family_t
        s.field :sun_path, :char_array
      end

      sockaddr_un.write_config f if sockaddr_un.found?
    end

    Rubinius::FFI::Generators::Structures.new 'linger' do |s|
      if BUILD_CONFIG[:windows]
        s.include "winsock2.h"
      else
        s.include "sys/socket.h"
      end
      s.name "struct linger"
        s.field :l_onoff
        s.field :l_linger
    end.write_config(f)

    Rubinius::FFI::Generators::Structures.new 'servent' do |s|
      if BUILD_CONFIG[:windows]
        s.include "winsock2.h"
      else
        s.include "netdb.h"
      end
      s.name 'struct servent'
      s.field :s_name, :pointer
      s.field :s_aliases, :pointer
      s.field :s_port, :int
      s.field :s_proto, :pointer
    end.write_config(f)

    Rubinius::FFI::Generators::Structures.new 'stat' do |s|
      unless BUILD_CONFIG[:windows]
        s.include "sys/types.h"
        s.include "unistd.h"
        s.include "time.h"
      end
      s.include "sys/stat.h"
      s.name 'struct stat'
      s.field :st_dev, :dev_t
      s.field :st_ino, :ino_t
      s.field :st_mode, :mode_t
      if BUILD_CONFIG[:windows]
        s.field :st_nlink, :short
        s.field :st_uid, :short
        s.field :st_gid, :short
      else
        s.field :st_nlink, :nlink_t
        s.field :st_uid, :uid_t
        s.field :st_gid, :gid_t
      end
      s.field :st_rdev, :dev_t
      s.field :st_size, :off_t
      unless BUILD_CONFIG[:windows]
        s.field :st_blksize
        s.field :st_blocks
      end
      s.field :st_atime, :time_t
      s.field :st_mtime, :time_t
      s.field :st_ctime, :time_t
    end.write_config(f)

    unless BUILD_CONFIG[:windows]
      Rubinius::FFI::Generators::Structures.new 'rlimit' do |s|
        s.include "sys/types.h"
        s.include "sys/time.h"
        s.include "sys/resource.h"
        s.name 'struct rlimit'
        s.field :rlim_cur, :rlim_t
        s.field :rlim_max, :rlim_t
      end.write_config(f)
    end

    # FIXME these constants don't have standard names.  LOCK_SH == Linux,
    # O_SHLOCK on Bsd/Darwin, etc.  Binary doesn't exist at all in many non-Unix
    # variants.  This should come out of something like config.h

    # fixme_constants = %w{
    #   LOCK_SH
    #   LOCK_EX
    #   LOCK_NB
    #   LOCK_UN
    #   BINARY
    # }

    Rubinius::FFI::Generators::Constants.new 'rbx.platform.file' do |cg|
      cg.include 'stdio.h'
      cg.include 'fcntl.h'
      cg.include 'sys/stat.h'

      file_constants = %w[
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
        S_ISVTX
      ]

      file_constants.each { |c| cg.const c }
    end.write_constants(f)

    Rubinius::FFI::Generators::Constants.new 'rbx.platform.io' do |cg|
      cg.include 'stdio.h'

      io_constants = %w[
        SEEK_SET
        SEEK_CUR
        SEEK_END
      ]

      io_constants.each { |c| cg.const c }
    end.write_constants(f)

    # Only constants needed by core are added here
    Rubinius::FFI::Generators::Constants.new 'rbx.platform.fcntl' do |cg|
      cg.include 'fcntl.h'

      fcntl_constants = %w[
        F_GETFL
        F_SETFL
        O_ACCMODE
        F_GETFD
        F_SETFD
        FD_CLOEXEC
      ]

      fcntl_constants.each { |c| cg.const c }
    end.write_constants(f)

    Rubinius::FFI::Generators::Constants.new 'rbx.platform.socket' do |cg|
      cg.include 'sys/types.h'
      if BUILD_CONFIG[:windows]
        cg.include "winsock2.h"
      else
        cg.include 'sys/socket.h'
        cg.include 'netdb.h'
        cg.include 'netinet/tcp.h'
        cg.include 'netinet/in.h'
      end

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
        AF_PACKET
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
        AI_NUMERICSERV
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
        EAI_OVERFLOW
        EAI_PROTOCOL
        EAI_SERVICE
        EAI_SOCKTYPE
        EAI_SYSTEM

        IF_NAMESIZE

        IFF_802_1Q_VLAN
        IFF_ALLMULTI
        IFF_ALTPHYS
        IFF_AUTOMEDIA
        IFF_BONDING
        IFF_BRIDGE_PORT
        IFF_BROADCAST
        IFF_CANTCHANGE
        IFF_CANTCONFIG
        IFF_DEBUG
        IFF_DISABLE_NETPOLL
        IFF_DONT_BRIDGE
        IFF_DORMANT
        IFF_DRV_OACTIVE
        IFF_DRV_RUNNING
        IFF_DYING
        IFF_DYNAMIC
        IFF_EBRIDGE
        IFF_ECHO
        IFF_ISATAP
        IFF_LINK0
        IFF_LINK1
        IFF_LINK2
        IFF_LIVE_ADDR_CHANGE
        IFF_LOOPBACK
        IFF_LOWER_UP
        IFF_MACVLAN_PORT
        IFF_MASTER
        IFF_MASTER_8023AD
        IFF_MASTER_ALB
        IFF_MASTER_ARPMON
        IFF_MONITOR
        IFF_MULTICAST
        IFF_NOARP
        IFF_NOTRAILERS
        IFF_OACTIVE
        IFF_OVS_DATAPATH
        IFF_POINTOPOINT
        IFF_PORTSEL
        IFF_PPROMISC
        IFF_PROMISC
        IFF_RENAMING
        IFF_ROUTE
        IFF_RUNNING
        IFF_SIMPLEX
        IFF_SLAVE
        IFF_SLAVE_INACTIVE
        IFF_SLAVE_NEEDARP
        IFF_SMART
        IFF_STATICARP
        IFF_SUPP_NOFCS
        IFF_TEAM_PORT
        IFF_TX_SKB_SHARING
        IFF_UNICAST_FLT
        IFF_UP
        IFF_VOLATILE
        IFF_WAN_HDLC
        IFF_XMIT_DST_RELEASE

        IFNAMSIZ

        INADDR_ALLHOSTS_GROUP
        INADDR_ANY
        INADDR_BROADCAST
        INADDR_LOOPBACK
        INADDR_MAX_LOCAL_GROUP
        INADDR_NONE
        INADDR_UNSPEC_GROUP

        INET6_ADDRSTRLEN
        INET_ADDRSTRLEN

        IP_ADD_MEMBERSHIP
        IP_ADD_SOURCE_MEMBERSHIP
        IP_BLOCK_SOURCE
        IP_DEFAULT_MULTICAST_LOOP
        IP_DEFAULT_MULTICAST_TTL
        IP_DONTFRAG
        IP_DROP_MEMBERSHIP
        IP_DROP_SOURCE_MEMBERSHIP
        IP_FREEBIND
        IP_HDRINCL
        IP_IPSEC_POLICY
        IP_MAX_MEMBERSHIPS
        IP_MINTTL
        IP_MSFILTER
        IP_MTU
        IP_MTU_DISCOVER
        IP_MULTICAST_IF
        IP_MULTICAST_LOOP
        IP_MULTICAST_TTL
        IP_ONESBCAST
        IP_OPTIONS
        IP_PASSSEC
        IP_PKTINFO
        IP_PKTOPTIONS
        IP_PMTUDISC_DO
        IP_PMTUDISC_DONT
        IP_PMTUDISC_WANT
        IP_PORTRANGE
        IP_RECVDSTADDR
        IP_RECVERR
        IP_RECVIF
        IP_RECVOPTS
        IP_RECVRETOPTS
        IP_RECVSLLA
        IP_RECVTOS
        IP_RECVTTL
        IP_RETOPTS
        IP_ROUTER_ALERT
        IP_SENDSRCADDR
        IP_TOS
        IP_TTL
        IP_UNBLOCK_SOURCE
        IP_XFRM_POLICY

        IPPORT_RESERVED
        IPPORT_USERRESERVED

        IPPROTO_AH
        IPPROTO_BIP
        IPPROTO_DSTOPTS
        IPPROTO_EGP
        IPPROTO_EON
        IPPROTO_ESP
        IPPROTO_FRAGMENT
        IPPROTO_GGP
        IPPROTO_HELLO
        IPPROTO_HOPOPTS
        IPPROTO_ICMP
        IPPROTO_ICMPV6
        IPPROTO_IDP
        IPPROTO_IGMP
        IPPROTO_IP
        IPPROTO_IPV6
        IPPROTO_MAX
        IPPROTO_ND
        IPPROTO_NONE
        IPPROTO_PUP
        IPPROTO_RAW
        IPPROTO_ROUTING
        IPPROTO_TCP
        IPPROTO_TP
        IPPROTO_UDP
        IPPROTO_XTP

        IPV6_CHECKSUM
        IPV6_DONTFRAG
        IPV6_DSTOPTS
        IPV6_HOPLIMIT
        IPV6_HOPOPTS
        IPV6_JOIN_GROUP
        IPV6_LEAVE_GROUP
        IPV6_MULTICAST_HOPS
        IPV6_MULTICAST_IF
        IPV6_MULTICAST_LOOP
        IPV6_NEXTHOP
        IPV6_PATHMTU
        IPV6_PKTINFO
        IPV6_RECVDSTOPTS
        IPV6_RECVHOPLIMIT
        IPV6_RECVHOPOPTS
        IPV6_RECVPATHMTU
        IPV6_RECVPKTINFO
        IPV6_RECVRTHDR
        IPV6_RECVTCLASS
        IPV6_RTHDR
        IPV6_RTHDR_TYPE_0
        IPV6_RTHDRDSTOPTS
        IPV6_TCLASS
        IPV6_UNICAST_HOPS
        IPV6_USE_MIN_MTU
        IPV6_V6ONLY

        IPX_TYPE

        LOCAL_CONNWAIT
        LOCAL_CREDS
        LOCAL_PEERCRED

        MCAST_BLOCK_SOURCE
        MCAST_EXCLUDE
        MCAST_INCLUDE
        MCAST_JOIN_GROUP
        MCAST_JOIN_SOURCE_GROUP
        MCAST_LEAVE_GROUP
        MCAST_LEAVE_SOURCE_GROUP
        MCAST_MSFILTER
        MCAST_UNBLOCK_SOURCE

        MSG_COMPAT
        MSG_CONFIRM
        MSG_CTRUNC
        MSG_DONTROUTE
        MSG_DONTWAIT
        MSG_EOF
        MSG_EOR
        MSG_ERRQUEUE
        MSG_FASTOPEN
        MSG_FIN
        MSG_FLUSH
        MSG_HAVEMORE
        MSG_HOLD
        MSG_MORE
        MSG_NOSIGNAL
        MSG_OOB
        MSG_PEEK
        MSG_PROXY
        MSG_RCVMORE
        MSG_RST
        MSG_SEND
        MSG_SYN
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
        PF_ATM
        PF_AX25
        PF_CCITT
        PF_CHAOS
        PF_CNT
        PF_COIP
        PF_DATAKIT
        PF_DEC
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
        PF_PACKET
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

        SCM_BINTIME
        SCM_CREDENTIALS
        SCM_CREDS
        SCM_RIGHTS
        SCM_TIMESTAMP
        SCM_TIMESTAMPNS
        SCM_UCRED

        SHUT_RD
        SHUT_RDWR
        SHUT_WR

        SO_ACCEPTCONN
        SO_ACCEPTFILTER
        SO_ALLZONES
        SO_ATTACH_FILTER
        SO_BINDTODEVICE
        SO_BINTIME
        SO_BROADCAST
        SO_DEBUG
        SO_DETACH_FILTER
        SO_DONTROUTE
        SO_DONTTRUNC
        SO_ERROR
        SO_KEEPALIVE
        SO_LINGER
        SO_MAC_EXEMPT
        SO_NKE
        SO_NO_CHECK
        SO_NOSIGPIPE
        SO_NREAD
        SO_OOBINLINE
        SO_PASSCRED
        SO_PEERCRED
        SO_PEERNAME
        SO_PRIORITY
        SO_RCVBUF
        SO_RCVLOWAT
        SO_RCVTIMEO
        SO_RECVUCRED
        SO_REUSEADDR
        SO_REUSEPORT
        SO_SECURITY_AUTHENTICATION
        SO_SECURITY_ENCRYPTION_NETWORK
        SO_SECURITY_ENCRYPTION_TRANSPORT
        SO_SNDBUF
        SO_SNDLOWAT
        SO_SNDTIMEO
        SO_TIMESTAMP
        SO_TIMESTAMPNS
        SO_TYPE
        SO_USELOOPBACK
        SO_WANTMORE
        SO_WANTOOBFLAG

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

        SOMAXCONN

        SOPRI_BACKGROUND
        SOPRI_INTERACTIVE
        SOPRI_NORMAL

        TCP_CORK
        TCP_DEFER_ACCEPT
        TCP_FASTOPEN
        TCP_INFO
        TCP_KEEPCNT
        TCP_KEEPIDLE
        TCP_KEEPINTVL
        TCP_LINGER2
        TCP_MAXSEG
        TCP_MD5SIG
        TCP_NODELAY
        TCP_NOOPT
        TCP_NOPUSH
        TCP_QUICKACK
        TCP_SYNCNT
        TCP_WINDOW_CLAMP

        UDP_CORK
      ]

      socket_constants.each { |c| cg.const c }
    end.write_constants(f)

    unless BUILD_CONFIG[:windows]
      Rubinius::FFI::Generators::Constants.new 'rbx.platform.process' do |cg|
        cg.include 'sys/wait.h'
        cg.include 'sys/resource.h'
        cg.include 'stdlib.h'

        process_constants = %w{
          EXIT_SUCCESS
          EXIT_FAILURE

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
          RLIMIT_RTPRIO
          RLIMIT_RTTIME
          RLIMIT_SIGPENDING
          RLIMIT_MSGQUEUE
          RLIMIT_NICE
        }

        process_constants.each { |c| cg.const c }

        long_process_constants = %w[
          RLIM_INFINITY
          RLIM_SAVED_MAX
          RLIM_SAVED_CUR
        ]

        long_process_constants.each { |c|
          cg.const c, "%llu", "(unsigned long long)"
        }
      end.write_constants(f)
    end

    # The constants come from MRI's signal.c. This means that some of them might
    # be missing.

    Rubinius::FFI::Generators::Constants.new 'rbx.platform.signal' do |cg|
      cg.include 'signal.h'
      unless BUILD_CONFIG[:windows]
        cg.include 'sys/signal.h'
      end

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

      signal_constants.each { |c| cg.const c }
    end.write_constants(f)

    Rubinius::FFI::Generators::Constants.new 'rbx.platform.zlib' do |cg|
      cg.include_dir 'vendor/zlib' if BUILD_CONFIG[:vendor_zlib]
      cg.include 'zlib.h'

      zlib_constants = %w[ZLIB_VERSION]

      zlib_constants.each { |c| cg.const c, "%s", "(char *)" }
    end.write_constants(f)

    unless BUILD_CONFIG[:windows]
      Rubinius::FFI::Generators::Constants.new 'rbx.platform.dlopen' do |cg|
        cg.include 'dlfcn.h'

        dlopen_constants = %w[
          RTLD_LAZY
          RTLD_NOW
          RTLD_LOCAL
          RTLD_GLOBAL
        ]

        dlopen_constants.each { |c| cg.const c }
      end.write_constants(f)
    end

    f.puts Rubinius::FFI::Generators::Types.new.generate
  end
end
