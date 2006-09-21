class Blah
    def woo(one, two, three)
        p one
        puts two
        puts three
        # puts a
        # puts a.tuple.at(0)
    end

    def rego(*a)
        woo(*a)
    end
end

b = Blah.new
b.rego 1, 2, 3
