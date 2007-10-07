@getoptlong_get = shared "GetoptLong#get" do |cmd|
  describe "GetoptLong##{cmd}" do
    before(:each) do
      @opts = GetoptLong.new(
        [ '--size', '-s',             GetoptLong::REQUIRED_ARGUMENT ],
        [ '--verbose', '-v',          GetoptLong::NO_ARGUMENT ],
        [ '--query', '-q',            GetoptLong::NO_ARGUMENT ],
        [ '--check', '--valid', '-c', GetoptLong::NO_ARGUMENT ]
      )
    end
    
    it "should return the next option name and its argument as an Array" do
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
    
    it "should shift ARGV on each call" do
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
    
    it "should terminate processing when encountering '--'" do
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
    
    it "should raise a if an argument was required, but none given" do
      begin
        old_argv = ARGV
        ARGV = [ "--size" ]
      
        should_raise(GetoptLong::MissingArgument, "option `--size' requires an argument") do
          @opts.send(cmd)
        end
      ensure
        ARGV = old_argv
      end
    end
    
  end
end