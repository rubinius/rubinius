require 'digest'
require 'ext/digest/sha1/sha1'

class Digest::SHA1 < Digest::Instance

  class Context < FFI::Struct

    def self.size # HACK FFI::Struct can't do arrays
      4 * 5 + # state
      4 * 2 + # count
      64 + # buffer
      4 # padding to be extra-safe
    end

  end

  attach_function 'rbx_Digest_SHA1_Init', :sha1_init, [:pointer], :void
  attach_function 'rbx_Digest_SHA1_Update', :sha1_update,
                  [:pointer, :string, :int], :void
  attach_function 'rbx_Digest_SHA1_Finish', :sha1_finish, [:pointer, :string],
                  :void

  def initialize
    reset
  end

  def digest_length
    20
  end

  def block_length
    64
  end

  def finish
    digest = ' ' * digest_length
    self.class.sha1_finish @context.pointer, digest
    digest
  end

  def reset
    @context.free if @context
    @context = Context.new
    self.class.sha1_init @context.pointer
  end

  def update(string)
    self.class.sha1_update @context.pointer, string, string.length
    self
  end

  alias << update

end

