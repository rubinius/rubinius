require File.dirname(__FILE__) + '/../../../spec_helper'

extension :rubinius do
  describe "Kernel.StringValue" do
    specify "raises TypeError if the instance does not respond to to_str" do
      class A; end
      lambda { StringValue(A.new) }.should raise_error(TypeError)
    end

    specify "raises TypeError for nil" do
      lambda { StringValue(nil) }.should raise_error(TypeError)
    end

    specify "raises TypeError for a Fixnum" do
      lambda { StringValue(1) }.should raise_error(TypeError)
    end

    specify "raises TypeError if the instance responds to to_str but doesn't return a String" do
      class B
        def to_str
          1
        end
      end
      lambda { StringValue(B.new) }.should raise_error(TypeError)
    end

    specify "calls to_str if the instance responds to it" do
      class C
        def to_str
          "zam"
        end
      end
      StringValue(C.new).should == "zam"
    end
  end
  
  describe "Kernel#load" do
    it "loads a .rbc file directly" do
      load('load_spec_2.rb').should == true
      load('load_spec_2.rbc').should == true
    end

    it "compiles a new .rbc file whenever using the source file" do
      `rm #{$load_fixture_dir}/load_spec_2.rbc`

      load('load_spec_2.rb').should == true

      File.exist?("#{$load_fixture_dir}/load_spec_2.rbc").should == true
    end

    it "generates a .rbc but no .rb file if using a file with no extension (appends .rbc)" do
      `rm #{$load_fixture_dir}/load_spec.rbc`

      load('load_spec').should == true
      File.exist?("#{$load_fixture_dir}/load_spec.rbc").should == true
    end

    it "generates a .rbc file if using a file with an arbitrary extension (appends .rbc)" do
      `rm #{$load_fixture_dir}/load_spec.rooby.rbc`

      load('load_spec.rooby').should == true
      File.exist?("#{$load_fixture_dir}/load_spec.rooby.rbc").should == true
    end

    it "loads a .rbc file if it's not older than the associated .rb file" do
      begin
        time = Time.now

        File.open("#{$load_fixture_dir}/load_spec_dynamic.rb", 'w+') do |f| 
          f.puts "$load_spec_dynamic = [#{time.tv_sec}, #{time.tv_usec}]" 
        end

        Kernel.compile "#{$load_fixture_dir}/load_spec_dynamic.rb"

        load('load_spec_dynamic.rb').should == true
        $load_spec_dynamic.should == [time.tv_sec, time.tv_usec]

        load('load_spec_dynamic.rb').should == true
        $load_spec_dynamic.should == [time.tv_sec, time.tv_usec]

        time2 = Time.now

        Dir.chdir($load_fixture_dir) do |dir|
          `mv load_spec_dynamic.rbc rsd.old`

          File.open('load_spec_dynamic.rb', 'w+') do |f| 
            f.puts "$load_spec_dynamic = [#{time2.tv_sec}, #{time2.tv_usec}]" 
          end
        end

        load('load_spec_dynamic.rb').should == true
        $load_spec_dynamic.should == [time2.tv_sec, time2.tv_usec]

        Dir.chdir($load_fixture_dir) do |dir|
          `mv rsd.old load_spec_dynamic.rbc`
          `touch load_spec_dynamic.rbc`
        end

        load('load_spec_dynamic.rb').should == true
        $load_spec_dynamic.should == [time.tv_sec, time.tv_usec]

      ensure
        Dir.chdir($load_fixture_dir) do |dir|
          `rm -f ./load_dynamic.rb*`
        end
      end
    end

    it "loads a .rbc even if the .rb is missing" do
      begin
        load('load_spec_9.rb').should == true

        Dir.chdir($load_fixture_dir) do |dir|
          `mv load_spec_9.rb ls9.old`
        end

        load('load_spec_9.rb').should == true

      ensure
        Dir.chdir($load_fixture_dir) do |dir|
           `mv ls9.old load_spec_9.rb`
        end
      end   
    end

    it "loads a .rbc from a .rba in $LOAD_PATH" do
      load('load_spec_10.rbc').should == true
      $load_spec_10.nil?.should == false
    end

    it "loads a .rbc from a .rba in $LOAD_PATH if only given .rb name" do
      $load_spec_10 = nil
      load('load_spec_10.rb').should == true
      $load_spec_10.nil?.should == false
    end
  end     
end
