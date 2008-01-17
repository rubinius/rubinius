require 'digest'
require 'ext/digest/md5/md5'

class Digest::MD5 < Digest::Instance
  self.extend(Digest::Class)

  #FFI
  attach_function "rbx_Digest_MD5_Alloc",  :md5_alloc, [], :pointer
  attach_function "rbx_Digest_MD5_StructSize", :md5_struct_size, [], :int

  attach_function "rbx_Digest_MD5_Init",   :md5_init, [:pointer], :void
  attach_function "rbx_Digest_MD5_Update", :md5_update, [:pointer, :string, :int], :void
  attach_function "rbx_Digest_MD5_Finish", :md5_finish, [:pointer, :string], :void

  def initialize
    @context = self.class.md5_alloc
    self.class.md5_init @context
  end

  def initialize_copy(other)
    size = self.class.md5_struct_size
    @context = MemoryPointer.new(size)
    Platform::POSIX.memcpy(@context, other.context, size)
  end

  def update(string)
    self.class.md5_update @context, string, string.length
    self
  end
  alias :<< :update

  def digest_length
    16
  end

  def block_length
    64
  end

  def reset
    @context = self.class.md5_alloc
    self.class.md5_init @context
  end

  def finish
    digest = ' ' * digest_length
    self.class.md5_finish @context, digest
    digest
  end

  protected
  attr_accessor :context

end
