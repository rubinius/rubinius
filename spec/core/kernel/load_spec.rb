require File.dirname(__FILE__) + '/../../spec_helper'

$load_fixture_dir = (File.dirname(__FILE__) + '/fixtures/load')
$LOAD_PATH << $load_fixture_dir
$LOAD_PATH << (File.dirname(__FILE__) + '/fixtures/load/load_spec_rba.rba')

$load_spec_1 = nil
$load_spec_2 = nil
$load_spec_3 = nil
$load_spec_4 = nil
$load_spec_5 = nil
$load_spec_6 = nil
$load_spec_7 = nil
$load_spec_8 = nil
$load_spec_9 = nil
$load_spec_10 = nil
$load_spec_wrap   = nil
$load_spec_wrap2  = nil
$load_spec = nil
$load_spec_rooby = nil

require 'rbconfig'

# The files used below just contain code that assigns
# Time.now to the respective global variable.
#
# The only exception are the _wrap files which contain
# similarly named classes. The class definitions, in
# addition, contain another global timestamp assignment.
describe "Kernel#load" do

  # Avoid storing .rbc and .rba in repo
  before :all do
    Dir.chdir($load_fixture_dir) do |dir|
      `rm -f ./*.rbc`
    end

    compliant :rubinius do
      Kernel.compile($load_fixture_dir + '/load_spec_10.rb')

      Dir.chdir($load_fixture_dir) do |dir|
        `zip load_spec_rba.rba load_spec_10.rbc`
      end
    end
  end

  it "loads a .rb from an absolute path and returns true" do
    path = File.expand_path(File.dirname(__FILE__) + '/fixtures/load/load_spec_1.rb')

    load(path).should == true
    $load_spec_1.nil?.should == false
  end

  it "loads a .rb from a relative path and returns true" do
    Dir.chdir($load_fixture_dir) do |dir|
      $load_spec_1 = nil
      load('../../fixtures/load/load_spec_1.rb').should == true
      $load_spec_1.nil?.should == false

      $load_spec_1 = nil
      load('./../load/load_spec_1.rb').should == true
      $load_spec_1.nil?.should == false
    end
  end

failure :ruby, :rubinius do
  it "loads extension files" do
    # Not sure how to spec this yet because it needs an extension file
    fail "Not implemented"
  end
end

  it "loads an unqualified .rb by looking in $LOAD_PATH and returns true" do
    load('load_spec_2.rb').should == true
    $load_spec_2.nil?.should == false
  end

  it "does not expand/resolve qualified files against $LOAD_PATH" do
    Dir.chdir($load_fixture_dir + '/../') do |dir|
      # This would be a valid path if expanded against the fixture dir
      lambda { load '../load/load_spec_2.rb' }.should raise_error LoadError
    end
  end

  it "allows unqualified files to contain path information (just not in the beginning)" do
    name = (File.dirname(__FILE__) + '/fixtures')
    $LOAD_PATH << name

    $load_spec_2 = nil
    load('load/../load/load_spec_2.rb').should == true
    $load_spec_2.nil?.should == false
    
    $LOAD_PATH.delete name
  end

  it "loads a file with no extension as a ruby source file" do
    load('load_spec').should == true
    $load_spec.nil?.should == false
  end

  it "loads a file with any extension as a ruby source file" do
    load('load_spec.rooby').should == true
    $load_spec_rooby.nil?.should == false
  end

  it "does not create a .rb file for the non-.rb files" do
    load('load_spec').should == true
    load('load_spec.rooby').should == true

    File.exist?("#{$load_fixture_dir}/load_spec.rb").should == false
    File.exist?("#{$load_fixture_dir}/load_spec.rooby.rb").should == false
  end

  it "does not add the loaded file to $LOADED_FEATURES" do
    $LOADED_FEATURES.grep(/load_spec_3.rb/).should == []
    load('load_spec_3.rb').should == true
    $load_spec_3.nil?.should == false
    $LOADED_FEATURES.grep(/load_spec_3.rb/).should == []
  end

  it "produces __FILE__ as the given filename and __LINE__ as the source line number" do
    Dir.chdir($load_fixture_dir) do |dir|
      load('load_spec_4.rb').should == true 
      $load_spec_4.should == [['./load_spec_4.rb', 1], ['./load_spec_4.rb', 10]]

      extension :rubinius do
        `rm load_spec_4.rbc`
      end
    end

    load("#{$load_fixture_dir}/load_spec_4.rb").should == true 
    $load_spec_4.should == [['./spec/core/kernel/fixtures/load/load_spec_4.rb', 1], 
                            ['./spec/core/kernel/fixtures/load/load_spec_4.rb', 10]]
  end

  it "reloads the file if invoked on the same filename again, returning true" do
    load('load_spec_4.rb').should == true
    load('load_spec_4.rb').should == true
  end

  it "re-evaluates the file each time it is loaded" do
    load('load_spec_5.rb').should == true
    a = $load_spec_5
    load('load_spec_5.rb').should == true
    b = $load_spec_5

    a.eql?(b).should == false
  end

  it "loads the file even if it has already been #required" do
    require('load_spec_6.rb').should == true
    a = $load_spec_6

    require('load_spec_6.rb').should == false
    b = $load_spec_6

    load('load_spec_6.rb').should == true
    c = $load_spec_6

    a.eql?(b).should == true
    c.eql?(a).should == false
  end
  
  it "does not cause #require on the same filename to fail" do 
    load('load_spec_7.rb').should == true
    a = $load_spec_7

    require('load_spec_7.rb').should == true
    b = $load_spec_7

    load('load_spec_7.rb').should == true
    c = $load_spec_7

    a.eql?(b).should == false
    b.eql?(c).should == false
    c.eql?(a).should == false
  end

  it "raises a LoadError if the file can't be found" do
    lambda { load("./nonexistent#{Time.now.to_f}#{$$}") }.should raise_error LoadError
  end

failure :rubinius do
  # TODO: This currently fails on Rubinius because the #load logic
  #       will take 'load_spec_1' and try 'load_spec_1.rbc' since
  #       that is the logic for .rb files. 
  it "raises a LoadError if filename given without its extension" do
    lambda { load('load_spec_1') }.should raise_error LoadError
    load('load_spec_1.rb').should == true
    lambda { load('load_spec_1') }.should raise_error LoadError
  end
end

  it "only accepts strings as the filename argument" do
    lambda { load(nil) }.should raise_error TypeError
    lambda { load(42) }.should raise_error TypeError
    lambda { load([]) }.should raise_error TypeError
  end

failure :rubinius do
  # TODO: This is completely unimplemented.
  it "allows wrapping the code in the file in an anonymous module" do
    lambda { LoadSpecWrap }.should raise_error NameError
    lambda { LoadSpecWrapTwo }.should raise_error NameError
   
    load('load_spec_wrap.rb').should == true
    $load_spec_wrap.nil?.should == false
    LoadSpecWrap.lsw.should == :lsw 

    load('load_spec_wrap2.rb', true).should == true
    $load_spec_wrap2.nil?.should == false
    lambda { LoadSpecWrapTwo }.should raise_error NameError
  end    
end


extension :rubinius do
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
