require File.dirname(__FILE__) + "/spec_helper"

describe Compiler do
  it "compiles defined? instance_exec" do
    gen [:defined, [:vcall, :instance_exec]] do |g|
      responded = g.new_label
      done = g.new_label
      g.push :true
      g.push_literal :instance_exec
      g.push :self
      g.send :__respond_to_eh__, 2
      g.git responded
      g.push_nil
      g.goto done
      responded.set!
      g.push_literal "method"
      done.set!
    end
  end
  it "compiles defined? instance_exec()" do
    gen [:defined, [:fcall, :instance_exec]] do |g|
      responded = g.new_label
      done = g.new_label
      g.push :true
      g.push_literal :instance_exec
      g.push :self
      g.send :__respond_to_eh__, 2
      g.git responded
      g.push_nil
      g.goto done
      responded.set!
      g.push_literal "method"
      done.set!
    end
  end
end
