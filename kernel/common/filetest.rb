# -*- encoding: us-ascii -*-

module FileTest
  def blockdev?(path)
    File.blockdev? path
  end

  def chardev?(path)
    File.chardev? path
  end

  def directory?(path)
    File.directory? path
  end

  def executable?(path)
    File.executable? path
  end

  def executable_real?(path)
    File.executable_real? path
  end

  def exist?(path)
    File.exist? path
  end
  alias_method :exists?, :exist?

  def file?(path)
    File.file? path
  end

  def grpowned?(path)
    File.grpowned? path
  end

  def identical?(a, b)
    File.identical? a, b
  end

  def owned?(path)
    File.owned? path
  end

  def pipe?(path)
    File.pipe? path
  end

  def readable?(path)
    File.readable? path
  end

  def readable_real?(path)
    File.readable_real? path
  end

  def setgid?(path)
    File.setgid? path
  end

  def setuid?(path)
    File.setuid? path
  end

  def size(path)
    File.size path
  end

  def size?(path)
    File.size? path
  end

  def socket?(path)
    File.socket? path
  end

  def sticky?(path)
    File.sticky? path
  end

  def symlink?(path)
    File.symlink? path
  end

  def writable?(path)
    File.writable? path
  end

  def writable_real?(path)
    File.writable_real? path
  end

  def zero?(path)
    File.zero? path
  end
end
