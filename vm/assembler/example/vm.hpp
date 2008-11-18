class RegisterVM {
public:

  enum Instructions {
    add,
    set,
    show,
    exit,
    jump,
    jump_if_equal
  };

  void run(int* op);

  typedef void (*Function)(int* registers);

  Function compile(int* op, int size);
};


