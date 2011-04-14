require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "IO.popen" do
  it "reads from a read-only pipe" do
    IO.popen("echo foo", "r") do |io|
      io.read.should == "foo\n"

      lambda {
        io.write('foo')
      }.should raise_error(IOError, 'not opened for writing')
    end
  end

  platform_is_not :windows do
    it "reads and writes to a read/write pipe" do
      data = IO.popen("cat", "r+") do |io|
        io.write("bar")
        io.read 3
      end

      data.should == "bar"
    end

    it "writes to a write-only pipe" do
      begin
        tmp_file = tmp "IO_popen_spec_#{$$}"

        data = IO.popen "cat > #{tmp_file}", 'w' do |io|
          io.write 'bar'

          lambda { io.read.should }.should \
            raise_error(IOError, 'not opened for reading')
        end
        system 'sync' # sync to flush writes for File.read below

        File.read(tmp_file).should == 'bar'

      ensure
        File.unlink tmp_file if File.exist? tmp_file
      end
    end
  end

  it "returns the value of the block when passed a block" do
    val = IO.popen("yes", "r") do |i|
      :hello
    end

    val.should == :hello
  end

  it "closes the IO when used with a block" do
    io = IO.popen("yes", "r") do |i|
      i
    end

    io.closed?.should be_true
  end

  it "allows the IO to be closed inside the block" do
    io = IO.popen('yes', 'r') do |i|
      i.close
      i
    end

    io.closed?.should be_true
  end

  it "returns the IO if no block given" do
    io = IO.popen("yes", "r")
    io.closed?.should be_false

    io.read(1).should == "y"
    io.close
  end

  it "starts returns a forked process if the command is -" do
    io = IO.popen("-")

    if io # parent
      io.gets.should == "hello from child\n"
      io.close
    else # child
      puts "hello from child"
      exit!
    end
  end

  it "yields an instance of a subclass when called on a subclass" do
    IOSpecs::SubIO.popen("true", "r") do |io|
      io.should be_an_instance_of(IOSpecs::SubIO)
    end
  end

  it "returns an instance of a subclass when called on a subclass" do
    io = IOSpecs::SubIO.popen("true", "r")
    io.should be_an_instance_of(IOSpecs::SubIO)
    io.close
  end

  ruby_version_is "1.9.2" do
    platform_is_not :windows do # not sure what commands to use on Windows
      describe "with a leading Array parameter" do
        it "uses the Array as command plus args for the child process" do
          io = IO.popen(["yes", "hello"]) do |i|
            i.read(5).should == 'hello'
          end
        end

        it "uses a leading Hash in the Array as additional environment variables" do
          io = IO.popen([{'foo' => 'bar'}, 'env']) do |i|
            i.read.should =~ /foo=bar/
          end
        end

        it "uses a trailing Hash in the Array for spawn-like settings" do
          io = IO.popen(['sh', '-c', 'does_not_exist', {:err => [:child, :out]}]) do |i|
            i.read.should =~ /not found/
          end
        end
      end
    end
  end

end
