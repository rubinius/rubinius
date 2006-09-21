class Blah
    def go
        yield 7
    end

    def done
        true
    end
end

b = Blah.new

b.go { |i| b.done }
