describe :rbx_kernel_load, :shared => true do
  before :each do
    CodeLoadingSpecs.spec_setup
    @rb, @rbc = CodeLoadingSpecs.rbc_fixture "load_fixture.rb"
  end

  after :each do
    CodeLoadingSpecs.spec_cleanup
    rm_r @rb, @rbc
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

    @object.send(@method, @rb).should be_true
    ScratchPad.recorded.should == [:loaded]
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
