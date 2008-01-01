require File.dirname(__FILE__) + '/../../spec_helper'

$load_fixture_dir = (File.dirname(__FILE__) + '/../../fixtures/load')
$LOAD_PATH << $load_fixture_dir
$LOAD_PATH << (File.dirname(__FILE__) + '/../../fixtures/load/load_spec_rba.rba')

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

describe "Kernel#load" do

  # Avoid storing .rbc and .rba in repo
  before :all do
    Dir.chdir($load_fixture_dir) do |dir|
      `rm -f ./*.rbc`
    end

    compliant_on :rubinius do
      Kernel.compile($load_fixture_dir + '/load_spec_10.rb')

      Dir.chdir($load_fixture_dir) do |dir|
        `zip load_spec_rba.rba load_spec_10.rbc`
      end
    end
  end

  it "loads a .rbc file directly" do
    load('load_spec_2.rb').should == true
    load('load_spec_2.rbc').should == true
  end

  it "compiles a new .rbc file whenever using the source file" do
    `rm -f #{$load_fixture_dir}/load_spec_2.rbc`

    load('load_spec_2.rb').should == true

    File.exist?("#{$load_fixture_dir}/load_spec_2.rbc").should == true
  end

  it "generates a .rbc but no .rb file if using a file with no extension (appends .rbc)" do
    `rm -f #{$load_fixture_dir}/load_spec.rbc`

    load('load_spec').should == true
    File.exist?("#{$load_fixture_dir}/load_spec.rbc").should == true
  end

  it "generates a .rbc file if using a file with an arbitrary extension (appends .rbc)" do
    `rm -f #{$load_fixture_dir}/load_spec.rooby.rbc`

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
