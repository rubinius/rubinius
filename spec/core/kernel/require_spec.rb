require File.dirname(__FILE__) + '/../../spec_helper'

$require_fixture_dir = (File.dirname(__FILE__) + '/fixtures/require')
$LOAD_PATH << $require_fixture_dir
$LOAD_PATH << (File.dirname(__FILE__) + '/fixtures/require/require_spec_rba.rba')

$require_spec   = nil
$require_spec_1 = nil
$require_spec_2 = nil
$require_spec_3 = nil
$require_spec_4 = nil
$require_spec_5 = nil
$require_spec_6 = nil
$require_spec_7 = nil
$require_spec_8 = nil
$require_spec_9 = nil
$require_spec_10 = nil
$require_spec_rooby = nil

require 'rbconfig'

describe "Kernel#require" do
  # Avoid storing .rbc and .rba in repo
  before :all do
    Dir.chdir($require_fixture_dir) {
#      `rm *.rbc require_dynamic*`
      `touch require_spec_dummy.#{Config::CONFIG['DLEXT']}`
      `touch require_spec_dummy.rb`
    }

    extension :rbx do
      Kernel.compile("#{$require_fixture_dir}/require_spec_10.rb")

      Dir.chdir($require_fixture_dir) {
        `zip require_spec_rba.rba require_spec_10.rbc`
      }
    end
  end

  # The files used below just contain code that assigns
  # Time.now to the respective global variable so that
  # reloads can easily be verified.

  # CAUTION: Some of these work because a different path is used to
  #          load the same file. Be careful if you change the order
  #          or add items.

  it "loads a .rb from an absolute path and returns true" do
    path = File.expand_path(File.dirname(__FILE__) + '/fixtures/require/require_spec_1.rb')

    require(path).should == true
    $require_spec_1.nil?.should == false
  end

  it "loads a .rb from a relative path and returns true" do
    Dir.chdir($require_fixture_dir) do |dir|
      $require_spec_1 = nil
      require('../../fixtures/require/require_spec_1.rb').should == true
      $require_spec_1.nil?.should == false

      $require_spec_1 = nil
      require('./../require/require_spec_1.rb').should == true
      $require_spec_1.nil?.should == false
    end
  end

  it "loads an unqualified .rb by looking in $LOAD_PATH and returns true" do
    require('require_spec_2.rb').should == true
    $require_spec_2.nil?.should == false
  end

failure :ruby, :rubinius do
  it "loads extension files" do
    # Not sure how to spec this yet since it needs an extfile
    fail "Not implemented"
  end
end

  it "does not expand/resolve qualified files against $LOAD_PATH" do
    Dir.chdir($require_fixture_dir + '/../') do |dir|
      # This would be a valid path if expanded against the fixture dir
      lambda { require '../require/require_spec_2.rb' }.should raise_error LoadError
    end
  end

  it "allows unqualified files to contain path information (just not in the beginning)" do
    name = (File.dirname(__FILE__) + '/fixtures')
    $LOAD_PATH << name

    $require_spec2 = nil
    require('require/../require/require_spec_2.rb').should == true
    $require_spec_2.nil?.should == false
    
    $LOAD_PATH.delete name
  end

  it "appends a file with no extension with .rb/.<ext> in that order to locate file" do
    load('require_spec')
    $require_spec.should == :noext
    load('require_spec.rb')
    $require_spec.should == :rb

    $require_spec = nil

    require('require_spec')
    $require_spec.should == :rb
  end

  it "prefers to use .rb over .<ext> if given non-extensioned file and both exist" do
    require('require_spec_dummy').should == true
    $LOADED_FEATURES.include?('require_spec_dummy.rb').should == true
    $LOADED_FEATURES.include?("require_spec_dummy.#{Config::CONFIG['DLEXT']}").should == false
  end

  it "will load file.rb when given 'file' if it exists even if file.<ext> is loaded" do
    $LOADED_FEATURES << "require_spec_3.#{Config::CONFIG['DLEXT']}"
    require('require_spec_3.rb').should == true
    $LOADED_FEATURES.include?('require_spec_3.rb').should == true
  end

  it "will not load file.<ext> when given 'file' if file.rb already loaded" do
    unless $LOADED_FEATURES.include?('require_spec_dummy.rb')
      require('require_spec_dummy.rb') 
    end

    require('require_spec_dummy').should == false
  end

failure :ruby, :rubinius do
  it "will load explicit file.<ext> even if file.rb already loaded and vice versa" do
    # Not sure how to spec this yet because it needs an extfile. 
    fail "Spec not implemented"
  end
end

  it "appends any non-ruby extensioned file with .rb/.<ext> in that order to locate file" do
    load('require_spec.rooby')
    $require_spec_rooby.should == :rooby
    load('require_spec.rooby.rb')
    $require_spec_rooby.should == :rb

    $require_spec_rooby = nil

    require('require_spec.rooby')
    $require_spec_rooby.should == :rb
  end

  it "stores the loaded file in $LOADED_FEATURES" do
    $LOADED_FEATURES.include?('require_spec_6.rb').should == false
    require('require_spec_6.rb').should == true
    $LOADED_FEATURES.include?('require_spec_6.rb').should == true
  end

  it "stores a non-extensioned file with its located suffix" do
    $LOADED_FEATURES.delete 'require_spec_6.rb'

    require('require_spec_6').should == true

    $LOADED_FEATURES.include?('require_spec_6.rb').should == true
  end

  it "bases the filename in $LOADED_FEATURES on the path given, not just basename" do
    $LOADED_FEATURES.include?('require_spec_6.rb').should == true

    Dir.chdir($require_fixture_dir) do |dir|
      require('../../fixtures/require/require_spec_6.rb').should == true 
    end

    $LOADED_FEATURES.include?('require_spec_6.rb').should == true
    $LOADED_FEATURES.include?('../../fixtures/require/require_spec_6.rb').should == true
  end

  it "will not load the same file twice, returns false instead" do
    $LOADED_FEATURES.include?('require_spec_7.rb').should == false

    require('require_spec_7.rb').should == true
    a = $require_spec_7
    a.nil?.should == false
    
    $LOADED_FEATURES.include?('require_spec_7.rb').should == true

    require('require_spec_7.rb').should == false
    b = $require_spec_7
    b.nil?.should == false

    # Timestamps should not differ
    a.eql?(b).should == true

    $LOADED_FEATURES.include?('require_spec_7.rb').should == true
  end

  it "checks $LOADED_FEATURES to see whether file is already loaded" do
    $LOADED_FEATURES.include?('require_spec_7.rb').should == true
    require('require_spec_7.rb').should == false

    $LOADED_FEATURES.delete 'require_spec_7.rb'
    require('require_spec_7.rb').should == true
    require('require_spec_7.rb').should == false

    $LOADED_FEATURES.include?('require_spec_7.rb').should == true
  end
  
  it "raises a LoadError if the file can't be found" do
    lambda { require "nonesuch#{$$}#{Time.now.to_f}" }.should raise_error LoadError
  end

  it "only accepts strings" do
    lambda { require(nil) }.should raise_error(TypeError)
    lambda { require(42)  }.should raise_error(TypeError)
    lambda { require([])  }.should raise_error(TypeError)
  end


extension :rbx do
  
#  it "loads a .rbc file directly" do
#    require('require_spec_2.rb').should == true
#    require('require_spec_2.rbc').should == true
#  end

  it "compiles a .rbc file when re-evaluating the source file" do
    Dir.chdir($require_fixture_dir) do |dir|
      system("rm -f require_spec_8.rbc")
    end

    File.exist?("#{$require_fixture_dir}/require_spec_8.rbc").should == false

    require('require_spec_8.rb').should == true

    File.exist?("#{$require_fixture_dir}/require_spec_8.rbc").should == true
  end

  it "loads a .rbc file if it's not older than the associated .rb file" do
    time = Time.now

    Dir.chdir($require_fixture_dir) do |dir|
      File.open('require_spec_dynamic.rb', 'w+') do |f| 
        f.puts "$require_spec_dynamic = [#{time.tv_sec}, #{time.tv_usec}]" 
      end
    end

    Kernel.compile "#{$require_fixture_dir}/require_spec_dynamic.rb"

    require('require_spec_dynamic.rb').should == true
    $require_spec_dynamic.should == [time.tv_sec, time.tv_usec]

    $LOADED_FEATURES.delete 'require_spec_dynamic.rb'

    require('require_spec_dynamic.rb').should == true
    $require_spec_dynamic.should == [time.tv_sec, time.tv_usec]

    require('require_spec_dynamic.rb').should == false
    $require_spec_dynamic.should == [time.tv_sec, time.tv_usec]

    time2 = Time.now

    Dir.chdir($require_fixture_dir) do |dir|
      `mv require_spec_dynamic.rbc rsd.old`

      File.open('require_spec_dynamic.rb', 'w+') do |f| 
        f.puts "$require_spec_dynamic = [#{time2.tv_sec}, #{time2.tv_usec}]" 
      end
    end

    $LOADED_FEATURES.delete 'require_spec_dynamic.rb'

    require('require_spec_dynamic.rb').should == true
    $require_spec_dynamic.should == [time2.tv_sec, time2.tv_usec]

    Dir.chdir($require_fixture_dir) do |dir|
      `mv rsd.old require_spec_dynamic.rbc`
      `touch require_spec_dynamic.rbc`
    end

    $LOADED_FEATURES.delete 'require_spec_dynamic.rb'

    require('require_spec_dynamic.rb').should == true
    $require_spec_dynamic.should == [time.tv_sec, time.tv_usec]
  end

  it "loads a .rbc even if the .rb is missing" do
    begin 
      Kernel.compile "#{$require_fixture_dir}/require_spec_9.rb"

      Dir.chdir($require_fixture_dir) do |dir|
        `mv require_spec_9.rb ls9.old`
      end

      require('require_spec_9.rb').should == true

    ensure
      Dir.chdir($require_fixture_dir) do |dir|
        system "mv ls9.old require_spec_9.rb"
      end
    end
  end

  it "loads a .rb from a .rba in $LOAD_PATH" do
    require('require_spec_10.rb').should == true
    $require_spec_10.nil?.should == false
  end

  it "loads a .rb from a .rba in $LOAD_PATH, only once" do
    $LOADED_FEATURES.delete 'require_spec_10.rb'
    
    require('require_spec_10.rb').should == true
    a = $require_spec_2
    a.nil?.should == false

    require('require_spec_2').should == false
    b = $require_spec_2

    a.eql?(b).should == true
  end
end
end
