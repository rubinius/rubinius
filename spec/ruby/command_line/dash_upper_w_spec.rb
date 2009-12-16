require File.dirname(__FILE__) + '/../spec_helper'
require File.dirname(__FILE__) + '/shared/verbose'

describe "The -W command line option" do
  it "with 0 sets $VERBOSE to nil" do
    ruby_exe("fixtures/verbose.rb", :options => "-W0", :dir => File.dirname(__FILE__)).chomp.should == "nil"
  end

  it "with 1 sets $VERBOSE to false" do
    ruby_exe("fixtures/verbose.rb", :options => "-W1", :dir => File.dirname(__FILE__)).chomp.should == "false"
  end
end

describe "The -W command line option with 2" do
  it_behaves_like "sets $VERBOSE to true", "-W2"
end
