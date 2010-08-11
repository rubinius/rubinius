require File.expand_path('../../../../spec_helper', __FILE__)

describe "DATA" do
  it "presents $0 file data after __END__ as a File object" do
    data = ruby_exe(fixture(__FILE__, "data1.rb"))
    data.should == "[File, \"hello\"]"
  end
end

describe "DATA" do
  before :each do
    @stdout = tmp("data_empty_stdout.txt")
    @stderr = tmp("data_empty_stderr.txt")
  end

  after :each do
    rm_r @stdout, @stderr
  end

  it "may be included in an empty script" do
    ruby_exe(fixture(__FILE__, "data3.rb"), :args => "> #{@stdout} 2> #{@stderr}")
    File.size?(@stdout).should be_nil
    File.size?(@stderr).should be_nil
  end
end
