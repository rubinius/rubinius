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
end
