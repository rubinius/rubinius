module EnsureSpec

  class Test

    def initialize
      @values = []
    end

    attr_reader :values
  
    def call_block
      begin
        @values << :start
        yield
      ensure
        @values << :end
      end
    end

    def do_test
      call_block do
        @values << :in_block
        return :did_test
      end
    end
  end
end
