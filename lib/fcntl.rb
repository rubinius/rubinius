module Fcntl
  Constants = 
    %w[ F_DUPFD F_GETFD F_GETLK F_SETFD F_GETFL F_SETFL F_SETLK F_SETLKW 
        FD_CLOEXEC F_RDLCK F_UNLCK F_WRLCK O_CREAT O_EXCL O_NOCTTY O_TRUNC 
        O_APPEND O_NONBLOCK O_NDELAY O_RDONLY O_RDWR O_WRONLY O_ACCMODE ]

  Constants.each do |constant|
    value = Rubinius::RUBY_CONFIG["rbx.platform.fcntl.#{constant}"]
    const_set(constant, value) if value
  end

end
