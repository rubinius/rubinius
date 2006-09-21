$g = 0
def eek
    p :top
    begin
        p :body
        raise "ee"
    rescue
        p :inner
        [1].each do |a|
            p a
            $g += 1
            p 8
            exit! if $g == 10
            retry
        end
    end

    p :bottom
end

eek
