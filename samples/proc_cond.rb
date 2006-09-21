def blah
    p :blah_top
    proc {
        p :proc_top
        next
    }
end

pr = blah()

while true
    p :while_top
    instance_eval(&pr)
end
