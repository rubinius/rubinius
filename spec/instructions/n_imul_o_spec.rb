require File.expand_path("../spec_helper", __FILE__)

describe "Instruction n_imul_o" do
  before do
    @spec = InstructionSpec.new :n_imul_o do |g|
      g.required_args = 2
      g.total_args = 2
      g.local_count = 2

      r0 = g.new_register
      r1 = g.new_register

      g.r_load_local r0, 0
      g.r_load_local r1, 1

      g.n_imul_o r0, r0, r1

      g.r_store_stack r0
      g.ret
    end
  end

  it "multiplies two Fixnums" do
    value = @spec.run 10, 20
    value.should == 200
    value.should be_an_instance_of(Fixnum)
  end

  it "multiplies two Fixnums that overflow" do
    value = @spec.run 0x3fff_ffff_ffff_ffff, 3
    value.should == 0xbfff_ffff_ffff_fffd
    value.should be_an_instance_of(Bignum)
  end
end
