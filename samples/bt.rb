class Blah
    def one
        two
    end

    def two
        three
    end

    def three
        four
    end

    def four
        fuck
    end
end

b = Blah.new
begin
    b.one
rescue Object => e
    p e
    puts e.backtrace.show
end
