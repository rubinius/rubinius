module ReturnSpecs
  class Blocks
    def yielding_method
      yield
      ScratchPad.record :after_yield
    end

    def enclosing_method
      yielding_method do
        ScratchPad.record :before_return
        return :return_value
        ScratchPad.record :after_return
      end

      ScratchPad.record :after_call
    end
  end

  class NestedCalls < Blocks
    def invoking_method(&b)
      yielding_method(&b)
      ScratchPad.record :after_invoke
    end

    def enclosing_method
      invoking_method do
        ScratchPad.record :before_return
        return :return_value
        ScratchPad.record :after_return
      end
      ScratchPad.record :after_invoke
    end
  end

  class NestedBlocks < Blocks
    def enclosing_method
      yielding_method do
        yielding_method do
          ScratchPad.record :before_return
          return :return_value
          ScratchPad.record :after_return
        end
        ScratchPad.record :after_invoke1
      end
      ScratchPad.record :after_invoke2
    end
  end
end
