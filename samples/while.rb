$g = 0
def eek
    p :top
    until $g == 10
        p :inner
        $g += 1
        redo
    end

    p :bottom
end

eek
