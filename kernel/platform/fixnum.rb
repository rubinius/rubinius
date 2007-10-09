class Platform::Fixnum
  # this is a method to have a consistent interface
  # see e.g. Platform::Float
  def self.MAX
    # (2**29) - 1
    0x1fff_ffff
  end
  
  def self.MIN
    ~0x1fff_ffff
  end
end
