require File.dirname(__FILE__) + '/../spec_helper'

context "Exceptions" do
  specify "raise should abort execution" do
    example do
      @a = []
      begin
        raise ArgumentError, "exception"
        @a << "raise failed"
      rescue Exception => @e
        @a << @e.message
      end
      @a
    end.should == ["exception"]
  end
  
  # FIXME: code string is only necessary because ensure crashes shotgun
  specify "ensure should execute when exception is raised" do
    example do
      class Foo
        def exception
          begin
            raise ArgumentError, "exception"
          rescue Exception => @e
            @e.message
          ensure
            @a = 'ensure ' << @e
          end
          @a
        end
      end

      Foo.new.exception
    end.should == "ensure exception"
  end
  
  # FIXME: code string is only necessary because ensure crashes shotgun
  specify "ensure should execute when exception is not raised" do
    example do
      class Foo
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

      Foo.new.exception
    end.should == "ensure I never got to be an exception"
  end
  
  # FIXME: code string is only necessary because ensure crashes shotgun
  specify "retry should restart execution at begin" do
    example do
      class Foo
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

      Foo.new.exception
    end.should == [1, 2, 3, 4, 7, 4]
  end

  specify "on a single line, a default can be assigned on exception" do
    example do
      variable = [1,2,3].frist rescue 'exception'
    end.should == 'exception'
  end

  specify "that StandardError is the default rescue class" do
    example do
      @ret = ''
      begin
        begin
          raise Exception, 'hey hey hey !'
        rescue => ex
          @ret = 'intercepted'
        end
      rescue Exception => ex
        @ret = 'not intercepted'
      end
    end.should == 'not intercepted'
    example do
      @ret = ''
      begin
        begin
          raise StandardError, 'hey hey hey !'
        rescue => ex
          @ret = 'intercepted'
        end
      rescue Exception => ex
        @ret = 'not intercepted'
      end
    end.should == 'intercepted'
  end

  specify "that RuntimeError is the default raise class" do
    example do
      @ret = ''
      begin
        raise
      rescue => ex
        @ret = ex.class.to_s
      end
      @ret
    end.should == 'RuntimeError'
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
    specify "exception #{exception_name} is in the core" do
      example do
        Object.const_defined?(exception_name)
      end.should === true
    end
  end

  generate_exception_ancestor_spec = lambda do |exception_name, parent_name|
    specify "#{exception_name} has #{parent_name} as ancestor" do
      example do
        exception = Object.const_get(exception_name)
        exception.ancestors.map{|x| x.to_s}.include?(parent_name.to_s)
      end.should === true
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

