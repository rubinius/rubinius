require File.dirname(__FILE__) + '/../spec_helper'

# %, &, *, **, **, +, -, -@, /, <<, <=>, ==, >>, [], ^, abs, coerce,
# div, divmod, eql?, hash, modulo, power!, quo, quo, rdiv, remainder,
# rpower, size, to_f, to_s, |, ~

context "Bignum instance method" do
  # the smallest bignum in MRI is a different value
  # than the smallest bignum in Rubinius, however,
  # rubinius(smallest_bignum) <= MRI(smallest_bignum)
  # so these specs are written such that they should
  # pass on either target.
  setup do
    @src = <<-CODE
    module B
      def self.sbm(plus=0)
        0x3fffffff + plus
      end
    end
    CODE
  end

  specify "% should return self modulo other" do
    example(@src) do
      @a = B.sbm(2_222)
      p [@a % 5, @a % 2.22, @a % B.sbm]
    end.should == '[0, 1.63999990549154, 2222]'
  end
  
end
