shared :getoptlong_get do |cmd|
  describe "GetoptLong##{cmd}" do
    before(:each) do
      @opts = GetoptLong.new(
        [ '--size', '-s',             GetoptLong::REQUIRED_ARGUMENT ],
        [ '--verbose', '-v',          GetoptLong::NO_ARGUMENT ],
        [ '--query', '-q',            GetoptLong::NO_ARGUMENT ],
        [ '--check', '--valid', '-c', GetoptLong::NO_ARGUMENT ]
      )
    end
    
    it "returns the next option name and its argument as an Array" do
      begin
        old_argv = ARGV
        ARGV = [ "--size", "10k", "-v", "-q", "a.txt", "b.txt" ]
      
        @opts.send(cmd).should == [ "--size", "10k" ]
        @opts.send(cmd).should == [ "--verbose", "" ]
        @opts.send(cmd).should == [ "--query", ""]
        @opts.send(cmd).should == nil
      ensure
        ARGV = old_argv
      end
    end
    
    it "shifts ARGV on each call" do
      begin
        old_argv = ARGV
        ARGV = [ "--size", "10k", "-v", "-q", "a.txt", "b.txt" ]
      
        @opts.send(cmd)
        ARGV.should == [ "-v", "-q", "a.txt", "b.txt" ]
        
        @opts.send(cmd)
        ARGV.should == [ "-q", "a.txt", "b.txt" ]
        
        @opts.send(cmd)
        ARGV.should == [ "a.txt", "b.txt" ]
        
        @opts.send(cmd)
        ARGV.should == [ "a.txt", "b.txt" ]
      ensure
        ARGV = old_argv
      end
    end
    
    it "terminates processing when encountering '--'" do
      begin
        old_argv = ARGV
        ARGV = [ "--size", "10k", "--", "-v", "-q", "a.txt", "b.txt" ]
      
        @opts.send(cmd)
        ARGV.should == ["--", "-v", "-q", "a.txt", "b.txt"]
        
        @opts.send(cmd)
        ARGV.should ==  ["-v", "-q", "a.txt", "b.txt"]
        
        @opts.send(cmd)
        ARGV.should ==  ["-v", "-q", "a.txt", "b.txt"]
      ensure
        ARGV = old_argv
      end
    end
    
    it "raises a if an argument was required, but none given" do
      begin
        s = $stderr
        $stderr = IOStub.new
        old_argv = ARGV
        ARGV = [ "--size" ]
      
        lambda { @opts.send(cmd) }.should raise_error(GetoptLong::MissingArgument)
      ensure
        ARGV = old_argv
        $stderr = s
      end
    end
    
  end
end