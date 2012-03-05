require File.expand_path('../../../spec_helper', __FILE__)

#Spec for Process.daemon(nochdir, noclose)
ruby_version_is "1.9" do
  describe "Process.daemon" do
    platform_is :windows do
      it "returns false from #respond_to?" do
        Process.respond_to?(:daemon).should be_false
      end

      it "raises a NotImplementedError when called" do
        lambda { Process.daemon }.should raise_error(NotImplementedError)
      end
    end

    platform_is_not :windows do
      before :each do
        def test_daemon(name,rootdir,nochdir,noclose)
          p = Process.fork {
            #Create the file handle in the same directory as the parent before we daemonize
            f_exist = File.new(File.join(rootdir,"#{name}_exist"),"w")
            f_stdin = File.new(File.join(rootdir,"#{name}_stdin"),"w")
            f_stdin << "true"
            f_stdin.close

            #TODO:  Reroute stderr and stdout to append to the p1_exist file.
            $stderr.reopen File.join(rootdir,"#{name}_stderr")
            $stdout.reopen File.join(rootdir,"#{name}_stdout")
            $stdin.reopen File.join(rootdir,"#{name}_stdin")

            #Daemonize the process
            returnval = Process.daemon(nochdir,noclose)

            #Ensure that standard streams are still pointing to the files we set up
            $stderr.puts "true"
            $stdout.puts "true"

            $stderr.flush
            $stdout.flush

            #Write the test results to the return file so the parent may process them
            f_exist << "{return=#{returnval},pwd=#{Dir.pwd},stdout=#{File.new(File.join(rootdir,"#{name}_stdout"),"r").read},
            stderr=#{File.new(File.join(rootdir,"#{name}_stderr"),"r").read},stdin=#{$stdin.gets},pid=#{Process.pid}}".gsub(/\r/,"").gsub(/\n/,"").gsub(/=,/,"=nil,")

            f_exist.close
            $stderr.close
            $stdout.close
            $stdin.close

            File.delete(File.join(rootdir,"#{name}_stderr"))
            File.delete(File.join(rootdir,"#{name}_stdout"))
            File.delete(File.join(rootdir,"#{name}_stdin"))
        
            exit
          }
          p
        end

        def wait_for_file(filename)
          #This is to ensure that the following file ops don't turn into
          #infinite loops.  The loops will run with a limit of 5 sec each.
          currtime = Time.now

          while not File.exists?(filename) do
            if(Time.now > currtime + 5)
              return -1
            end
          end

          currtime = Time.now

          while /.*\}\Z/.match(File.new(filename,"r").read) == nil do
            if(Time.now > currtime + 5)
              return -1
            end
          end

          return 0
        end

        @rootdir = Dir.pwd
        @procfilename = File.join(@rootdir, 'p_exist')
        
        if(File.exists?(@procfilename))
          File.delete(@procfilename)
        end
      end

      after :each do
        if(File.exists?(@procfilename))
          File.delete(@procfilename)
        end
      end

      it "redirects the std input, std output, and std error to /dev/null when noclose = false and changes the pwd to root when nochdir = false" do
        p = test_daemon("p",@rootdir,false,false)

        if wait_for_file(@procfilename) == 0
          File.new(@procfilename,"r").read.sub("{","").sub("}","").split(/,/).each {|return_pair|
          mypair = return_pair.split(/=/)

            case mypair[0]
              #Return value must be zero for a successful run
              when "return"
                mypair[1].should == "0"
              #The pwd must be the same as the OS root, since
              #nochdir = false
              when "pwd"
                mypair[1].should == "/"
              #STDOUT, STDERR, and STDIN must all return nil, since
              #noclose = false
              when "stdout"
                mypair[1].should == "nil"
              when "stderr"
                mypair[1].should == "nil"
              when "stdin"
                mypair[1].should == "nil"
              when "pid"
                #The PID cannot be the same as that of the parent process or the
                #child process that daemonized it.
                mypair[1].should_not == Process.pid.to_s
                mypair[1].should_not == p.to_s
            end
          }
        end
      end

      it "keeps the std input, std output, and std error as-is when noclose = true and keeps the pwd as-is when nochdir = true" do
        p = test_daemon("p",@rootdir,true,true)

        if wait_for_file(@procfilename) == 0
          File.new(@procfilename,"r").read.sub("{","").sub("}","").split(/,/).each {|return_pair|
          mypair = return_pair.split(/=/)

            case mypair[0]
              #Return value must be zero for a successful run
              when "return"
                mypair[1].should == "0"
              #The pwd must be the same as the parent process, since
              #nochdir = true
              when "pwd"
                mypair[1].should == @rootdir
              #STDOUT, STDERR, and STDIN must all return true, since
              #noclose = true
              when "stdout"
                mypair[1].should == "true"
              when "stderr"
                mypair[1].should == "true"
              when "stdin"
                mypair[1].should == "true"
              when "pid"
                #The PID cannot be the same as that of the parent process or the
                #child process that daemonized it.
                mypair[1].should_not == Process.pid.to_s
                mypair[1].should_not == p.to_s
            end
          }
        end
      end

      #it "returns "Errno.*" on error." do
      #end
    end
  end

  #it "needs to be reviewed for spec completeness"
  #end
end
