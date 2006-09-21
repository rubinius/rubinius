def blah
    p :blah_top
    yield
    p :blah_bottom
end

def eek
    p :top
    i = 0
    out = blah {
        i += 1
        p i
        retry
    }
    p out
    p :bottom
end

eek
