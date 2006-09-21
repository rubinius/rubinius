$g = 0
def eek
    p :top
    p $g
    if true
        p :inner
        $g += 1
        redo
    end

    p :bottom
end

eek
