require File.expand_path('../spec_helper', __FILE__)

load_extension('file')

describe :rb_file_open, :shared => true do
  it "raises an ArgumentError if passed an empty mode string" do
    touch @name
    lambda { @s.rb_file_open(@name, "") }.should raise_error(ArgumentError)
  end

  it "opens a file in read-only mode with 'r'" do
    touch(@name) { |f| f.puts "readable" }
    @file = @s.send(@method, @name, "r")
    @file.should be_an_instance_of(File)
    @file.read.chomp.should == "readable"
  end

  it "creates and opens a non-existent file with 'w'" do
    @file = @s.send(@method, @name, "w")
    @file.write "writable"
    @file.flush
    @name.should have_data("writable")
  end

  it "truncates an existing file with 'w'" do
    touch(@name) { |f| f.puts "existing" }
    @file = @s.send(@method, @name, "w")
    @name.should have_data("")
  end
end

describe "C-API File function" do
  before :each do
    @s = CApiFileSpecs.new

    @name = tmp("rb_file_open")
  end

  after :each do
    @file.close if @file and !@file.closed?
    rm_r @name
  end

  describe "rb_file_open" do
    it_behaves_like :rb_file_open, :rb_file_open
  end
end
