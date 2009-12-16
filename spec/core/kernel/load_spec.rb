require File.dirname(__FILE__) + '/../../spec_helper'

# TODO: fix!
$load_fixture_dir = (File.dirname(__FILE__) + '/../../ruby/fixtures/load')
$LOAD_PATH << $load_fixture_dir
$LOAD_PATH << (File.dirname(__FILE__) + '/../../ruby/fixtures/load/load_spec_rba.rba')

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
    # Kernel.compile will not work correctly from that dir so no chdir here
    system "rm #{$load_fixture_dir}/*.rbc 2>/dev/null"

    Kernel.compile($load_fixture_dir + '/load_spec_10.rb')
    Dir.chdir($load_fixture_dir) do |dir|
      system "rm load_spec_rba.rba 2>/dev/null"
      system "ar r load_spec_rba.rba load_spec_10.rbc 2>/dev/null"
    end
  end

  after :all do
    Dir.glob("#{$load_fixture_dir}/*.rbc") {|f| File.delete f }
    Dir.glob("#{$load_fixture_dir}/load_dynamic.rb*") {|f| File.delete f }
  end

  before :each do
    Dir.glob("#{$load_fixture_dir}/load_dynamic.rb*") {|f| File.delete f }
  end

  it "loads a .rbc file directly" do
    load('load_spec_2.rb').should == true
    load('load_spec_2.rbc').should == true
  end

  it "compiles a new .rbc file if one does not exist" do
    `rm -f #{$load_fixture_dir}/load_spec_2.rbc`

    load('load_spec_2.rb').should == true

    File.exist?("#{$load_fixture_dir}/load_spec_2.rbc").should == true
  end

  it "generates a <name>.compiled.rbc but no .rb file if using a file with no extension" do
    `rm -f #{$load_fixture_dir}/load_spec.rbc`

    load('load_spec').should == true
    File.exist?("#{$load_fixture_dir}/load_spec.compiled.rbc").should == true
  end

  it "generates a <name>.compiled.rbc file if using a file with some arbitrary extension" do
    `rm -f #{$load_fixture_dir}/load_spec.rooby.rbc`

    load('load_spec.rooby').should == true
    File.exist?("#{$load_fixture_dir}/load_spec.rooby.compiled.rbc").should == true
  end

  it "loads the .rbc if the .rb does not exist" do
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

  it "loads the .rbc file unless the .rb file is newer by modification time" do
    File.open("#{$load_fixture_dir}/load_spec_dynamic.rb", "w+") do |f|
      f.puts "$load_spec_dynamic = 1"
    end
    Kernel.compile "#{$load_fixture_dir}/load_spec_dynamic.rb"
    system "mv #{$load_fixture_dir}/load_spec_dynamic.rbc #{$load_fixture_dir}/lsd.old"

    File.open("#{$load_fixture_dir}/load_spec_dynamic.rb", "w+") do |f|
      f.puts "$load_spec_dynamic = 2"
    end

    load("load_spec_dynamic.rb").should == true
    $load_spec_dynamic.should == 2

    system "mv #{$load_fixture_dir}/lsd.old #{$load_fixture_dir}/load_spec_dynamic.rbc"

    system "touch -t 200701011800 #{$load_fixture_dir}/load_spec_dynamic.rb"
    system "touch -t 200801011800 #{$load_fixture_dir}/load_spec_dynamic.rbc"

    load("load_spec_dynamic.rb").should == true
    $load_spec_dynamic.should == 1

    system "touch -t 200701011800 #{$load_fixture_dir}/load_spec_dynamic.rbc"
    system "touch -t 200801011800 #{$load_fixture_dir}/load_spec_dynamic.rb"

    load("load_spec_dynamic.rb").should == true
    $load_spec_dynamic.should == 2

    # Smaller resolution
    system "touch -t 200801011800 #{$load_fixture_dir}/load_spec_dynamic.rbc"
    system "touch -t 200801011801 #{$load_fixture_dir}/load_spec_dynamic.rb"

    load("load_spec_dynamic.rb").should == true
    $load_spec_dynamic.should == 2
  end

  it "recompiles the file each time if the second parameter is a Hash with non-false :recompile" do
    File.open("#{$load_fixture_dir}/load_spec_dynamic.rb", "w+") do |f|
      f.puts "$load_spec_dynamic = 1"
    end
    Kernel.compile "#{$load_fixture_dir}/load_spec_dynamic.rb"
    system "mv #{$load_fixture_dir}/load_spec_dynamic.rbc #{$load_fixture_dir}/lsd.old"

    File.open("#{$load_fixture_dir}/load_spec_dynamic.rb", "w+") do |f|
      f.puts "$load_spec_dynamic = 2"
    end

    load("load_spec_dynamic.rb").should == true
    $load_spec_dynamic.should == 2

    system "mv #{$load_fixture_dir}/lsd.old #{$load_fixture_dir}/load_spec_dynamic.rbc"

    system "touch -t 200701011800 #{$load_fixture_dir}/load_spec_dynamic.rb"
    system "touch -t 200801011800 #{$load_fixture_dir}/load_spec_dynamic.rbc"

    load("load_spec_dynamic.rb", :recompile => true).should == true
    $load_spec_dynamic.should == 2
  end

  it "raises an error if the source file does not exist and :recompile is present" do
    File.open("#{$load_fixture_dir}/load_spec_dynamic.rb", "w+") do |f|
      f.puts "$load_spec_dynamic = 1"
    end
    Kernel.compile "#{$load_fixture_dir}/load_spec_dynamic.rb"
    system "rm #{$load_fixture_dir}/load_spec_dynamic.rb"

    lambda { load "load_spec_dynamic.rb", :recompile => true }.should raise_error
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


