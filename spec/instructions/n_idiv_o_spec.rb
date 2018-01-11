require File.expand_path("../spec_helper", __FILE__)

describe "Instruction n_idiv_o" do
  before do
    @spec = InstructionSpec.new :n_idiv_o do |g|
      g.required_args = 2
      g.total_args = 2
      g.local_count = 2

      r0 = g.new_register
      r1 = g.new_register

      g.r_load_local r0, 0
      g.r_load_local r1, 1

      g.n_idiv_o r0, r0, r1

      g.r_store_stack r0
      g.ret
    end
  end

  it "divides two Fixnums" do
    value = @spec.run 87, 2
    value.should == 43
    value.should be_an_instance_of(Fixnum)
  end

  # TODO: Fix Fixnum min bug
  # it "divides two Fixnums that overflow" do
  #   value = @spec.run(-0x4000_0000_0000_0000, -1)
  #   value.should == 0x4000_0000_0000_0000
  #   value.should be_an_instance_of(Bignum)
  # end
end
