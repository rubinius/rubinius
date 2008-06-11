##
# Platform-specific behavior for Fixnum.

class Platform::Fixnum

  ##
  # The maximum value that a Fixnum can hold.
  #--
  # TODO This is a method needs to have a consistent interface see e.g.
  # Platform::Float

  def self.MAX
    # (2**29) - 1
    0x1fff_ffff
  end

  ##
  # The minimum value a Fixnum can hold.

  def self.MIN
    ~0x1fff_ffff
  end
end
