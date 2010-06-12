require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::Options#to_s" do
  before :each do
    @opt = Rubinius::Options.new "command"
  end

  it "returns the banner and descriptive strings for all registered options" do
    @opt.on "-t", "--this ARG", "Adds this ARG to the list"
    @opt.to_s.should == <<-EOD
command

   -t, --this ARG             Adds this ARG to the list
EOD
  end
end
