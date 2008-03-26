require File.dirname(__FILE__) + "/spec_helper"

describe Compiler do
  it "compiles defined? instance_exec" do
    gen [:defined, [:vcall, :instance_exec]] do |g|
      g.push :self
      g.push_literal :instance_exec
      g.send :"respond_to?", 1
    end
  end
  it "compiles defined? instance_exec()" do
    gen [:defined, [:fcall, :instance_exec]] do |g|
      g.push :self
      g.push_literal :instance_exec
      g.send :"respond_to?", 1
    end
  end
end
