require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/common', __FILE__)

describe "Rubinius::Loader.main" do
  before :each do
    @stdout, $stdout = $stdout, IOStub.new
  end

  after :each do
    $stdout = @stdout
  end

  it "catches any uncaught exceptions raised while running" do
    loader = Rubinius::Loader.new
    loader.should_receive(:main).and_raise(LoaderSpecs::UncaughtException.new)
    Rubinius::Loader.stub!(:new).and_return(loader)

    Rubinius::Loader.main.should == 128

    $stdout.should == <<-EOM

=====================================
Exception occurred during top-level exception output! (THIS IS BAD)
Exception: #<TypeError: exception class/object expected> (TypeError)
    EOM
  end
end
