describe "sets $VERBOSE to true", :shared => true do
  it "sets $VERBOSE to true" do
    ruby_exe("fixtures/verbose.rb", :options => @method, :dir => "#{File.dirname(__FILE__)}/..").chomp.match(/true$/).should_not == nil
  end
end
