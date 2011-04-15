require File.expand_path('../../../spec_helper', __FILE__)

describe "Proc#to_s" do

  ruby_version_is ""..."1.9" do
    it "returns a description of self" do
      Proc.new { "hello" }.to_s.should =~ /^#<Proc:(.*?)@(.*)\/to_s_spec\.rb:7>$/
      lambda { "hello" }.to_s.should =~ /^#<Proc:(.*?)@(.*)\/to_s_spec\.rb:8>$/
      proc { "hello" }.to_s.should =~ /^#<Proc:(.*?)@(.*)\/to_s_spec\.rb:9>$/
    end
  end

  ruby_version_is "1.9" do
    it "returns a description of self" do
      Proc.new { "hello" }.to_s.should =~ /^#<Proc:(.*?)@(.*)\/to_s_spec\.rb:15>$/
      lambda { "hello" }.to_s.should =~ /^#<Proc:(.*?)@(.*)\/to_s_spec\.rb:16 \(lambda\)>$/
      proc { "hello" }.to_s.should =~ /^#<Proc:(.*?)@(.*)\/to_s_spec\.rb:17>$/
    end
  end
end
