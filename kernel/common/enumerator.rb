# A class which provides a method `each' to be used as an Enumerable
# object.

module Enumerable
  class Enumerator
    include Enumerable

    def initialize(obj, iter=:each, *args)
      @object = obj
      @iter = iter.to_sym
      @args = args
      @generator = nil
    end

    def each(&block)
      @object.__send__(@iter, *@args, &block)
    end

    ##
    # :call-seq:
    #   enum.each_with_index(*arg){ |obj, i| block }  -> enum or enumerator
    #
    # Calls +block+ with two arguments, the item and its index, for
    # each item in +enum+.
    #
    #   hash = {}
    #   %w[cat dog wombat].each_with_index { |item, index|
    #     hash[item] = index
    #   }
    #
    #   p hash   #=> {"cat"=>0, "wombat"=>2, "dog"=>1}

    def each_with_index
      return to_enum(:each_with_index) unless block_given?

      idx = 0

      each do |o|
        val = yield(o, idx)
        idx += 1
        val
      end
    end

    alias_method :enum_with_index, :each_with_index
    alias_method :with_index, :each_with_index

    if Rubinius::Fiber::ENABLED
      class FiberGenerator
        def initialize(obj)
          @object = obj
          rewind
        end

        def next?
          !@done
        end

        def next
          reset unless @fiber

          val = @fiber.resume

          raise StopIteration, "iteration has ended" if @done

          return val
        end

        def rewind
          @fiber = nil
          @done = false
        end

        def reset
          @done = false
          @fiber = Rubinius::Fiber.new(0) do
            obj = @object
            obj.each do |val|
              Rubinius::Fiber.yield val
            end
            @done = true
          end
        end
      end
    else
      FiberGenerator = nil
    end

    class ThreadGenerator
      def initialize(enum, obj, meth, args)
        @object = obj
        @method = meth
        @args = args

        ObjectSpace.define_finalizer(enum, method(:kill))

        rewind
      end

      # Used to cleanup the background thread when the enumerator
      # is GC'd.
      def kill(obj_id)
        if @thread
          @thread.kill
        end
      end

      def next?
        if @done
          @thread.join if @thread
          @thread = nil
          return false
        end

        true
      end

      def next
        reset unless @thread

        @hold_channel << nil
        val = @channel.receive

        raise StopIteration, "iteration has ended" if @done

        return val
      end

      def rewind
        if @thread
          @thread.kill
        end

        @done = false
        @thread = nil
      end

      def reset
        @done = false
        @channel = Rubinius::Channel.new
        @hold_channel = Rubinius::Channel.new

        @thread = Thread.new do
          @object.__send__(@method, *@args) do |val|
            @hold_channel.receive
            @channel << val
          end

          # Hold to indicate done to avoid race conditions setting
          # the ivar.
          @hold_channel.receive
          @done = true

          # Put an extra value in the channel, so that main
          # thread doesn't accidentally block if it doesn't
          # detect @done in time.
          @channel << nil
        end
      end
    end
  end
end

module Kernel
  def to_enum(method=:each, *args)
    Enumerable::Enumerator.new(self, method, *args)
  end

  alias_method :enum_for, :to_enum
end
