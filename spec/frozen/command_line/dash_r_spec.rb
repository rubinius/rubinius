require File.dirname(__FILE__) + '/../spec_helper'

describe "The -r command line option" do
  it "requires the specified file" do
    ruby_exe("fixtures/require.rb", :options => "-r fixtures/test_file", :dir => File.dirname(__FILE__)).chomp.should include("fixtures/test_file.rb")
  end
end
