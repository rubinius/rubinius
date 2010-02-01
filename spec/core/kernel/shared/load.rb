describe :rbx_kernel_load, :shared => true do
  before :each do
    CodeLoadingSpecs.spec_setup
    @rb, @rbc = CodeLoadingSpecs.rbc_fixture "load_fixture.rb"
  end

  after :each do
    CodeLoadingSpecs.spec_cleanup
    rm_r @rb, @rbc
  end

  it "loads a .rbc file directly" do
    @object.send(@method, @rbc).should be_true
    ScratchPad.recorded.should == [:loaded]
  end

  it "saves a .rbc file when loading a .rb file" do
    rm_r @rbc
    @object.send(@method, @rb).should be_true
    File.exists?(@rbc).should be_true
  end

  it "loads a .rbc file if it is newer than the related .rb file" do
    touch(@rb) { |f| f.puts "ScratchPad << :not_loaded" }

    now = Time.now
    File.utime now, now, @rbc

    @object.send(@method, @rbc).should be_true
    ScratchPad.recorded.should == [:loaded]
  end

  it "loads a .rbc file even if the related .rb file is missing" do
    rm_r @rb
    @object.send(@method, @rbc).should be_true
    ScratchPad.recorded.should == [:loaded]
  end
end

describe :rbx_kernel_load_recursive, :shared => true do
  before :each do
    CodeLoadingSpecs.spec_setup
    @rb, @rbc = CodeLoadingSpecs.rbc_fixture("recursive_fixture.rb") do |rb, rbc, f|
      f.puts "ScratchPad << :loaded"
      f.puts "require #{rbc.inspect}"
    end
  end

  after :each do
    CodeLoadingSpecs.spec_cleanup
    rm_r @rb, @rbc
  end

  it "does not recursively load a .rbc file that requires itself" do
    @object.load(@rbc).should be_true
    ScratchPad.recorded.should == [:loaded, :loaded]
  end
end

describe :rbx_kernel_load_no_ext, :shared => true do
  before :each do
    CodeLoadingSpecs.spec_setup
  end

  after :each do
    CodeLoadingSpecs.spec_cleanup
    rm_r @rb, @rbc
  end

  it "saves a Ruby source file with no extension to <name>.compiled.rbc" do
    @rb = tmp("no_ext_fixture")
    @rbc = @rb + ".compiled.rbc"

    touch(@rb) { |f| f.puts "ScratchPad << :loaded_no_ext" }
    @object.load(@rb).should be_true
    ScratchPad.recorded.should == [:loaded_no_ext]
    File.exists?(@rbc).should be_true
  end

  it "saves a Ruby source file with arbitrary extension to <name>.compiled.rbc" do
    @rb = tmp("no_ext_fixture.ext")
    @rbc = @rb + ".compiled.rbc"

    touch(@rb) { |f| f.puts "ScratchPad << :loaded_no_ext" }
    @object.load(@rb).should be_true
    ScratchPad.recorded.should == [:loaded_no_ext]
    File.exists?(@rbc).should be_true
  end
end

describe :rbx_kernel_load_rba_relative, :shared => true do
  before :each do
    CodeLoadingSpecs.spec_setup

    @rb, @rbc = CodeLoadingSpecs.rbc_fixture "load_fixture.rb"
    @rba = CodeLoadingSpecs.rba_fixture File.basename(@rbc), @rbc
    $LOAD_PATH << @rba

    rm_r @rb, @rbc
  end

  after :each do
    CodeLoadingSpecs.spec_cleanup
    rm_r @rba
  end

  it "loads a .rb file from a .rba file that appears in $LOAD_PATH" do
    @object.send(@method, "load_fixture.rb").should be_true
    ScratchPad.recorded.should == [:loaded]
  end

  it "loads a .rbc file from a .rba file that appears in $LOAD_PATH" do
    @object.send(@method, "load_fixture.rbc").should be_true
    ScratchPad.recorded.should == [:loaded]
  end
end

# TODO: load from rba with absolute paths
