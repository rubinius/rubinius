class Blah
    def go
        yield 7
    end
end

b = Blah.new
b.go { |i| puts i }
