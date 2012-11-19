namespace rubinius {

  class Object;
  typedef void (*FinalizerFunction)(STATE, Object*);

  struct FinalizeObject {
  public:
    enum FinalizationStatus {
      eLive,
      eQueued,
      eFinalized
    };

  public:
    FinalizeObject()
      : object(NULL)
      , status(eLive)
      , queue_count(0)
      , finalizer(0)
      , ruby_finalizer(0)
    {}

    Object* object;
    FinalizationStatus status;
    int queue_count;
    FinalizerFunction finalizer;
    Object* ruby_finalizer;

    void queued() {
      status = eQueued;
    }
  };
}
