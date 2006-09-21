class Blah
    def go
    end
end

b = Blah.new
p b.respond_to?(:go)
p b.respond_to?(:at)
