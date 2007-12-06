require 'digest'
require 'ext/digest/md5/md5'

class Digest::MD5

  attach_function nil, "rbx_Digest_MD5_Alloc", :md5_alloc, [], :pointer
  attach_function nil, "rbx_Digest_MD5_Init", :md5_init, [:pointer], :void
  attach_function nil, "rbx_Digest_MD5_Update", :md5_update,
                  [:pointer, :string, :int], :void
  attach_function nil, "rbx_Digest_MD5_Finish", :md5_finish,
                  [:pointer, :string], :void

  def initialize
    @context = self.class.md5_alloc
    self.class.md5_init @context
  end

  def digest
    finish
  end

  def finish
    digest = ' ' * 16
    self.class.md5_finish @context, digest
    digest
  end

  def update(data)
    self.class.md5_update @context, data, data.length
    self
  end

end

