# This file specifies behaviour for the methods of
# Continuation. The mechanics thereof may be further
# examined in spec/language.
#
require File.dirname(__FILE__) + '/../../spec_helper'

ruby_version_is "1.9" do
  require 'continuation'
end

# Class methods
#   -
#
# Instance methods
#   #call             OK
#   #[]               OK

module ContinuationSpecs
  def self.create_cc
    Kernel.callcc { |cc| return cc }
    :create_cc
  end
end

not_supported_on :ironruby do
  describe "Creating a Continuation object" do
    it "must be done through Kernel.callcc, no .new" do
      lambda { Continuation.new }.should raise_error(NoMethodError)

      cont = ContinuationSpecs.create_cc
      cont.class.should == Continuation
    end
  end
end


not_supported_on :ironruby do
  describe "Executing a Continuation" do
    it "using #call transfers execution to right after the Kernel.callcc block" do
      array = [:reached, :not_reached]

      cont = ContinuationSpecs.create_cc

      unless array.first == :not_reached
        array.shift
        cont.call
      end

      array.should == [:not_reached]
    end

    it "arguments given to #call (or nil) are returned by the Kernel.callcc block (as Array unless only one object)" do
      Kernel.callcc {|cc| cc.call}.should == nil
      Kernel.callcc {|cc| cc.call 1}.should == 1
      Kernel.callcc {|cc| cc.call 1, 2, 3}.should == [1, 2, 3]
    end

    it "#[] is an alias for #call" do
      Kernel.callcc {|cc| cc.call}.should == Kernel.callcc {|cc| cc[]}
      Kernel.callcc {|cc| cc.call 1}.should == Kernel.callcc {|cc| cc[1]}
      Kernel.callcc {|cc| cc.call 1, 2, 3}.should == Kernel.callcc {|cc| cc[1, 2, 3]}
    end

    it "closes over lexical environments" do
      o = Object.new
      def o.f; a = 1; Kernel.callcc {|c| a = 2; c.call }; a; end
      o.f().should == 2
    end

    it "escapes an inner ensure block" do
      a = []
      cont = nil
      a << :pre_callcc
      Kernel.callcc do |cc|
        a << :in_callcc
        cont = cc
      end
      a << :post_callcc
      unless a.include? :pre_call
        begin
          a << :pre_call
          cont.call
          a << :post_call
        ensure
          a << :ensure
        end
      end
      a.should == [:pre_callcc,:in_callcc,:post_callcc,:pre_call,:post_callcc]
    end

    it "executes an outer ensure block" do
      a = []
      cont = nil
      begin
        a << :pre_callcc
        Kernel.callcc do |cc|
          a << :in_callcc
          cont = cc
        end
        a << :post_callcc
        unless a.include? :pre_call
          a << :pre_call
          cont.call
          a << :post_call
        end
      ensure
        a << :ensure
      end
      a.should == [:pre_callcc,:in_callcc,:post_callcc,:pre_call,:post_callcc,:ensure]
    end
  end
end
