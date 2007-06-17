blah do |a,b|
    c = a + b
    go do |d|
        c = c + d
    end
end
