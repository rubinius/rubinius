require File.dirname(__FILE__) + '/../../spec_helper'

# TODO: fix!
$require_fixture_dir = (File.dirname(__FILE__) + '/../../ruby/fixtures/require')
$LOAD_PATH << $require_fixture_dir
$LOAD_PATH << (File.dirname(__FILE__) + '/../../ruby/fixtures/require/require_spec_rba.rba')

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
      `rm -f ./*.rbc`
      `touch require_spec_dummy.#{Config::CONFIG['DLEXT']}`
      `touch require_spec_dummy.rb`
    }

    Kernel.compile("#{$require_fixture_dir}/require_spec_10.rb")

    Dir.chdir($require_fixture_dir) {
      system "rm -f reqire_spec_rba.rba"
      system "ar r reqire_spec_rba.rba reqire_spec_10.rbc 2>/dev/null"
    }
  end

  it "loads a .rbc file directly" do
    $LOADED_FEATURES.delete 'require_spec_2.rb'
    load('require_spec_2.rb')
    require('require_spec_2.rbc').should == true
    $LOADED_FEATURES.include?('require_spec_2.rb').should == true
  end

  it "compiles a .rbc file when re-evaluating the source file" do
    Dir.chdir($require_fixture_dir) do |dir|
      system("rm -f require_spec_8.rbc")
    end

    File.exist?("#{$require_fixture_dir}/require_spec_8.rbc").should == false

    require('require_spec_8.rb').should == true

    File.exist?("#{$require_fixture_dir}/require_spec_8.rbc").should == true
  end

  it "loads a .rbc file if it's not older than the associated .rb file" do
    begin
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

    ensure
      Dir.chdir($require_fixture_dir) do |dir|
        `rm -f ./require_dynamic.rb*`
      end
    end
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

  it "does not recursively load an rbc file that requires itself" do
    $require_spec_recursive = nil
    $LOADED_FEATURES.delete 'require_spec_recursive.rb'

    begin
      Kernel.compile("#{$require_fixture_dir}/require_spec_recursive.rb")

      Dir.chdir($require_fixture_dir) do
        `mv require_spec_recursive.rb tmp1234`
      end

      $LOADED_FEATURES.include?('require_spec_recursive.rb').should == false

      require('require_spec_recursive').should == true

      $LOADED_FEATURES.include?('require_spec_recursive.rb').should == true
      $require_spec_recursive.nil?.should == false

    ensure
      Dir.chdir($require_fixture_dir) do
        `mv tmp1234 require_spec_recursive.rb`
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
