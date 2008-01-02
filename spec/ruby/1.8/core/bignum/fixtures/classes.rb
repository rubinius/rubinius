module BignumHelper
  # we don't care about the exact number, but we want to
  # ensure that a Bignum is being used, so we set this to
  # a big enough number that it is a Bignum on any implementation.
  def self.sbm(plus=0)
    0x8000_0000_0000_0000 + plus
  end
end
