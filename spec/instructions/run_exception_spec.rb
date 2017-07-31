require File.expand_path("../spec_helper", __FILE__)

describe "Instruction run_exception" do
  before do
    @spec = InstructionSpec.new :run_exception do |g|
      g.push_self
      g.send_method :m
      g.pop
      g.push_literal :a
      g.ret
      g.run_exception
    end

    def @spec.m
      raise RuntimeError
    end
  end

  it "propagates an un-rescued exception" do
    lambda { @spec.run }.should raise_error(RuntimeError)
  end
end

