require File.dirname(__FILE__) + "/spec_helper"

describe Compiler do
  it "compiles defined? instance_exec" do
    ruby = <<-EOC
      defined? instance_exec
    EOC

    sexp = s(:defined, s(:vcall, :instance_exec))

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      responded = g.new_label
      done      = g.new_label

      g.push :self
      g.push_literal :instance_exec
      g.push :true
      g.send :__respond_to_eh__, 2

      g.git responded

      g.push :nil
      g.goto done
      responded.set!
      g.push_literal "method"
      done.set!
    end
  end
  it "compiles defined? instance_exec()" do
    ruby = <<-EOC
      defined? instance_exec()
    EOC

    sexp = s(:defined, s(:fcall, :instance_exec))

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      responded = g.new_label
      done      = g.new_label

      g.push :self
      g.push_literal :instance_exec
      g.push :true
      g.send :__respond_to_eh__, 2

      g.git responded

      g.push :nil
      g.goto done
      responded.set!
      g.push_literal "method"
      done.set!
    end
  end
end
