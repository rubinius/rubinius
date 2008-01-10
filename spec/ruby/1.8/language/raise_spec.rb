require File.dirname(__FILE__) + '/../spec_helper'

describe "Exceptions" do
  it "raise should abort execution" do
    lambda { 
      begin
        raise ArgumentError, "you don't know what you're talking about"
      rescue ArgumentError => e
        e.message.should == "you don't know what you're talking about"
        raise
      end
    }.should raise_error(ArgumentError)
  end
  
  # FIXME: code string is only necessary because ensure crashes shotgun
  it "ensure should execute when exception is raised" do
    module RaiseSpecs
      class A
        def exception
          begin
            raise ArgumentError, "exception"
          rescue Exception => @e
            # pass
          ensure
            @a = 'ensure ' << @e
          end
          @a
        end
      end
    end

    RaiseSpecs::A.new.exception.should == "ensure exception"
  end
  
  # FIXME: code string is only necessary because ensure crashes shotgun
  it "ensure should execute when exception is not raised" do
    class B
      def exception
        begin
          @e = 'I never got to be an exception'
        rescue Exception => @e
          @e.message
        ensure
          @a = 'ensure ' << @e
        end
        return @a
      end
    end

    B.new.exception.should == "ensure I never got to be an exception"
  end

  it "the result of ensure should be elided" do
    begin
      true
    ensure
      false
    end.should == true
  end

  it "the result of else should be returned when no exception is raised" do
    begin
      true
    rescue
      5
    else
      6
    end.should == 6
  end
  
  it "the result of else should be returned when no exception is raised, even with an ensure" do
    begin
      true
    rescue
      5
    else
      6
    ensure
      7
    end.should == 6
  end

  it "the result of else should be returned even if the body is empty" do
    begin
    rescue
      1
    else
      2
    end.should == 2
  end

  it "retry should restart execution at begin" do
    class C
      def exception
        @ret = []
        @count = 1
        begin
          @ret << @count
          raise ArgumentError, 'just kidding' unless @count > 3
        rescue Exception => @e
          @count += 1
          retry
        else
          @ret << 7
        ensure
          @ret << @count
        end
        @ret
      end
    end

    C.new.exception.should == [1, 2, 3, 4, 7, 4]
  end

  it "on a single line, a default can be assigned on exception" do
    variable = [1,2,3].frist rescue 'exception'
    variable.should == 'exception'
  end

  it "that exceptions are not cleared exiting a thread" do
    lambda { Thread.new { raise RangeError }.join }.should raise_error(RangeError)
  end
  
  it "that StandardError is the default rescue class" do
    begin
      @ret = ''
      begin
        raise Exception, 'hey hey hey !'
      rescue => ex
        @ret = 'intercepted'
      end
    rescue Exception => ex
      @ret = 'not intercepted'
    end.should == 'not intercepted'

    begin
      @ret = ''
      begin
        raise StandardError, 'hey hey hey !'
      rescue => ex
        @ret = 'intercepted'
      end
    rescue Exception => ex
      @ret = 'not intercepted'
    end.should == 'intercepted'
  end

  it "that RuntimeError is the default raise class" do
    begin
      @ret = ''
      raise
    rescue => ex
      @ret = ex.class.to_s
    end.should == 'RuntimeError'
  end

  it "that $! is cleared when an exception is rescued" do
    begin
      raise
    rescue
    end
    $!.should == nil
  end

  EXCEPTION_TREE = [
    :Exception, [
      :ScriptError, [
        :LoadError,
        :NotImplementedError,
        :SyntaxError
      ],
      :SignalException, [
        :Interrupt
      ],
      :StandardError, [ # default for rescue
        :ArgumentError,
        :IOError, [
          :EOFError
        ],
        :IndexError,
        :LocalJumpError,
        :NameError, [
          :NoMethodError
        ],
        :RangeError, [
          :FloatDomainError
        ],
        :RegexpError,
        :RuntimeError, # default for raise
        :SecurityError,
        :SystemCallError, # FIXME : Errno::*  missing
        :SystemStackError,
        :ThreadError,
        :TypeError,
        :ZeroDivisionError
      ],
      :SystemExit
    ]
  ]

  @exception_stack = []
  @last_exception  = nil

  generate_exception_existance_spec = lambda do |exception_name|
    it "exception #{exception_name} is in the core" do
      Object.const_defined?(exception_name).should == true
    end
  end

  generate_exception_ancestor_spec = lambda do |exception_name, parent_name|
    it "#{exception_name} has #{parent_name} as ancestor" do
      exception = Object.const_get(exception_name.to_sym)
      ancestors = exception.ancestors.map { |x| x.to_s }
      ancestors.include?(parent_name.to_s).should == true
    end
  end

  build_spec_tree = lambda do |tree|
    tree.each do |element|
      case element
      when Array
        if @exception_stack
          @exception_stack.push(@last_exception)
          build_spec_tree.call(element)
          @exception_stack.pop()
          @last_exception = nil
        else
          raise 'Spec generation error, this case should never occur'
        end
      else
        generate_exception_existance_spec.call(element)
        @exception_stack.each do |parent_name|
          generate_exception_ancestor_spec.call(element, parent_name)
        end
        @last_exception = element
      end
    end
  end

  build_spec_tree.call(EXCEPTION_TREE)
  
  
end

