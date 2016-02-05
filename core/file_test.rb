module FileTest
  def blockdev?(path)
    File.blockdev? path
  end
  module_function :blockdev?

  def chardev?(path)
    File.chardev? path
  end
  module_function :chardev?

  def directory?(path)
    File.directory? path
  end
  module_function :directory?

  def executable?(path)
    File.executable? path
  end
  module_function :executable?

  def executable_real?(path)
    File.executable_real? path
  end
  module_function :executable_real?

  def exist?(path)
    File.exist? path
  end
  module_function :exist?

  alias_method :exists?, :exist?
  module_function :exists?

  def file?(path)
    File.file? path
  end
  module_function :file?

  def grpowned?(path)
    File.grpowned? path
  end
  module_function :grpowned?

  def identical?(a, b)
    File.identical? a, b
  end
  module_function :identical?

  def owned?(path)
    File.owned? path
  end
  module_function :owned?

  def pipe?(path)
    File.pipe? path
  end
  module_function :pipe?

  def readable?(path)
    File.readable? path
  end
  module_function :readable?

  def readable_real?(path)
    File.readable_real? path
  end
  module_function :readable_real?

  def setgid?(path)
    File.setgid? path
  end
  module_function :setgid?

  def setuid?(path)
    File.setuid? path
  end
  module_function :setuid?

  def size(path)
    File.size path
  end
  module_function :size

  def size?(path)
    File.size? path
  end
  module_function :size?

  def socket?(path)
    File.socket? path
  end
  module_function :socket?

  def sticky?(path)
    File.sticky? path
  end
  module_function :sticky?

  def symlink?(path)
    File.symlink? path
  end
  module_function :symlink?

  def writable?(path)
    File.writable? path
  end
  module_function :writable?

  def writable_real?(path)
    File.writable_real? path
  end
  module_function :writable_real?

  def zero?(path)
    File.zero? path
  end
  module_function :zero?
end
