require File.dirname(__FILE__) + '/../../../spec_helper'

describe "IO#gets" do
  after :each do
    File.delete IOSpecs.gets_output if File.exists?(IOSpecs.gets_output)
  end

  it "returns <limit bytes if IO size is smaller than limit" do
    tmp = tmp "gets_specs"
    f = File.open(tmp, "w") { |f| f.print("A\n") }
    File.open(tmp) do |f|
      f.gets("", 10).should == "A\n"
    end
    File.unlink tmp
  end

  it "returns =limit bytes if IO size is same size as limit" do
    tmp = tmp "gets_specs"
    f = File.open(tmp, "w") { |f| f.print("ABC\n") }
    File.open(tmp) do |f|
      f.gets("", 4).should == "ABC\n"
    end
    File.unlink tmp
  end

  it "returns limit bytes if IO size is greater size than limit" do
    tmp = tmp "gets_specs"
    f = File.open(tmp, "w") { |f| f.print("ABCD\n") }
    File.open(tmp) do |f|
      f.gets("", 2).should == "AB"
    end
    File.unlink tmp
  end
end
