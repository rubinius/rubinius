class Blah
    def f
        i = 0
        while i < 5
            out = yield
            p out
            i += 1
        end
    end
    
def blah
    ary = [1,2,3,4,5,6]
    ary.each do |i|
        next unless i > 3
        p i
    end
end
end

b = Blah.new
b.f { 
    next 8
    10
}
b.blah
