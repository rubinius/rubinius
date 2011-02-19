module Rubinius::JITCompiler
  class Analysis::MathLocals < Analysis::Pass
    def constant_math(local, val, op=:+)
      return false unless val.kind_of? Send
      return false unless val.method == op

      return false unless val.arg_count == 1

      rec = val.store_search_backward(val.receiver)
      arg = val.store_search_backward(val.receiver+1)

      return false unless rec.kind_of? GetLocal and rec.local == local
      return false unless arg.kind_of? SetSimple and arg.value.kind_of?(Fixnum)

      return [rec, arg]
    end

    def run(pc, blocks)
      unless li = pc.consume_local_info
        raise "No local info available, please add LocalInfo pass"
      end

      locals = li.locals

      locals.each do |idx,info|
        if init = info.find_init
          tro = init.intro
          if tro.kind_of? SetSimple and tro.value.kind_of? Fixnum
            if info.writes.size == 2
              update = info.writes.detect { |i| i != init }

              rec, arg = constant_math(idx, update.intro)
              if rec
                update(info, rec, arg, init, update)
              end
            end
          end
        end
      end
    end

    def update(info, rec, arg, init, update)
      stride = arg.value
      rec.remove!
      arg.remove!

      init.intro.remove!
      update.intro.remove!

      reg = init.use.local

      init.use.replace IntSetConstant.new(reg, init.intro.value)
      update.use.replace IntAdd.new(reg, reg, stride)

      info.reads.each do |r|
        unless r.intro == rec
          debug "r-intro: #{r.intro.asm}"
          debug "  r-use: #{r.use.asm}"
        end

        r2, a2 = constant_math(reg, r.use, :<)

        if r2
          goto = r.use.find_user
          if goto.kind_of? GotoIfFalse
            r2.remove!
            a2.remove!
            goto.remove!
            igoto = IntGotoEQGT.new(reg, a2.value, goto.dest_block,
                                   goto.else_block)
            r.use.replace igoto
          end
        end
      end
    end
  end
end
