class Blah
    def go(&block)
        p block
        block.call
    end
end

b = Blah.new
b.go { puts "in block!" }
